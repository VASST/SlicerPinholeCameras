import os
import vtk
import qt
import slicer
import numpy as np
import logging
from slicer.ScriptedLoadableModule import ScriptedLoadableModule, ScriptedLoadableModuleWidget, ScriptedLoadableModuleLogic, ScriptedLoadableModuleTest

# PinholeCameraCalibration
class PinholeCameraCalibration(ScriptedLoadableModule):
  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "PinholeCamera Calibration"
    self.parent.categories = ["Computer Vision"]
    self.parent.dependencies = ["PinholeCameras", "PointToLineRegistration", "Annotations"]
    self.parent.contributors = ["Adam Rankin (Robarts Research Institute)"]
    self.parent.helpText = """This module utilizes OpenCV camera calibration functions to perform intrinsic calibration and calibration to an external tracker using a tracked, calibrated stylus. """ + self.getDefaultModuleDocumentationLink()
    self.parent.acknowledgementText = """This module was developed with support from the Natural Sciences and Engineering Research Council of Canada, the Canadian Foundation for Innovation, and the Virtual Augmentation and Simulation for Surgery and Therapy laboratory, Western University."""

# PinholeCameraCalibrationWidget
class PinholeCameraCalibrationWidget(ScriptedLoadableModuleWidget):
  @staticmethod
  def get(widget, objectName):
    if widget.objectName == objectName:
      return widget
    else:
      for w in widget.children():
        resulting_widget = PinholeCameraCalibrationWidget.get(w, objectName)
        if resulting_widget:
          return resulting_widget
      return None

  @staticmethod
  def areSameVTK4x4(a, b):
    for i in range(0, 4):
      for j in range(0, 4):
        if a.GetElement(i,j) != b.GetElement(i,j):
          return False
    return True

  @staticmethod
  def vtk4x4ToNumpy(vtk4x4):
    if vtk4x4 is None:
      return

    val = np.asmatrix(np.eye(4, 4, dtype=np.float64))
    for i in range(0, 4):
      for j in range(0, 4):
        val[i, j] = vtk4x4.GetElement(i, j)

    return val

  @staticmethod
  def vtk3x3ToNumpy(vtk3x3):
    if vtk3x3 is None:
      return

    val = np.asmatrix(np.eye(3, 3, dtype=np.float64))
    for i in range(0, 3):
      for j in range(0, 3):
        val[i, j] = vtk3x3.GetElement(i, j)

    return val

  @staticmethod
  def loadPixmap(param, x, y):
    iconPath = os.path.join(os.path.dirname(slicer.modules.pinholecameracalibration.path), 'Resources/Icons/', param + ".png")
    icon = qt.QIcon(iconPath)
    return icon.pixmap(icon.actualSize(qt.QSize(x, y)))

  def __init__(self, parent):
    ScriptedLoadableModuleWidget.__init__(self, parent)

    global SVC_OPENCV2_AVAILABLE
    global SVC_ARUCO_AVAILABLE
    try:
      global cv2
      import cv2
      SVC_OPENCV2_AVAILABLE = True
    except ImportError:
      SVC_OPENCV2_AVAILABLE = False

    if not SVC_OPENCV2_AVAILABLE:
      logging.error("OpenCV2 python interface not available.")
      return

    try:
      global aruco
      import cv2.aruco as aruco
      SVC_ARUCO_AVAILABLE = True
    except ImportError:
      SVC_ARUCO_AVAILABLE = False

    if not SVC_ARUCO_AVAILABLE:
      logging.warning("Aruco python interface not available.")

    self.logic = PinholeCameraCalibrationLogic()
    self.markupsLogic = slicer.modules.markups.logic()

    self.canSelectFiducials = True
    self.isManualCapturing = False
    self.rayList = []
    self.invertImage = False

    self.markupsNode = None
    self.centerFiducialSelectionNode = None
    self.copyNode = None
    self.imageGridNode = None
    self.trivialProducer = None
    self.widget = None
    self.videoCameraIntrinWidget = None
    self.videoCameraSelector = None

    self.inputsContainer = None
    self.trackerContainer = None
    self.intrinsicsContainer = None
    self.autoSettingsContainer = None
    self.checkerboardContainer = None
    self.flagsContainer = None
    self.checkerboardFlags = None
    self.circleGridFlags = None

    self.arucoContainer = None
    self.arucoMarkerSizeSpinBox = None
    self.arucoMarkerSeparationSpinBox = None

    self.charucoContainer = None
    self.charucoSquareSizeSpinBox = None
    self.charucoMarkerSizeSpinBox = None

    # Observer tags
    self.stylusTipTransformObserverTag = None
    self.pointModifiedObserverTag = None

    # Inputs
    self.imageSelector = None
    self.stylusTipTransformSelector = None

    self.stylusTipTransformNode = None

    self.okPixmap = PinholeCameraCalibrationWidget.loadPixmap('icon_Ok', 20, 20)
    self.notOkPixmap = PinholeCameraCalibrationWidget.loadPixmap('icon_NotOk', 20, 20)

    # Tracker
    self.manualButton = None
    self.semiAutoButton = None
    self.manualModeButton = None
    self.autoModeButton = None
    self.semiAutoModeButton = None
    self.autoButton = None
    self.resetButton = None
    self.resetPtLButton = None
    self.trackerResultsLabel = None
    self.captureCountSpinBox = None
    self.stylusTipTransformStatusLabel = None

    # Intrinsics
    self.capIntrinsicButton = None
    self.intrinsicCheckerboardButton = None
    self.intrinsicCircleGridButton = None
    self.intrinsicArucoButton = None
    self.intrinsicCharucoButton = None
    self.adaptiveThresholdButton = None
    self.normalizeImageButton = None
    self.filterQuadsButton = None
    self.fastCheckButton = None
    self.symmetricButton = None
    self.asymmetricButton = None
    self.squareSizeDoubleSpinBox = None
    self.clusteringButton = None
    self.invertImageButton = None
    self.arucoDictComboBox = None
    self.arucoDictContainer = None
    self.calibrateButton = None

    self.columnsSpinBox = None
    self.rowsSpinBox = None

    # Results
    self.labelResult = None
    self.labelPointsCollected = None

    self.videoCameraOriginInReference = None
    self.stylusTipToPinholeCamera = vtk.vtkMatrix4x4()
    self.IdentityMatrix = vtk.vtkMatrix4x4()

  def setup(self):
    ScriptedLoadableModuleWidget.setup(self)

    if not SVC_OPENCV2_AVAILABLE:
      self.layout.addWidget(qt.QLabel("OpenCV2 python is required and not available. Check installation/configuration of SlicerOpenCV."))
    else:
      # Load the UI From file
      scriptedModulesPath = eval('slicer.modules.%s.path' % self.moduleName.lower())
      scriptedModulesPath = os.path.dirname(scriptedModulesPath)
      path = os.path.join(scriptedModulesPath, 'Resources', 'UI', 'q' + self.moduleName + 'Widget.ui')
      self.widget = slicer.util.loadUI(path)
      self.layout.addWidget(self.widget)

      # Camera UI
      layout = PinholeCameraCalibrationWidget.get(self.widget, "gridLayout")
      placeholder = PinholeCameraCalibrationWidget.get(self.widget, "placeholder")
      layout.removeWidget(placeholder)
      self.videoCameraIntrinWidget = slicer.qMRMLPinholeCameraIntrinsicsWidget()
      self.videoCameraIntrinWidget.setMRMLScene(slicer.mrmlScene)
      layout.addWidget(self.videoCameraIntrinWidget, 0, 0)

      # Workaround for PinholeCamera selector
      self.videoCameraSelector = self.videoCameraIntrinWidget.children()[1].children()[1]

      # Inputs
      self.imageSelector = PinholeCameraCalibrationWidget.get(self.widget, "comboBox_ImageSelector")
      self.stylusTipTransformSelector = PinholeCameraCalibrationWidget.get(self.widget, "comboBox_StylusTipSelector")

      # Tracker calibration members
      self.inputsContainer = PinholeCameraCalibrationWidget.get(self.widget, "collapsibleButton_Inputs")
      self.trackerContainer = PinholeCameraCalibrationWidget.get(self.widget, "collapsibleButton_Tracker")
      self.intrinsicsContainer = PinholeCameraCalibrationWidget.get(self.widget, "collapsibleButton_Intrinsics")
      self.checkerboardContainer = PinholeCameraCalibrationWidget.get(self.widget, "widget_ContainerCheckerboard")
      self.flagsContainer = PinholeCameraCalibrationWidget.get(self.widget, "widget_ContainerFlags")
      self.checkerboardFlags = PinholeCameraCalibrationWidget.get(self.widget, "widget_CheckerboardFlags")
      self.circleGridFlags = PinholeCameraCalibrationWidget.get(self.widget, "widget_CircleGridFlags")

      self.manualButton = PinholeCameraCalibrationWidget.get(self.widget, "pushButton_Manual")
      self.semiAutoButton = PinholeCameraCalibrationWidget.get(self.widget, "pushButton_SemiAuto")
      self.autoButton = PinholeCameraCalibrationWidget.get(self.widget, "pushButton_Automatic")
      self.manualModeButton = PinholeCameraCalibrationWidget.get(self.widget, "radioButton_Manual")
      self.semiAutoModeButton = PinholeCameraCalibrationWidget.get(self.widget, "radioButton_SemiAuto")
      self.autoModeButton = PinholeCameraCalibrationWidget.get(self.widget, "radioButton_Automatic")
      self.autoSettingsContainer = PinholeCameraCalibrationWidget.get(self.widget, "groupBox_AutoSettings")
      self.resetPtLButton = PinholeCameraCalibrationWidget.get(self.widget, "pushButton_resetPtL")
      self.trackerResultsLabel = PinholeCameraCalibrationWidget.get(self.widget, "label_TrackerResultsValue")
      self.captureCountSpinBox = PinholeCameraCalibrationWidget.get(self.widget, "spinBox_captureCount")
      self.stylusTipTransformStatusLabel = PinholeCameraCalibrationWidget.get(self.widget, "label_StylusTipToCamera_Status")

      # Intrinsic calibration members
      self.capIntrinsicButton = PinholeCameraCalibrationWidget.get(self.widget, "pushButton_CaptureIntrinsic")
      self.resetButton = PinholeCameraCalibrationWidget.get(self.widget, "pushButton_Reset")
      self.intrinsicCheckerboardButton = PinholeCameraCalibrationWidget.get(self.widget, "radioButton_IntrinsicCheckerboard")
      self.intrinsicCircleGridButton = PinholeCameraCalibrationWidget.get(self.widget, "radioButton_IntrinsicCircleGrid")
      self.intrinsicArucoButton = PinholeCameraCalibrationWidget.get(self.widget, "radioButton_IntrinsicAruco")
      self.intrinsicCharucoButton = PinholeCameraCalibrationWidget.get(self.widget, "radioButton_IntrinsicCharuco")
      self.columnsSpinBox = PinholeCameraCalibrationWidget.get(self.widget, "spinBox_Columns")
      self.rowsSpinBox = PinholeCameraCalibrationWidget.get(self.widget, "spinBox_Rows")
      self.squareSizeDoubleSpinBox = PinholeCameraCalibrationWidget.get(self.widget, "doubleSpinBox_SquareSize")
      self.adaptiveThresholdButton = PinholeCameraCalibrationWidget.get(self.widget, "checkBox_AdaptiveThreshold")
      self.normalizeImageButton = PinholeCameraCalibrationWidget.get(self.widget, "checkBox_NormalizeImage")
      self.filterQuadsButton = PinholeCameraCalibrationWidget.get(self.widget, "checkBox_FilterQuads")
      self.fastCheckButton = PinholeCameraCalibrationWidget.get(self.widget, "checkBox_FastCheck")
      self.symmetricButton = PinholeCameraCalibrationWidget.get(self.widget, "radioButton_SymmetricGrid")
      self.asymmetricButton = PinholeCameraCalibrationWidget.get(self.widget, "radioButton_AsymmetricGrid")
      self.clusteringButton = PinholeCameraCalibrationWidget.get(self.widget, "checkBox_Clustering")
      self.invertImageButton = PinholeCameraCalibrationWidget.get(self.widget, "checkBox_ImageInvert")
      self.arucoDictComboBox = PinholeCameraCalibrationWidget.get(self.widget, "comboBox_arucoDict")
      self.arucoDictContainer = PinholeCameraCalibrationWidget.get(self.widget, "widget_arucoDictContainer")
      self.arucoContainer = PinholeCameraCalibrationWidget.get(self.widget, "widget_arucoArgs")
      self.arucoMarkerSizeSpinBox = PinholeCameraCalibrationWidget.get(self.widget, "doubleSpinBox_markerSize")
      self.arucoMarkerSeparationSpinBox = PinholeCameraCalibrationWidget.get(self.widget, "doubleSpinBox_markerSeparation")
      self.charucoContainer = PinholeCameraCalibrationWidget.get(self.widget, "widget_charucoArgs")
      self.charucoSquareSizeSpinBox = PinholeCameraCalibrationWidget.get(self.widget, "doubleSpinBox_charucoSquareSize")
      self.charucoMarkerSizeSpinBox = PinholeCameraCalibrationWidget.get(self.widget, "doubleSpinBox_charucoMarkerSize")
      self.calibrateButton = PinholeCameraCalibrationWidget.get(self.widget, "pushButton_Calibrate")

      # Results
      self.labelResult = PinholeCameraCalibrationWidget.get(self.widget, "label_ResultValue")
      self.labelPointsCollected = PinholeCameraCalibrationWidget.get(self.widget, "label_PointsCollected")

      # Disable capture as image processing isn't active yet
      self.trackerContainer.setEnabled(False)
      self.intrinsicsContainer.setEnabled(False)

      # UI file method does not do mrml scene connections, do them manually
      self.videoCameraIntrinWidget.setMRMLScene(slicer.mrmlScene)
      self.imageSelector.setMRMLScene(slicer.mrmlScene)
      self.stylusTipTransformSelector.setMRMLScene(slicer.mrmlScene)

      # Inputs
      self.imageSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
      self.stylusTipTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onStylusTipTransformSelected)

      # Connections
      self.capIntrinsicButton.connect('clicked(bool)', self.onIntrinsicCapture)
      self.resetButton.connect('clicked(bool)', self.onReset)
      self.intrinsicCheckerboardButton.connect('clicked(bool)', self.onIntrinsicModeChanged)
      self.intrinsicCircleGridButton.connect('clicked(bool)', self.onIntrinsicModeChanged)
      self.intrinsicArucoButton.connect('clicked(bool)', self.onIntrinsicModeChanged)
      self.intrinsicCharucoButton.connect('clicked(bool)', self.onIntrinsicModeChanged)
      self.rowsSpinBox.connect('valueChanged(int)', self.onPatternChanged)
      self.columnsSpinBox.connect('valueChanged(int)', self.onPatternChanged)
      self.arucoMarkerSizeSpinBox.connect('valueChanged(double)', self.onPatternChanged)
      self.arucoMarkerSeparationSpinBox.connect('valueChanged(double)', self.onPatternChanged)
      self.charucoSquareSizeSpinBox.connect('valueChanged(double)', self.onPatternChanged)
      self.charucoMarkerSizeSpinBox.connect('valueChanged(double)', self.onPatternChanged)
      self.captureCountSpinBox.connect('valueChanged(int)', self.onCaptureCountChanged)
      self.arucoDictComboBox.connect('currentIndexChanged(int)', self.onArucoDictChanged)
      self.calibrateButton.connect('clicked(bool)', self.onCalibrateButtonClicked)

      self.manualButton.connect('clicked(bool)', self.onManualButton)
      self.semiAutoButton.connect('clicked(bool)', self.onSemiAutoButton)
      self.autoButton.connect('clicked(bool)', self.onAutoButton)
      self.manualModeButton.connect('clicked(bool)', self.onProcessingModeChanged)
      self.autoModeButton.connect('clicked(bool)', self.onProcessingModeChanged)
      self.resetPtLButton.connect('clicked(bool)', self.onResetPtL)

      self.adaptiveThresholdButton.connect('clicked(bool)', self.onFlagChanged)
      self.normalizeImageButton.connect('clicked(bool)', self.onFlagChanged)
      self.filterQuadsButton.connect('clicked(bool)', self.onFlagChanged)
      self.fastCheckButton.connect('clicked(bool)', self.onFlagChanged)
      self.symmetricButton.connect('clicked(bool)', self.onFlagChanged)
      self.asymmetricButton.connect('clicked(bool)', self.onFlagChanged)
      self.clusteringButton.connect('clicked(bool)', self.onFlagChanged)
      self.invertImageButton.connect('stateChanged(int)', self.onInvertImageChanged)

      # Choose red slice only
      lm = slicer.app.layoutManager()
      lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutOneUpRedSliceView)

      # Populate Aruco Dict combobox
      entries = dir(cv2.aruco)
      for entry in entries:
        if entry.find("DICT_") != -1:
          # Crop off initial DICT_ for easy keyboard jumping
          self.arucoDictComboBox.addItem(entry[entry.find('DICT_')+5:])

      #  Force the dictionary updating in the logic
      self.onArucoDictChanged()

      # Initialize pattern, etc..
      self.onIntrinsicModeChanged()

      # Refresh Apply button state
      self.updateUI()
      self.onProcessingModeChanged()

  def cleanup(self):
    self.onReset()
    self.onResetPtL()

    self.capIntrinsicButton.disconnect('clicked(bool)', self.onIntrinsicCapture)
    self.intrinsicCheckerboardButton.disconnect('clicked(bool)', self.onIntrinsicModeChanged)
    self.intrinsicCircleGridButton.disconnect('clicked(bool)', self.onIntrinsicModeChanged)
    self.intrinsicArucoButton.disconnect('clicked(bool)', self.onIntrinsicModeChanged)
    self.intrinsicCharucoButton.disconnect('clicked(bool)', self.onIntrinsicModeChanged)
    self.rowsSpinBox.disconnect('valueChanged(int)', self.onPatternChanged)
    self.columnsSpinBox.disconnect('valueChanged(int)', self.onPatternChanged)
    self.arucoMarkerSizeSpinBox.disconnect('valueChanged(double)', self.onPatternChanged)
    self.arucoMarkerSeparationSpinBox.disconnect('valueChanged(double)', self.onPatternChanged)
    self.charucoSquareSizeSpinBox.disconnect('valueChanged(double)', self.onPatternChanged)
    self.charucoMarkerSizeSpinBox.disconnect('valueChanged(double)', self.onPatternChanged)
    self.captureCountSpinBox.disconnect('valueChanged(int)', self.onCaptureCountChanged)
    self.arucoDictComboBox.disconnect('currentIndexChanged(int)', self.onArucoDictChanged)
    self.calibrateButton.disconnect('clicked(bool)', self.onCalibrateButtonClicked)

    self.imageSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
    self.stylusTipTransformSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.updateUI)

    self.manualButton.disconnect('clicked(bool)', self.onManualButton)
    self.semiAutoButton.disconnect('clicked(bool)', self.onSemiAutoButton)
    self.autoButton.disconnect('clicked(bool)', self.onAutoButton)
    self.manualModeButton.disconnect('clicked(bool)', self.onProcessingModeChanged)
    self.autoModeButton.disconnect('clicked(bool)', self.onProcessingModeChanged)
    self.resetPtLButton.disconnect('clicked(bool)', self.onResetPtL)

    self.adaptiveThresholdButton.disconnect('clicked(bool)', self.onFlagChanged)
    self.normalizeImageButton.disconnect('clicked(bool)', self.onFlagChanged)
    self.filterQuadsButton.disconnect('clicked(bool)', self.onFlagChanged)
    self.fastCheckButton.disconnect('clicked(bool)', self.onFlagChanged)
    self.symmetricButton.disconnect('clicked(bool)', self.onFlagChanged)
    self.asymmetricButton.disconnect('clicked(bool)', self.onFlagChanged)
    self.clusteringButton.disconnect('clicked(bool)', self.onFlagChanged)
    self.invertImageButton.disconnect('stateChanged(int)', self.onInvertImageChanged)

  def onReset(self):
    self.logic.resetIntrinsic()
    self.labelResult.text = "Reset."
    self.videoCameraIntrinWidget.GetCurrentNode().SetAndObserveIntrinsicMatrix(vtk.vtkMatrix3x3().Identity())
    self.videoCameraIntrinWidget.GetCurrentNode().SetNumberOfDistortionCoefficients(5)
    for i in range(0,5):
      self.videoCameraIntrinWidget.GetCurrentNode().SetDistortionCoefficientValue(i, 0.0)

  def onResetPtL(self):
    self.rayList = []
    self.logic.resetMarkerToSensor()
    self.trackerResultsLabel.text = "Reset."

  def onImageSelected(self):
    # Set red slice to the copy node
    if self.imageSelector.currentNode() is not None:
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.imageSelector.currentNode().GetID())
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().FitSliceToAll()
      slicer.app.layoutManager().sliceWidget('Red').sliceController().rotateSliceToLowestVolumeAxes() # If the image is not RAS aligned, we want to show it to the user anyways

      # Check pixel spacing, x and y must be 1px = 1mm in order for markups to produce correct pixel locations
      spacing = self.imageSelector.currentNode().GetImageData().GetSpacing()
      if spacing[0] != 1.0 or spacing[1] != 1.0:
        self.labelResult.text = "Image does not have 1.0 spacing in x or y, markup fiducials will not represent pixels exactly! Cannot proceed."
        self.canSelectFiducials = False
      else:
        self.canSelectFiducials = True

    self.updateUI()

  def onCaptureCountChanged(self):
    countString = str(self.logic.countMarkerToSensor()) + "/" + str(self.captureCountSpinBox.value) + " points captured."
    string = ""
    result = False

    if self.logic.countMarkerToSensor() >= self.captureCountSpinBox.value:
      result, videoCameraToImage, string = self.calcRegAndBuildString()

    if result:
      self.trackerResultsLabel.text = countString + " " + string
    else:
      self.trackerResultsLabel.text = countString

  def onFlagChanged(self):
    flags = 0
    if self.intrinsicCheckerboardButton.checked:
      if self.adaptiveThresholdButton.checked:
        flags = flags + cv2.CALIB_CB_ADAPTIVE_THRESH
      if self.normalizeImageButton.checked:
        flags = flags + cv2.CALIB_CB_NORMALIZE_IMAGE
      if self.filterQuadsButton.checked:
        flags = flags + cv2.CALIB_CB_FILTER_QUADS
      if self.fastCheckButton.checked:
        flags = flags + cv2.CALIB_CB_FAST_CHECK
    else:
      if self.symmetricButton.checked:
        flags = flags + cv2.CALIB_CB_SYMMETRIC_GRID
      if self.asymmetricButton.checked:
        flags = flags + cv2.CALIB_CB_ASYMMETRIC_GRID
      if self.clusteringButton.checked:
        flags = flags + cv2.CALIB_CB_CLUSTERING

    self.logic.setFlags(flags)

  def onInvertImageChanged(self, value):
    self.invertImage = self.invertImageButton.isChecked()

  def onPatternChanged(self, value):
    self.onIntrinsicModeChanged()

  def onIntrinsicCapture(self):
    vtk_im = self.imageSelector.currentNode().GetImageData()
    rows, cols, _ = vtk_im.GetDimensions()
    components = vtk_im.GetNumberOfScalarComponents()
    sc = vtk_im.GetPointData().GetScalars()
    im = vtk.util.numpy_support.vtk_to_numpy(sc)
    im = im.reshape(cols, rows, components)

    if self.intrinsicCheckerboardButton.checked:
      ret = self.logic.findCheckerboard(im, self.invertImage)
      _count = 0
      for i in range(0, len(self.logic.imagePoints)):
        _count = _count + len(self.logic.imagePoints[i])
      self.labelPointsCollected.text = _count
    elif self.intrinsicCircleGridButton.checked:
      ret = self.logic.findCircleGrid(im, self.invertImage)
      _count = 0
      for i in range(0, len(self.logic.imagePoints)):
        _count = _count + len(self.logic.imagePoints[i])
      self.labelPointsCollected.text = _count
    elif self.intrinsicArucoButton.checked:
      ret = self.logic.findAruco(im, self.invertImage)
      _count = 0
      for i in range(0, len(self.logic.arucoCorners)):
        _count = _count + len(self.logic.arucoCorners[i])
      self.labelPointsCollected.text = _count
    elif self.intrinsicCharucoButton.checked:
      ret = self.logic.findCharuco(im, self.invertImage)
      _count = 0
      for i in range(0, len(self.logic.charucoCorners)):
        _count = _count + len(self.logic.charucoCorners[i])
      self.labelPointsCollected.text = _count
    else:
      pass

    if ret:
      self.labelResult.text = "Success (" + str(self.logic.countIntrinsics()) + ")"
    else:
      self.labelResult.text = "Failure."

  def onIntrinsicModeChanged(self):
    if self.intrinsicCheckerboardButton.checked:
      self.checkerboardContainer.enabled = True
      self.squareSizeDoubleSpinBox.enabled = True
      self.flagsContainer.enabled = True
      self.checkerboardFlags.enabled = True
      self.circleGridFlags.enabled = False
      self.arucoDictContainer.enabled = False
      self.arucoContainer.enabled = False
      self.charucoContainer.enabled = False
      self.logic.calculateObjectPattern(self.rowsSpinBox.value, self.columnsSpinBox.value, 'checkerboard', self.squareSizeDoubleSpinBox.value, 0)
    elif self.intrinsicCircleGridButton.checked:
      self.checkerboardContainer.enabled = True
      self.squareSizeDoubleSpinBox.enabled = False
      self.flagsContainer.enabled = True
      self.checkerboardFlags.enabled = False
      self.circleGridFlags.enabled = True
      self.arucoDictContainer.enabled = False
      self.arucoContainer.enabled = False
      self.charucoContainer.enabled = False
      self.logic.calculateObjectPattern(self.rowsSpinBox.value, self.columnsSpinBox.value, 'circlegrid', self.squareSizeDoubleSpinBox.value, 0)
    elif self.intrinsicArucoButton.checked:
      self.checkerboardContainer.enabled = True
      self.squareSizeDoubleSpinBox.enabled = False
      self.flagsContainer.enabled = False
      self.checkerboardFlags.enabled = False
      self.circleGridFlags.enabled = False
      self.arucoDictContainer.enabled = True
      self.arucoContainer.enabled = True
      self.charucoContainer.enabled = False
      self.logic.calculateObjectPattern(self.rowsSpinBox.value, self.columnsSpinBox.value, 'aruco', self.arucoMarkerSizeSpinBox.value, self.arucoMarkerSeparationSpinBox.value)
    elif self.intrinsicCharucoButton.checked:
      self.checkerboardContainer.enabled = True
      self.squareSizeDoubleSpinBox.enabled = False
      self.flagsContainer.enabled = False
      self.checkerboardFlags.enabled = False
      self.circleGridFlags.enabled = False
      self.arucoDictContainer.enabled = True
      self.arucoContainer.enabled = False
      self.charucoContainer.enabled = True
      self.logic.calculateObjectPattern(self.rowsSpinBox.value, self.columnsSpinBox.value, 'charuco', self.charucoSquareSizeSpinBox.value, self.charucoMarkerSizeSpinBox.value)
    else:
      pass

  def onStylusTipTransformSelected(self):
    if self.stylusTipTransformObserverTag is not None:
      self.stylusTipTransformNode.RemoveObserver(self.stylusTipTransformObserverTag)
      self.stylusTipTransformObserverTag = None

    self.stylusTipTransformNode = self.stylusTipTransformSelector.currentNode()
    if self.stylusTipTransformNode is not None:
      self.stylusTipTransformObserverTag = self.stylusTipTransformNode.AddObserver(slicer.vtkMRMLTransformNode.TransformModifiedEvent, self.onStylusTipTransformModified)

    self.updateUI()

  def onCalibrateButtonClicked(self):
    done, error, mtx, dist = self.logic.calibratePinholeCamera()
    if done:
      self.videoCameraIntrinWidget.GetCurrentNode().SetAndObserveIntrinsicMatrix(mtx)
      self.videoCameraIntrinWidget.GetCurrentNode().SetNumberOfDistortionCoefficients(dist.GetNumberOfValues())
      for i in range(0, dist.GetNumberOfValues()):
        self.videoCameraIntrinWidget.GetCurrentNode().SetDistortionCoefficientValue(i, dist.GetValue(i))
      self.videoCameraIntrinWidget.GetCurrentNode().SetReprojectionError(error)
      self.labelResult.text = "Calibration reprojection error: " + str(error) + "."

  @vtk.calldata_type(vtk.VTK_OBJECT)
  def onStylusTipTransformModified(self, caller, event):
    mat = vtk.vtkMatrix4x4()
    self.stylusTipTransformNode.GetMatrixTransformToWorld(mat)
    if PinholeCameraCalibrationWidget.areSameVTK4x4(mat, self.IdentityMatrix):
      self.stylusTipTransformStatusLabel.setPixmap(self.notOkPixmap)
      self.manualButton.enabled = False
    else:
      self.stylusTipTransformStatusLabel.setPixmap(self.okPixmap)
      self.manualButton.enabled = True

  def updateUI(self):
    self.capIntrinsicButton.enabled = self.imageSelector.currentNode() is not None \
                                      and self.videoCameraSelector.currentNode() is not None

    self.intrinsicsContainer.enabled = self.imageSelector.currentNode() is not None \
                                       and self.videoCameraSelector.currentNode() is not None

    self.trackerContainer.enabled = self.imageSelector.currentNode() is not None \
                                    and self.stylusTipTransformSelector.currentNode() is not None \
                                    and self.videoCameraSelector.currentNode() is not None \
                                    and self.canSelectFiducials

  def onProcessingModeChanged(self):
    if self.manualModeButton.checked:
      self.manualButton.setVisible(True)
      self.semiAutoButton.setVisible(False)
      self.autoButton.setVisible(False)
      self.autoSettingsContainer.setVisible(False)
    elif self.semiAutoModeButton.checked:
      self.manualButton.setVisible(False)
      self.semiAutoButton.setVisible(True)
      self.autoButton.SetVisible(False)
      self.autoSettingsContainer.setVisible(True)
    else:
      self.manualButton.setVisible(False)
      self.semiAutoButton.setVisible(False)
      self.autoButton.setVisible(True)
      self.autoSettingsContainer.setVisible(True)

  def endManualCapturing(self):
    self.isManualCapturing = False
    self.manualButton.setText('Capture')
    # Resume playback
    slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.centerFiducialSelectionNode.GetID())
    slicer.mrmlScene.RemoveNode(self.copyNode)
    self.copyNode = None
    # Re-enable UI
    self.inputsContainer.setEnabled(True)
    self.resetPtLButton.setEnabled(True)

  def onManualButton(self):
    if self.isManualCapturing:
      # Cancel button hit
      self.endManualCapturing()
      slicer.modules.annotations.logic().StopPlaceMode()
      return()

    # Record tracker data at time of freeze and store
    self.stylusTipTransformSelector.currentNode().GetMatrixTransformToWorld(self.stylusTipToPinholeCamera)

    # Make a copy of the volume node (aka freeze cv capture) to allow user to play with detection parameters or click on center
    self.centerFiducialSelectionNode = slicer.mrmlScene.GetNodeByID(slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().GetBackgroundVolumeID())
    self.copyNode = slicer.mrmlScene.CopyNode(self.centerFiducialSelectionNode)
    imData = vtk.vtkImageData()
    imData.DeepCopy(self.centerFiducialSelectionNode.GetImageData())
    self.copyNode.SetAndObserveImageData(imData)
    self.copyNode.SetName('FrozenImage')
    slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.copyNode.GetID())

    # Initiate fiducial selection
    self.markupsNode = slicer.vtkMRMLMarkupsFiducialNode()
    slicer.mrmlScene.AddNode(self.markupsNode)
    self.markupsNode.SetName('SphereCenter')
    self.markupsLogic.SetActiveListID(self.markupsNode)
    self.markupsLogic.StartPlaceMode(False)
    self.pointModifiedObserverTag = self.markupsNode.AddObserver(slicer.vtkMRMLMarkupsNode.PointModifiedEvent, self.onPointModified)

    # Disable input changing while capture is active
    self.inputsContainer.setEnabled(False)
    self.resetPtLButton.setEnabled(False)

    self.isManualCapturing = True
    self.manualButton.setText('Cancel')

  @vtk.calldata_type(vtk.VTK_INT)
  def onPointModified(self, caller, event, callData):
    if callData is None:
      return()

    if self.markupsNode.GetNthControlPointPositionStatus(callData) == slicer.vtkMRMLMarkupsNode.PositionDefined:
      self.endManualCapturing()

      # Calculate point and line pair
      arr = [0, 0, 0]
      self.markupsNode.GetNthControlPointPosition(callData, arr)
      point = np.zeros((1, 1, 2), dtype=np.float64)
      point[0, 0, 0] = abs(arr[0])
      point[0, 0, 1] = abs(arr[1])

      # Get PinholeCamera parameters
      mtx = PinholeCameraCalibrationWidget.vtk3x3ToNumpy(self.videoCameraSelector.currentNode().GetIntrinsicMatrix())

      if self.videoCameraSelector.currentNode().GetNumberOfDistortionCoefficients() != 0:
        dist = np.asarray(np.zeros((1, self.videoCameraSelector.currentNode().GetNumberOfDistortionCoefficients()), dtype=np.float64))
        for i in range(0, self.videoCameraSelector.currentNode().GetNumberOfDistortionCoefficients()):
          dist[0, i] = self.videoCameraSelector.currentNode().GetDistortionCoefficientValue(i)
      else:
        dist = np.asarray([], dtype=np.float64)

      tip_cam = [self.stylusTipToPinholeCamera.GetElement(0, 3), self.stylusTipToPinholeCamera.GetElement(1, 3), self.stylusTipToPinholeCamera.GetElement(2, 3)]

      # Origin - defined in camera, typically 0,0,0
      origin_sen = np.asarray(np.zeros((3, 1), dtype=np.float64))
      for i in range(0, 3):
        origin_sen[i, 0] = self.videoCameraSelector.currentNode().GetCameraPlaneOffsetValue(i)

      # Calculate the direction vector for the given pixel (after undistortion)
      undistPoint = cv2.undistortPoints(point, mtx, dist, P=mtx)
      pixel = np.vstack((undistPoint[0].transpose(), np.array([1.0], dtype=np.float64)))

      # Find the inverse of the videoCamera intrinsic param matrix
      # Calculate direction vector by multiplying the inverse of the intrinsic param matrix by the pixel
      directionVec_sen = np.linalg.inv(mtx) * pixel / np.linalg.norm(np.linalg.inv(mtx) * pixel)

      # And add it to the list!)
      self.logic.addPointLinePair(tip_cam, origin_sen, directionVec_sen)

      if self.developerMode:
        self.rayList.append([tip_cam, origin_sen, directionVec_sen])

      countString = str(self.logic.countMarkerToSensor()) + "/" + str(self.captureCountSpinBox.value) + " points captured."

      if self.logic.countMarkerToSensor() >= self.captureCountSpinBox.value:
        result, videoCameraToImage, string = self.calcRegAndBuildString()
        if result and self.developerMode:
          for combination in self.rayList:
            logging.debug("x: " + str(combination[0]))
            logging.debug("origin: " + str(combination[1]))
            logging.debug("dir: " + str(combination[2]))

          trans = vtk.vtkTransform()
          trans.PostMultiply()
          trans.Identity()
          trans.Concatenate(self.stylusTipToPinholeCamera)
          trans.Concatenate(videoCameraToImage)

          posePosition = trans.GetPosition()

          xPrime = posePosition[0] / posePosition[2]
          yPrime = posePosition[1] / posePosition[2]

          u = (mtx[0, 0] * xPrime) + mtx[0, 2]
          v = (mtx[1, 1] * yPrime) + mtx[1, 2]

          logging.debug("undistorted point: " + str(undistPoint[0, 0, 0]) + "," + str(undistPoint[0, 0, 1]))
          logging.debug("u,v: " + str(u) + "," + str(v))
        self.trackerResultsLabel.text = countString + " " + string
      else:
        self.trackerResultsLabel.text = countString

      # Allow markups module some time to process the new markup, but then quickly delete it
      # Avoids VTK errors in log
      qt.QTimer.singleShot(10, self.removeMarkup)

  def calcRegAndBuildString(self):
    result, markerToSensor = self.logic.calculateMarkerToSensor()
    string = ""

    if result:
      self.videoCameraSelector.currentNode().SetAndObserveMarkerToImageSensorTransform(markerToSensor)

      string = "Registration complete. Error: " + str(self.logic.getErrorMarkerToSensor())
    else:
      string = "Registration failed."

    return result, markerToSensor, string

  def removeMarkup(self):
    if self.markupsNode is not None:
      self.markupsNode.RemoveObserver(self.pointModifiedObserverTag)
      self.pointModifiedObserverTag = None
      self.markupsNode.RemoveAllMarkups()
      slicer.mrmlScene.RemoveNode(self.markupsNode)
      self.markupsNode = None

  def onSemiAutoButton(self):
    pass

  def onAutoButton(self):
    pass

  def onArucoDictChanged(self):
    self.logic.changeArucoDict(self.arucoDictComboBox.currentText)
    self.onIntrinsicModeChanged()

# PinholeCameraCalibrationLogic
class PinholeCameraCalibrationLogic(ScriptedLoadableModuleLogic):
  def __init__(self):
    self.objectPoints = []
    self.imagePoints = []
    # TODO logic should not have state, move these out to client (UI in this case)
    self.arucoDict = None
    self.arucoBoard = None
    self.arucoCorners = []
    self.arucoIDs = []
    self.arucoCount = []
    self.charucoCorners = []
    self.charucoIDs = []

    self.flags = 0
    self.imageSize = (0,0)
    self.objPatternRows = 0
    self.objPatternColumns = 0
    self.objSize = 0
    self.subPixRadius = 5
    self.objPattern = None
    self.terminationCriteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.1)

    self.pointToLineRegistrationLogic = slicer.vtkSlicerPointToLineRegistrationLogic()
    self.pointToLineRegistrationLogic.SetLandmarkRegistrationModeToRigidBody()

  def setTerminationCriteria(self, criteria):
    self.terminationCriteria = criteria

  def calculateObjectPattern(self, rows, columns, type, param1, param2):
    self.objPatternRows = rows
    self.objPatternColumns = columns
    self.objSize = param1
    pattern_size = (self.objPatternColumns, self.objPatternRows)
    self.objPattern = np.zeros((np.prod(pattern_size), 3), np.float32)
    self.objPattern[:, :2] = np.indices(pattern_size).T.reshape(-1, 2)
    self.objPattern *= param1
    self.createBoard(type, param1, param2)

  def createBoard(self, type, param1_mm, param2_mm):
    if self.arucoDict is not None:
      param1 = param1_mm * 0.001;
      param2 = param2_mm * 0.001;
      if type.find('charuco') != -1:
        # square size, marker size
        try:
          self.arucoBoard = aruco.CharucoBoard_create(self.objPatternColumns, self.objPatternRows, param1, param2, self.arucoDict)
        except:
          pass

      elif type.find('aruco') != -1:
        # marker size, marker separation
        try:
          self.arucoBoard = aruco.GridBoard_create(self.objPatternColumns, self.objPatternRows, param1, param2, self.arucoDict)
        except:
          pass

  def setSubPixRadius(self, radius):
    self.subPixRadius = radius

  def resetIntrinsic(self):
    self.objectPoints = []
    self.imagePoints = []
    self.arucoCorners = []
    self.arucoIDs = []
    self.arucoCount = []
    self.charucoCorners = []
    self.charucoIDs = []

  def setFlags(self, flags):
    self.flags = flags

  def findCheckerboard(self, image, invert):
    try:
      gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    except:
      gray = image
    self.imageSize = gray.shape[::-1]

    if invert:
      gray = cv2.bitwise_not(gray)

    # Find the chess board corners
    ret, corners = cv2.findChessboardCorners(gray, (self.objPatternColumns, self.objPatternRows), self.flags)

    # If found, add object points, image points (after refining them)
    if ret:
      self.objectPoints.append(self.objPattern)
      corners2 = cv2.cornerSubPix(gray, corners, (self.subPixRadius, self.subPixRadius), (-1, -1), self.terminationCriteria)
      self.imagePoints.append(corners.reshape(-1,2))

    return ret

  def findCircleGrid(self, image, invert):
    try:
      gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    except:
      gray = image
    self.imageSize = gray.shape[::-1]

    if invert:
      gray = cv2.bitwise_not(gray)

    ret, centers = cv2.findCirclesGrid(gray, (self.objPatternRows, self.objPatternColumns), self.flags)

    if ret:
      self.objectPoints.append(self.objPattern)
      self.imagePoints.append(centers)
      string = "Success (" + str(self.logic.countIntrinsics()) + ")"
      done, result, error, mtx, dist = self.logic.calibratePinholeCamera()
      if done:
        self.videoCameraIntrinWidget.GetCurrentNode().SetAndObserveIntrinsicMatrix(mtx)
        self.videoCameraIntrinWidget.GetCurrentNode().SetNumberOfDistortionCoefficients(dist.GetNumberOfValues())
        for i in range(0, dist.GetNumberOfValues()):
          self.videoCameraIntrinWidget.GetCurrentNode().SetDistortionCoefficientValue(i, dist.GetValue(i))
        string += ". Calibration reprojection error: " + str(error)
      self.labelResult.text = string
    else:
      self.labelResult.text = "Failure."

    return ret

  def findAruco(self, image, invert):
    if self.arucoDict is None or self.arucoBoard is None:
      return False

    try:
      gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    except:
      gray = image
    self.imageSize = gray.shape[::-1]

    if invert:
      gray = cv2.bitwise_not(gray)

    corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(gray, self.arucoDict)

    if len(corners) > 0:
      if len(self.arucoCorners) == 0:
        self.arucoCorners = corners
        self.arucoIDs = ids
      else:
        self.arucoCorners.append(corners[1])
        self.arucoIDs.append(corners[2])
      self.arucoCount.append(len(ids))

    return len(corners)>0

  def findCharuco(self, image, invert):
    if self.arucoDict is None or self.arucoBoard is None:
      return False

    try:
      gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    except:
      gray = image
    self.imageSize = gray.shape

    if invert:
      gray = cv2.bitwise_not(gray)

    # SUB PIXEL CORNER DETECTION CRITERION
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 100, 0.0001)
    corners, ids, rejectedImgPoints = cv2.aruco.detectMarkers(gray, self.arucoDict)

    res = None
    if len(corners) > 0:
      # SUB PIXEL DETECTION
      for corner in corners:
        cv2.cornerSubPix(gray, corner,
                         winSize=(20, 20),
                         zeroZone=(-1, -1),
                         criteria=criteria)
      res = cv2.aruco.interpolateCornersCharuco(corners, ids, gray, self.arucoBoard)
      if res[1] is not None and res[2] is not None and len(res[1]) > 3:
        self.charucoCorners.append(res[1])
        self.charucoIDs.append(res[2])
    return (res is not None)

  def calibratePinholeCamera(self):
    if len(self.imagePoints) > 0:
      ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(self.objectPoints, self.imagePoints, self.imageSize, None, None)
      mat = vtk.vtkMatrix3x3()
      for i in range(0, 3):
        for j in range(0, 3):
          mat.SetElement(i,j, mtx[i,j])
      pts = vtk.vtkDoubleArray()
      for i in range(0, len(dist[0])):
        pts.InsertNextValue(dist[0,i])

      return True, ret, mat, pts
    if len(self.arucoCorners) > 0:
      cameraMatrixInit = np.array([[2000., 0., self.imageSize[0] / 2.],
                                   [0., 2000., self.imageSize[1] / 2.],
                                   [0., 0., 1.]])

      distCoeffsInit = np.zeros((5, 1))
      flags = (cv2.CALIB_USE_INTRINSIC_GUESS + cv2.CALIB_RATIONAL_MODEL)
      ret, mtx, dist, rvecs, tvecs = aruco.calibrateCameraCharucoExtended(charucoCorners=self.arucoCorners,
            charucoIds=self.arucoIDs,
            board=self.arucoBoard,
            imageSize=self.imageSize,
            cameraMatrix=cameraMatrixInit,
            distCoeffs=distCoeffsInit,
            flags=flags,
            criteria=(cv2.TERM_CRITERIA_EPS & cv2.TERM_CRITERIA_COUNT, 10000, 1e-9))

      mat = vtk.vtkMatrix3x3()
      for i in range(0, 3):
        for j in range(0, 3):
          mat.SetElement(i,j, mtx[i,j])
      pts = vtk.vtkDoubleArray()
      for i in range(0, len(dist)):
        pts.InsertNextValue(dist[i])

      return True, ret, mat, pts
    if len(self.charucoCorners) > 0:
      cameraMatrixInit = np.array([[2000., 0., self.imageSize[0] / 2.],
                                   [0., 2000., self.imageSize[1] / 2.],
                                   [0., 0., 1.]])

      distCoeffsInit = np.zeros((5, 1))
      flags = (cv2.CALIB_USE_INTRINSIC_GUESS + cv2.CALIB_RATIONAL_MODEL)
      (ret, camera_matrix, distortion_coefficients0,
       rotation_vectors, translation_vectors,
       stdDeviationsIntrinsics, stdDeviationsExtrinsics,
       perViewErrors) = cv2.aruco.calibrateCameraCharucoExtended(
        charucoCorners=self.charucoCorners,
        charucoIds=self.charucoIDs,
        board=self.arucoBoard,
        imageSize=self.imageSize,
        cameraMatrix=cameraMatrixInit,
        distCoeffs=distCoeffsInit,
        flags=flags,
        criteria=(cv2.TERM_CRITERIA_EPS & cv2.TERM_CRITERIA_COUNT, 10000, 1e-9))

      mat = vtk.vtkMatrix3x3()
      for i in range(0, 3):
        for j in range(0, 3):
          mat.SetElement(i,j, camera_matrix[i,j])
      pts = vtk.vtkDoubleArray()
      for i in range(0, min(len(distortion_coefficients0),5)):
        pts.InsertNextValue(distortion_coefficients0[i,0])

      return True, ret, mat, pts
    return False

  def countIntrinsics(self):
    return max(len(self.imagePoints), len(self.arucoCorners), len(self.charucoCorners))

  def addPointLinePair(self, point, lineOrigin, lineDirection):
    self.pointToLineRegistrationLogic.AddPointAndLine(point, lineOrigin, lineDirection)

  def calculateMarkerToSensor(self):
    mat = self.pointToLineRegistrationLogic.CalculateRegistration()
    eye = vtk.vtkMatrix4x4()
    eye.Identity()
    for i in range(0, 4):
      for j in range(0, 4):
        if mat.GetElement(i,j) != eye.GetElement(i,j):
          return True, mat
    return False, mat

  def resetMarkerToSensor(self):
    self.pointToLineRegistrationLogic.Reset()

  def countMarkerToSensor(self):
    return self.pointToLineRegistrationLogic.GetCount()

  def getErrorMarkerToSensor(self):
    return self.pointToLineRegistrationLogic.GetError()

  def changeArucoDict(self, newDictName):
    for attr in dir(cv2.aruco):
      if attr.find(newDictName) != -1 and isinstance(getattr(cv2.aruco, attr), int):
        self.arucoDict = cv2.aruco.getPredefinedDictionary(getattr(cv2.aruco, attr))

# PinholeCameraCalibrationTest
class PinholeCameraCalibrationTest(ScriptedLoadableModuleTest):
  def setUp(self):
    slicer.mrmlScene.Clear(0)

  def test_PinholeCameraCalibration1(self):
    self.delayDisplay("Starting the test")
    self.delayDisplay('Test passed!')

  def runTest(self):
    self.setUp()
    self.test_PinholeCameraCalibration1()
