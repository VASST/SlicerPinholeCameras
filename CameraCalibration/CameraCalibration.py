import os
import vtk
import qt
import slicer
import numpy as np
import logging
from slicer.ScriptedLoadableModule import ScriptedLoadableModule, ScriptedLoadableModuleWidget, ScriptedLoadableModuleLogic, ScriptedLoadableModuleTest

# CameraCalibration
class CameraCalibration(ScriptedLoadableModule):
  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "Calibration"
    self.parent.categories = ["Webcams"]
    self.parent.dependencies = ["PointToLineRegistration", "Annotations"]
    self.parent.contributors = ["Adam Rankin (Robarts Research Institute)"]
    self.parent.helpText = """Perform intrinsic and extrinsic camera calibration against an external tracker."""
    self.parent.helpText += self.getDefaultModuleDocumentationLink()
    self.parent.acknowledgementText = """This file was originally developed by Adam Rankin, Robarts Research Institute and was partially funded by NSERC."""


# CameraCalibrationWidget
class CameraCalibrationWidget(ScriptedLoadableModuleWidget):
  @staticmethod
  def get(widget, objectName):
    if widget.objectName == objectName:
      return widget
    else:
      for w in widget.children():
        resulting_widget = CameraCalibrationWidget.get(w, objectName)
        if resulting_widget:
          return resulting_widget
      return None

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

  def __init__(self, parent):
    ScriptedLoadableModuleWidget.__init__(self, parent)

    global OPENCV2_AVAILABLE
    try:
      global cv2
      import cv2
      OPENCV2_AVAILABLE = True
    except ImportError:
      OPENCV2_AVAILABLE = False

    if not OPENCV2_AVAILABLE:
      logging.error("OpenCV2 python interface not available.")
      return

    self.logic = CameraCalibrationLogic()

    self.debugMode = False
    self.canSelectFiducials = True
    self.isManualCapturing = False

    self.centerFiducialSelectionNode = None
    self.copyNode = None
    self.imageGridNode = None
    self.trivialProducer = None
    self.widget = None
    self.cameraIntrinWidget = None
    self.cameraSelector = None

    self.inputsContainer = None
    self.trackerContainer = None
    self.intrinsicsContainer = None
    self.autoSettingsContainer = None

    # Inputs/Outputs
    self.imageSelector = None
    self.cameraTransformSelector = None
    self.stylusTipTransformSelector = None
    self.outputTransformSelector = None

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

    # Intrinsics
    self.capIntrinsicButton = None
    self.intrinsicCheckerboardButton = None
    self.intrinsicCircleGridButton = None
    self.adaptiveThresholdButton = None
    self.normalizeImageButton = None
    self.filterQuadsButton = None
    self.fastCheckButton = None
    self.symmetricButton = None
    self.asymmetricButton = None
    self.squareSizeEdit = None
    self.clusteringButton = None
    self.labelResult = None

    self.columnsSpinBox = None
    self.rowsSpinBox = None

    self.objPattern = None
    self.sceneObserverTag = None
    self.tempMarkupNode = None
    self.cameraOriginInReference = None
    self.stylusTipToCamera = None
    self.cameraToReference = None

  def setup(self):
    ScriptedLoadableModuleWidget.setup(self)

    if not OPENCV2_AVAILABLE:
      self.layout.addWidget(qt.QLabel("OpenCV2 python is required and not available. Check installation/configuration of SlicerOpenCV."))
    else:
      # Load the UI From file
      scriptedModulesPath = eval('slicer.modules.%s.path' % self.moduleName.lower())
      scriptedModulesPath = os.path.dirname(scriptedModulesPath)
      path = os.path.join(scriptedModulesPath, 'Resources', 'UI', 'q' + self.moduleName + 'Widget.ui')
      self.widget = slicer.util.loadUI(path)
      self.layout.addWidget(self.widget)

      # Nodes
      self.cameraIntrinWidget = CameraCalibrationWidget.get(self.widget, "cameraIntrinsicsWidget")

      # Workaround for camera selector
      self.cameraSelector = self.cameraIntrinWidget.children()[1].children()[1]

      # Inputs/Outputs
      self.imageSelector = CameraCalibrationWidget.get(self.widget, "comboBox_ImageSelector")
      self.cameraTransformSelector = CameraCalibrationWidget.get(self.widget, "comboBox_CameraTransform")
      self.stylusTipTransformSelector = CameraCalibrationWidget.get(self.widget, "comboBox_StylusTipSelector")
      self.outputTransformSelector = CameraCalibrationWidget.get(self.widget, "comboBox_OutputTransform")

      # Tracker calibration members
      self.inputsContainer = CameraCalibrationWidget.get(self.widget, "collapsibleButton_Inputs")
      self.trackerContainer = CameraCalibrationWidget.get(self.widget, "collapsibleButton_Tracker")
      self.intrinsicsContainer = CameraCalibrationWidget.get(self.widget, "collapsibleButton_Intrinsics")
      self.manualButton = CameraCalibrationWidget.get(self.widget, "pushButton_Manual")
      self.semiAutoButton = CameraCalibrationWidget.get(self.widget, "pushButton_SemiAuto")
      self.autoButton = CameraCalibrationWidget.get(self.widget, "pushButton_Automatic")
      self.manualModeButton = CameraCalibrationWidget.get(self.widget, "radioButton_Manual")
      self.semiAutoModeButton = CameraCalibrationWidget.get(self.widget, "radioButton_SemiAuto")
      self.autoModeButton = CameraCalibrationWidget.get(self.widget, "radioButton_Automatic")
      self.autoSettingsContainer = CameraCalibrationWidget.get(self.widget, "groupBox_AutoSettings")
      self.resetPtLButton = CameraCalibrationWidget.get(self.widget, "pushButton_resetPtL")
      self.trackerResultsLabel = CameraCalibrationWidget.get(self.widget, "label_TrackerResultsValue")
      self.captureCountSpinBox = CameraCalibrationWidget.get(self.widget, "spinBox_captureCount")

      # Intrinsic calibration members
      self.capIntrinsicButton = CameraCalibrationWidget.get(self.widget, "pushButton_CaptureIntrinsic")
      self.resetButton = CameraCalibrationWidget.get(self.widget, "pushButton_Reset")
      self.intrinsicCheckerboardButton = CameraCalibrationWidget.get(self.widget, "radioButton_IntrinsicCheckerboard")
      self.intrinsicCircleGridButton = CameraCalibrationWidget.get(self.widget, "radioButton_IntrinsicCircleGrid")
      self.columnsSpinBox = CameraCalibrationWidget.get(self.widget, "spinBox_Columns")
      self.rowsSpinBox = CameraCalibrationWidget.get(self.widget, "spinBox_Rows")
      self.squareSizeEdit = CameraCalibrationWidget.get(self.widget, "lineEdit_SquareSize")
      self.adaptiveThresholdButton = CameraCalibrationWidget.get(self.widget, "checkBox_AdaptiveThreshold")
      self.normalizeImageButton = CameraCalibrationWidget.get(self.widget, "checkBox_NormalizeImage")
      self.filterQuadsButton = CameraCalibrationWidget.get(self.widget, "checkBox_FilterQuads")
      self.fastCheckButton = CameraCalibrationWidget.get(self.widget, "checkBox_FastCheck")
      self.symmetricButton = CameraCalibrationWidget.get(self.widget, "radioButton_SymmetricGrid")
      self.asymmetricButton = CameraCalibrationWidget.get(self.widget, "radioButton_AsymmetricGrid")
      self.clusteringButton = CameraCalibrationWidget.get(self.widget, "radioButton_Clustering")
      self.labelResult = CameraCalibrationWidget.get(self.widget, "label_ResultValue")

      # Disable capture as image processing isn't active yet
      self.trackerContainer.setEnabled(False)
      self.intrinsicsContainer.setEnabled(False)

      # UI file method does not do mrml scene connections, do them manually
      self.cameraIntrinWidget.setMRMLScene(slicer.mrmlScene)
      self.imageSelector.setMRMLScene(slicer.mrmlScene)
      self.cameraTransformSelector.setMRMLScene(slicer.mrmlScene)
      self.stylusTipTransformSelector.setMRMLScene(slicer.mrmlScene)
      self.outputTransformSelector.setMRMLScene(slicer.mrmlScene)

      # Inputs
      self.imageSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
      self.cameraTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
      self.stylusTipTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
      self.outputTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)

      # Connections
      self.capIntrinsicButton.connect('clicked(bool)', self.onIntrinsicCapture)
      self.resetButton.connect('clicked(bool)', self.onReset)
      self.intrinsicCheckerboardButton.connect('clicked(bool)', self.onIntrinsicModeChanged)
      self.intrinsicCircleGridButton.connect('clicked(bool)', self.onIntrinsicModeChanged)
      self.rowsSpinBox.connect('valueChanged(int)', self.onPatternChanged)
      self.columnsSpinBox.connect('valueChanged(int)', self.onPatternChanged)
      self.captureCountSpinBox.connect('valueChanged(int)', self.onCaptureCountChanged)

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

      # Adding an observer to scene to listen for mrml node
      self.sceneObserverTag = slicer.mrmlScene.AddObserver(slicer.mrmlScene.NodeAddedEvent, self.onNodeAdded)

      # Choose red slice only
      lm = slicer.app.layoutManager()
      lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutOneUpRedSliceView)

      # Initialize pattern, etc..
      self.logic.calculateObjectPattern(self.rowsSpinBox.value, self.columnsSpinBox.value, int(self.squareSizeEdit.text))

      # Refresh Apply button state
      self.onSelect()
      self.onProcessingModeChanged()

  def cleanup(self):
    self.capIntrinsicButton.disconnect('clicked(bool)', self.onIntrinsicCapture)
    self.intrinsicCheckerboardButton.disconnect('clicked(bool)', self.onIntrinsicModeChanged)
    self.intrinsicCircleGridButton.disconnect('clicked(bool)', self.onIntrinsicModeChanged)
    self.rowsSpinBox.disconnect('valueChanged(int)', self.onPatternChanged)
    self.columnsSpinBox.disconnect('valueChanged(int)', self.onPatternChanged)
    self.captureCountSpinBox.disconnect('valueChanged(int)', self.onCaptureCountChanged)

    self.imageSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
    self.cameraTransformSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.stylusTipTransformSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.outputTransformSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)

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

    slicer.mrmlScene.RemoveObserver(self.sceneObserverTag)

  def onIntrinsicCapture(self):
    vtk_im = self.imageSelector.currentNode().GetImageData()
    rows, cols, _ = vtk_im.GetDimensions()
    sc = vtk_im.GetPointData().GetScalars()
    im = vtk.util.numpy_support.vtk_to_numpy(sc)
    im = im.reshape(cols, rows, -1)

    if self.intrinsicCheckerboardButton.checked:
      ret = self.logic.findCheckerboard(im)
    else:
      ret = self.logic.findCircleGrid(im)

    if ret:
      self.labelResult.text = "Success (" + str(self.logic.countIntrinsics()) + ")"
      done, error, mtx, dist = self.logic.calibrateCamera()
      if done:
        self.cameraIntrinWidget.GetCurrentNode().SetAndObserveIntrinsicMatrix(mtx)
        self.cameraIntrinWidget.GetCurrentNode().SetAndObserveDistortionCoefficients(dist)
    else:
      self.labelResult.text = "Failure."

  def onReset(self):
    self.logic.resetIntrinsic()
    self.labelResult.text = "Reset."
    self.cameraIntrinWidget.GetCurrentNode().SetAndObserveIntrinsicMatrix(vtk.vtkMatrix3x3.Identity())
    self.cameraIntrinWidget.GetCurrentNode().SetAndObserveDistortionCoefficients(vtk.vtkDoubleArray())

  def onResetPtL(self):
    self.logic.resetCameraToImage()

  def onImageSelected(self):
    # Set red slice to the copy node
    if self.imageSelector.currentNode() is not None:
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.imageSelector.currentNode().GetID())
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().FitSliceToAll()

      # Check pixel spacing, x and y must be 1px = 1mm in order for markups to produce correct pixel locations
      spacing = self.imageSelector.currentNode().GetImageData().GetSpacing()
      if spacing[0] != 1.0 or spacing[1] != 0:
        logging.error("Image does not have 1.0 spacing in x or y, markup fiducials will not represent pixels exactly!")
        self.canSelectFiducials = False
      else:
        self.canSelectFiducials = True

    self.onSelect()

  def onCaptureCountChanged(self):
    countString = str(self.logic.countCameraToImage()) + "/" + str(self.captureCountSpinBox.value) + " points captured."
    string = ""
    result = False

    if self.logic.countCameraToImage() >= self.captureCountSpinBox.value:
      result, cameraToImage, string = self.calcRegAndBuildString()

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

  def onPatternChanged(self, value):
    self.logic.calculateObjectPattern(self.rowsSpinBox.value, self.columnsSpinBox.value, int(self.squareSizeEdit.text))

  def onIntrinsicModeChanged(self):
    if self.intrinsicCheckerboardButton.checked:
      self.adaptiveThresholdButton.enabled = True
      self.normalizeImageButton.enabled = True
      self.filterQuadsButton.enabled = True
      self.fastCheckButton.enabled = True
      self.symmetricButton.enabled = False
      self.asymmetricButton.enabled = False
      self.clusteringButton.enabled = False
    else:
      self.adaptiveThresholdButton.enabled = False
      self.normalizeImageButton.enabled = False
      self.filterQuadsButton.enabled = False
      self.fastCheckButton.enabled = False
      self.symmetricButton.enabled = True
      self.asymmetricButton.enabled = True
      self.clusteringButton.enabled = True

  def onSelect(self):
    self.capIntrinsicButton.enabled = self.imageSelector.currentNode() is not None \
                                      and self.cameraSelector.currentNode() is not None

    self.intrinsicsContainer.enabled = self.imageSelector.currentNode() is not None \
                                       and self.cameraSelector.currentNode() is not None

    self.trackerContainer.enabled = self.imageSelector.currentNode() is not None \
                                    and self.cameraTransformSelector.currentNode() is not None \
                                    and self.stylusTipTransformSelector.currentNode() is not None \
                                    and self.cameraSelector.currentNode() is not None \
                                    and self.outputTransformSelector.currentNode() is not None \
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

    # Make a copy of the volume node (aka freeze cv capture) to allow user to play with detection parameters or click on center
    self.centerFiducialSelectionNode = slicer.mrmlScene.GetNodeByID(slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().GetBackgroundVolumeID())
    self.copyNode = slicer.mrmlScene.CopyNode(self.centerFiducialSelectionNode)
    imData = vtk.vtkImageData()
    imData.DeepCopy(self.centerFiducialSelectionNode.GetImageData())
    self.copyNode.SetAndObserveImageData(imData)
    self.copyNode.SetName('FrozenImage')
    slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.copyNode.GetID())

    # Initiate fiducial selection
    slicer.modules.markups.logic().StartPlaceMode(False)

    # Record tracker data at time of freeze and store
    mat = vtk.vtkMatrix4x4()
    self.stylusTipTransformSelector.currentNode().GetMatrixTransformToParent(mat)
    self.stylusTipToCamera = mat
    cameraToReferenceVtk = vtk.vtkMatrix4x4()
    self.cameraTransformSelector.currentNode().GetMatrixTransformToParent(cameraToReferenceVtk)
    self.cameraToReference = CameraCalibrationWidget.vtk4x4ToNumpy(cameraToReferenceVtk)
    self.cameraOriginInReference = [cameraToReferenceVtk.GetElement(0, 3), cameraToReferenceVtk.GetElement(1, 3), cameraToReferenceVtk.GetElement(2, 3)]

    # Disable input changing while capture is active
    self.inputsContainer.setEnabled(False)
    self.resetPtLButton.setEnabled(False)

    self.isManualCapturing = True
    self.manualButton.setText('Cancel')

  @vtk.calldata_type(vtk.VTK_OBJECT)
  def onNodeAdded(self, caller, event, callData):
    if type(callData) is slicer.vtkMRMLMarkupsFiducialNode and self.isManualCapturing:
      self.endManualCapturing()

      # Calculate point and line pair
      arr = [0,0,0]
      callData.GetMarkupPoint(callData.GetNumberOfMarkups()-1, 0, arr)
      point = np.zeros((1,1,2),dtype=np.float64)
      point[0,0,0] = abs(arr[0])
      point[0,0,1] = abs(arr[1])

      # Get camera parameters
      # Convert vtk to numpy
      mtx = CameraCalibrationWidget.vtk3x3ToNumpy(self.cameraSelector.currentNode().GetIntrinsicMatrix())

      dist = np.asarray(np.zeros((1, self.cameraSelector.currentNode().GetDistortionCoefficients().GetNumberOfValues()), dtype=np.float64))
      for i in range(0, self.cameraSelector.currentNode().GetDistortionCoefficients().GetNumberOfValues()):
        dist[0, i] = self.cameraSelector.currentNode().GetDistortionCoefficients().GetValue(i)
      else:
        dist = np.asarray([], dtype=np.float64)

      x = [self.stylusTipToCamera.GetElement(0, 3), self.stylusTipToCamera.GetElement(1, 3), self.stylusTipToCamera.GetElement(2, 3)]

      # Origin - always 0, 0, 0
      origin = np.asarray([[0.0],[0.0],[0.0]], dtype=np.float64)

      # Calculate the direction vector for the given pixel (after undistortion)
      undistPoint = cv2.undistortPoints(point, mtx, dist, P=mtx)
      pixel = np.asarray([[undistPoint[0,0,0]], [undistPoint[0,0,1]], [1.0]], dtype=np.float64)

      # Find the inverse of the camera intrinsic param matrix
      # Calculate direction vector by multiplying the inverse of the
      # intrinsic param matrix by the pixel
      directionVec = np.linalg.inv(mtx) * pixel

      # Normalize the direction vector
      directionVecNormalized = directionVec / np.linalg.norm(directionVec)

      # And add it to the list!
      self.logic.addPointLinePair(x, origin, directionVecNormalized)

      countString = str(self.logic.countCameraToImage()) + "/" + str(self.captureCountSpinBox.value) + " points captured."

      if self.logic.countCameraToImage() >= self.captureCountSpinBox.value:
        result, cameraToImage, string = self.calcRegAndBuildString()
        if result and self.debugMode:
          trans = vtk.vtkTransform()
          trans.PostMultiply()
          trans.Identity()
          trans.Concatenate(self.stylusTipToCamera)
          trans.Concatenate(cameraToImage)

          posePosition = trans.GetPosition()

          xPrime = posePosition[0] / posePosition[2]
          yPrime = posePosition[1] / posePosition[2]

          u = (mtx[0, 0] * xPrime) + mtx[0, 2];
          v = (mtx[1, 1] * yPrime) + mtx[1, 2];

          logging.debug("undistorted point: " + str(undistPoint[0, 0, 0]) + "," + str(undistPoint[0, 0, 1]))
          logging.debug("u,v: " + str(u) + "," + str(v))
        self.trackerResultsLabel.text = countString + " " + string
      else:
        self.trackerResultsLabel.text = countString

      # Allow markups module some time to process the new markup, but then quickly delete it
      # Avoids VTK errors in log
      self.tempMarkupNode = callData
      qt.QTimer.singleShot(10, self.removeMarkup)

  def calcRegAndBuildString(self):
    mtx = CameraCalibrationWidget.vtk3x3ToNumpy(self.cameraSelector.currentNode().GetIntrinsicMatrix())

    result, cameraToImage = self.logic.calculateCameraToImage()
    string = ""

    if result:
      self.outputTransformSelector.currentNode().SetMatrixTransformToParent(cameraToImage)
      self.cameraSelector.currentNode().SetAndObserveMarkerToImageSensorTransform(cameraToImage)

      string = "Registration complete. Error: " + str(self.logic.getErrorCameraToImage())
    else:
      string = "Registration failed."

    return result, cameraToImage, string

  def removeMarkup(self):
    if self.tempMarkupNode is not None:
      self.tempMarkupNode.RemoveAllMarkups()
      slicer.mrmlScene.RemoveNode(self.tempMarkupNode)
      self.tempMarkupNode = None

  def onSemiAutoButton(self):
    pass

  def onAutoButton(self):
    pass

# CameraCalibrationLogic
class CameraCalibrationLogic(ScriptedLoadableModuleLogic):
  def __init__(self):
    self.objectPoints = []
    self.imagePoints = []

    self.flags = 0
    self.imageSize = (0,0)
    self.objPatternRows = 0
    self.objPatternColumns = 0
    self.subPixRadius = 5
    self.objPattern = None
    self.terminationCriteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.1)

    self.pointToLineRegistrationLogic = slicer.vtkSlicerPointToLineRegistrationLogic()

  def setTerminationCriteria(self, criteria):
    self.terminationCriteria = criteria

  def calculateObjectPattern(self, rows, columns, square_size):
    self.objPatternRows = rows
    self.objPatternColumns = columns
    pattern_size = (self.objPatternColumns, self.objPatternRows)
    self.objPattern = np.zeros((np.prod(pattern_size), 3), np.float32)
    self.objPattern[:, :2] = np.indices(pattern_size).T.reshape(-1, 2)
    self.objPattern *= square_size

  def setSubPixRadius(self, radius):
    self.subPixRadius = radius

  def resetIntrinsic(self):
    self.objectPoints = []
    self.imagePoints = []

  def setFlags(self, flags):
    self.flags = flags

  def findCheckerboard(self, image):
    try:
      gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    except:
      gray = image
    self.imageSize = gray.shape[::-1]

    # Find the chess board corners
    ret, corners = cv2.findChessboardCorners(gray, (self.objPatternColumns, self.objPatternRows), self.flags)

    # If found, add object points, image points (after refining them)
    if ret:
      self.objectPoints.append(self.objPattern)
      corners2 = cv2.cornerSubPix(gray, corners, (self.subPixRadius, self.subPixRadius), (-1, -1), self.terminationCriteria)
      self.imagePoints.append(corners.reshape(-1,2))

    return ret

  def findCircleGrid(self, image):
    try:
      gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    except:
      gray = image
    self.imageSize = gray.shape[::-1]

    ret, centers = cv2.findCirclesGrid(gray, (self.objPatternRows, self.objPatternColumns), self.flags)

    if ret:
      self.objectPoints.append(self.objPattern)
      self.imagePoints.append(centers)
      self.calibrateCamera()

    return ret

  def calibrateCamera(self):
    if len(self.imagePoints) > 0:
      print self.imageSize
      ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(self.objectPoints, self.imagePoints, self.imageSize, None, None)
      mat = vtk.vtkMatrix3x3()
      for i in range(0, 3):
        for j in range(0, 3):
          mat.SetElement(i,j, mtx[i,j])
      pts = vtk.vtkDoubleArray()
      for i in range(0, len(dist[0])):
        pts.InsertNextValue(dist[0,i])
      return True, ret, mat, pts
    return False

  def countIntrinsics(self):
    return len(self.imagePoints)

  def addPointLinePair(self, point, lineOrigin, lineDirection):
    self.pointToLineRegistrationLogic.AddPointAndLine(point, lineOrigin, lineDirection)

  def calculateCameraToImage(self):
    mat = self.pointToLineRegistrationLogic.CalculateRegistration()
    eye = vtk.vtkMatrix4x4()
    eye.Identity()
    for i in range(0, 4):
      for j in range(0, 4):
        if mat.GetElement(i,j) != eye.GetElement(i,j):
          return True, mat
    return False, mat

  def resetCameraToImage(self):
    self.pointToLineRegistrationLogic.Reset()

  def countCameraToImage(self):
    return self.pointToLineRegistrationLogic.GetCount()

  def getErrorCameraToImage(self):
    return self.pointToLineRegistrationLogic.GetError()

# CameraCalibrationTest
class CameraCalibrationTest(ScriptedLoadableModuleTest):
  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough. """
    slicer.mrmlScene.Clear(0)

  def test_CameraCalibration1(self):
    self.delayDisplay("Starting the test")
    self.delayDisplay('Test passed!')

  def runTest(self):
    """ Run as few or as many tests as needed here. """
    self.setUp()
    self.test_CameraCalibration1()
