import os, vtk, qt, ctk, slicer, numpy as np
import cv2
from vtk.util.numpy_support import vtk_to_numpy, numpy_to_vtk
from decimal import Decimal
from slicer.ScriptedLoadableModule import *

# CameraCalibration
class CameraCalibration(ScriptedLoadableModule):
  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "Calibration"
    self.parent.categories = ["Webcams"]
    self.parent.dependencies = []
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

  def __init__(self, parent):
    ScriptedLoadableModuleWidget.__init__(self, parent)

    self.logic = CameraCalibrationLogic()

    self.isManualCapturing = False

    self.centerFiducialSelectionNode = None
    self.copyNode = None
    self.imageGridNode = None
    self.trivialProducer = None
    self.widget = None

    self.inputsContainer = None
    self.trackerContainer = None
    self.intrinsicsContainer = None
    self.autoSettingsContainer = None

    # Inputs
    self.imageSelector = None
    self.cameraTransformSelector = None
    self.stylusTipTransformSelector = None

    # Tracker
    self.manualButton = None
    self.semiAutoButton = None
    self.manualModeButton = None
    self.autoModeButton = None
    self.semiAutoModeButton = None
    self.autoButton = None
    self.resetButton = None

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

    self.cameraMatrixTable = None
    self.distCoeffsTable = None

    self.columnsSpinBox = None
    self.rowsSpinBox = None

    # OpenCV vars
    self.objPattern = None

    self.sceneObserverTag = None

  def setup(self):
    ScriptedLoadableModuleWidget.setup(self)

    # Load the UI From file
    scriptedModulesPath = eval('slicer.modules.%s.path' % self.moduleName.lower())
    scriptedModulesPath = os.path.dirname(scriptedModulesPath)
    path = os.path.join(scriptedModulesPath, 'Resources', 'UI', 'q' + self.moduleName + 'Widget.ui')
    self.widget = slicer.util.loadUI(path)
    self.layout.addWidget(self.widget)

    # Nodes
    self.cameraIntrinWidget = CameraCalibrationWidget.get(self.widget, "cameraIntrinsicsWidget")

    # Inputs
    self.imageSelector = CameraCalibrationWidget.get(self.widget, "comboBox_ImageSelector")
    self.cameraTransformSelector = CameraCalibrationWidget.get(self.widget, "comboBox_CameraTransform")
    self.stylusTipTransformSelector = CameraCalibrationWidget.get(self.widget, "comboBox_StylusTipSelector")

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


    self.cameraMatrixTable = CameraCalibrationWidget.get(self.widget, "tableWidget_CameraMatrix")
    self.distCoeffsTable = CameraCalibrationWidget.get(self.widget, "tableWidget_DistCoeffs")

    # Disable capture as image processing isn't active yet
    self.trackerContainer.setEnabled(False)
    self.intrinsicsContainer.setEnabled(False)

    # UI file method does not do mrml scene connections, do them manually
    self.imageSelector.setMRMLScene(slicer.mrmlScene)
    self.cameraTransformSelector.setMRMLScene(slicer.mrmlScene)
    self.stylusTipTransformSelector.setMRMLScene(slicer.mrmlScene)

    # Inputs
    self.imageSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
    self.cameraTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.stylusTipTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)

    # Connections
    self.capIntrinsicButton.connect('clicked(bool)', self.onIntrinsicCapture)
    self.resetButton.connect('clicked(bool)', self.onReset)
    self.intrinsicCheckerboardButton.connect('clicked(bool)', self.onIntrinsicModeChanged)
    self.intrinsicCircleGridButton.connect('clicked(bool)', self.onIntrinsicModeChanged)
    self.rowsSpinBox.connect('valueChanged(int)', self.onPatternChanged)
    self.columnsSpinBox.connect('valueChanged(int)', self.onPatternChanged)

    self.manualButton.connect('clicked(bool)', self.onManualButton)
    self.semiAutoButton.connect('clicked(bool)', self.onSemiAutoButton)
    self.autoButton.connect('clicked(bool)', self.onAutoButton)
    self.manualModeButton.connect('clicked(bool)', self.onProcessingModeChanged)
    self.autoModeButton.connect('clicked(bool)', self.onProcessingModeChanged)

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

    self.manualButton.disconnect('clicked(bool)', self.onManualButton)
    self.semiAutoButton.disconnect('clicked(bool)', self.onSemiAutoButton)
    self.autoButton.disconnect('clicked(bool)', self.onAutoButton)
    self.imageSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
    self.cameraTransformSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.manualModeButton.disconnect('clicked(bool)', self.onProcessingModeChanged)
    self.autoModeButton.disconnect('clicked(bool)', self.onProcessingModeChanged)

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
    im = vtk_to_numpy(sc)
    im = im.reshape(cols, rows, -1)

    if self.intrinsicCheckerboardButton.checked:
      ret = self.logic.findCheckerboard(im)
    else:
      ret = self.logic.findCircleGrid(im)

    if ret:
      self.labelResult.text = "Success (" + str(self.logic.count()) + ")"
      done, error, mtx, dist = self.logic.calibrateCamera()
      if done:
        for i in range(0,3):
          for j in range(0,3):
            self.cameraMatrixTable.item(i,j).setText(mtx[i][j])
        for i in range(0, len(dist[0])):
            self.distCoeffsTable.item(0,i).setText(dist[0][i])

    else:
      self.labelResult.text = "Failure."

  def onReset(self):
    self.logic.resetIntrinsic()
    self.labelResult.text = "Reset."
    for i in range(0, 3):
      for j in range(0, 3):
        self.cameraMatrixTable.item(i, j).setText(0.0)
    for i in range(0, self.distCoeffsTable.columnCount):
      self.distCoeffsTable.item(0, i).setText(0.0)

  def onImageSelected(self):
    # Set red slice to the copy node
    if self.imageSelector.currentNode() is not None:
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.imageSelector.currentNode().GetID())
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().FitSliceToAll()
    self.onSelect()

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
    self.logic.calculateObjectPattern(self.rowsSpinBox.value, self.columnsSpinBox.value)

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
    self.capIntrinsicButton.enabled = self.imageSelector.currentNode() is not None
    self.intrinsicsContainer.enabled = self.imageSelector.currentNode() is not None
    self.trackerContainer.enabled = self.imageSelector.currentNode() and self.cameraTransformSelector.currentNode() and self.stylusTipTransformSelector.currentNode()

  def onProcessingModeChanged(self):
    if self.manualModeButton.checked:
      self.manualButton.setVisible(True)
      self.semiAutoButton.setVisible(False)
      self.autoSettingsContainer.setEnabled(False)
    elif self.semiAutoModeButton.checked:
      self.manualButton.setVisible(False)
      self.semiAutoButton.setVisible(True)
      self.autoSettingsContainer.setEnabled(True)
    else:
      self.manualButton.setVisible(True)
      self.semiAutoButton.setVisible(True)
      self.autoSettingsContainer.setEnabled(True)

  def onManualButton(self):
    if self.isManualCapturing:
      # Cancel button hit
      slicer.modules.markups.logic().StopPlaceMode()
      self.inputsContainer.setEnabled(True)
      self.trackerContainer.setEnabled(True)
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.centerFiducialSelectionNode.GetID())
      self.manualButton.setText('Capture')
      return()

    # Make a copy of the volume node (aka freeze cv capture) to allow user to play with detection parameters or click on center
    if self.copyNode is not None:
      # Clean up old copy
      slicer.mrmlScene.RemoveNode(self.copyNode)
      self.copyNode = None

    self.centerFiducialSelectionNode = slicer.mrmlScene.GetNodeByID(slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().GetBackgroundVolumeID())
    self.copyNode = slicer.mrmlScene.CopyNode(self.centerFiducialSelectionNode)

    # Set red slice to the copy node
    slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.copyNode.GetID())

    # Initiate fiducial selection
    slicer.modules.markups.logic().StartPlaceMode(False)

    # Disable input changing while capture is active
    self.inputsContainer.setEnabled(False)
    self.trackerContainer.setEnabled(False)

    self.isManualCapturing = True
    self.manualButton.setText('Cancel')

  @vtk.calldata_type(vtk.VTK_OBJECT)
  def onNodeAdded(self, caller, event, callData):
    if type(callData) is slicer.vtkMRMLMarkupsFiducialNode and self.isManualCapturing is True:
      arr = [0,0,0]
      callData.GetMarkupPoint(callData.GetNumberOfMarkups()-1, 0, arr)
      callData.RemoveAllMarkups()
      slicer.mrmlScene.RemoveNode(callData)
      point = [[[arr[0],arr[1]]]] # cv2.undistortPoints wants points to be in a 3-dimensional array

      # Record stylus point in reference space
      mat = vtk.vtkMatrix4x4()
      self.stylusTipTransformSelector.currentNode().GetMatrixTransformToParent(mat)
      stylusTipPoint = [mat.GetElement(0,3), mat.GetElement(1,3), mat.GetElement(2,3)]

      # transform camera origin (0,0,0) by current camera tracker sensor pose (aka grab translation)
      self.cameraTransformSelector.currentNode().GetMatrixTransformToParent(mat)
      cameraOriginInReference = np.asarray([mat.GetElement(0,3), mat.GetElement(1,3), mat.GetElement(2,3)])

      # Undistort point according to camera parameters
      mtx, dist = self.getIntrinsicsFromUI()

      undistPoint = cv2.undistortPoints(point, mtx, dist)
      undistPoint = np.asarray([[undistPoint[0][0][0]], [undistPoint[0][0][1]], [1.0]])

      # multiply by inverse intrinsics to get point on image plane
      backProjectedPoint = np.linalg.inv(mtx) * undistPoint

      # camera ray is vector difference of transformed camera origin point and transformed undistorted clicked point
      backProjectedPointInReference = np.asarray(mat.MultiplyPoint(backProjectedPoint.transpose()))
      lineInReference = backProjectedPointInReference - cameraOriginInReference

      # Store point and line pair

      # Re-enable UI
      self.inputsContainer.setEnabled(True)
      self.trackerContainer.setEnabled(True)

      # Resume playback
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.centerFiducialSelectionNode.GetID())

  def onSemiAutoButton(self):
    pass

  def onAutoButton(self):
    pass

  def getIntrinsicsFromUI(self):
    mtx = np.asmatrix(np.eye(3, 3, dtype='float64'))
    for i in range(0, 3):
      for j in range(0, 3):
        mtx[i,j] = Decimal(self.cameraMatrixTable.item(i, j).text())

    dist = np.zeros((1, self.distCoeffsTable.columnCount), dtype='float64')
    for i in range(0, self.distCoeffsTable.columnCount):
      dist[0,i] = Decimal(self.distCoeffsTable.item(0, i).text())

    return mtx, dist

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

    if ret == True:
      self.objectPoints.append(self.objPattern)
      self.imagePoints.append(centers)
      self.calibrateCamera()

    return ret

  def calibrateCamera(self):
    if len(self.imagePoints) > 0:
      print self.imageSize
      ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(self.objectPoints, self.imagePoints, self.imageSize, None, None)
      return True, ret, mtx, dist
    return False

  def count(self):
    return len(self.imagePoints)

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
