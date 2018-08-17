import os
import vtk
import qt
import slicer
import numpy as np
import logging
from slicer.ScriptedLoadableModule import ScriptedLoadableModule, ScriptedLoadableModuleWidget, ScriptedLoadableModuleLogic, ScriptedLoadableModuleTest

# VideoCameraRayIntersection
class VideoCameraRayIntersection(ScriptedLoadableModule):
  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "VideoCamera Ray Intersection"
    self.parent.categories = ["VideoCameras"]
    self.parent.dependencies = ["VideoCameras", "LinesIntersection", "Annotations"]
    self.parent.contributors = ["Adam Rankin (Robarts Research Institute)"]
    self.parent.helpText = """This module calculates the offset between ray intersections on an object from multiple videoCamera angles. """ + self.getDefaultModuleDocumentationLink()
    self.parent.acknowledgementText = """This module was developed with support from the Natural Sciences and Engineering Research Council of Canada, the Canadian Foundation for Innovation, and the Virtual Augmentation and Simulation for Surgery and Therapy laboratory, Western University."""

# VideoCameraRayIntersectionWidget
class VideoCameraRayIntersectionWidget(ScriptedLoadableModuleWidget):
  @staticmethod
  def get(widget, objectName):
    if widget.objectName == objectName:
      return widget
    else:
      for w in widget.children():
        resulting_widget = VideoCameraRayIntersectionWidget.get(w, objectName)
        if resulting_widget:
          return resulting_widget
      return None

  @staticmethod
  def emptyOrZeros(doubleArray):
    count = doubleArray.GetNumberOfValues()
    result = True
    for i in range(0, count):
      if doubleArray.GetValue(i) != 0.0:
        return False
    return True

  @staticmethod
  def areSameVTK4x4(a, b):
    for i in range(0, 4):
      for j in range(0, 4):
        if a.GetElement(i,j) != b.GetElement(i,j):
          return False
    return True

  @staticmethod
  def areSameVTK3x3(a, b):
    for i in range(0, 3):
      for j in range(0, 3):
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
    iconPath = os.path.join(os.path.dirname(slicer.modules.videocameracalibration.path), 'Resources/Icons/', param + ".png")
    icon = qt.QIcon(iconPath)
    return icon.pixmap(icon.actualSize(qt.QSize(x, y)))

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

    self.logic = VideoCameraRayIntersectionLogic()

    self.canSelectFiducials = False
    self.isManualCapturing = False
    self.validVideoCamera = False

    self.centerFiducialSelectionNode = None
    self.copyNode = None
    self.widget = None
    self.videoCameraIntrinWidget = None
    self.videoCameraSelector = None
    self.videoCameraNode = None
    self.videoCameraObserverTag = None

    self.videoCameraTransformNode = None
    self.videoCameraTransformObserverTag = None
    self.videoCameraTransformStatusLabel = None

    self.okPixmap = VideoCameraRayIntersectionWidget.loadPixmap('icon_Ok', 20, 20)
    self.notOkPixmap = VideoCameraRayIntersectionWidget.loadPixmap('icon_NotOk', 20, 20)

    # Inputs/Outputs
    self.imageSelector = None
    self.videoCameraTransformSelector = None

    # Actions
    self.captureButton = None
    self.resetButton = None
    self.actionContainer = None

    # Results
    self.resultsLabel = None

    self.tempMarkupNode = None
    self.sceneObserverTag = None
    self.videoCameraToReference = None

    self.identity3x3 = vtk.vtkMatrix3x3()
    self.identity4x4 = vtk.vtkMatrix4x4()

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

    self.videoCameraIntrinWidget = VideoCameraRayIntersectionWidget.get(self.widget, "videoCameraIntrinsicsWidget")

    # Workaround for videoCamera selector
    self.videoCameraSelector = self.videoCameraIntrinWidget.children()[1].children()[1]

    # Inputs/Outputs
    self.imageSelector = VideoCameraRayIntersectionWidget.get(self.widget, "comboBox_ImageSelector")
    self.videoCameraTransformSelector = VideoCameraRayIntersectionWidget.get(self.widget, "comboBox_VideoCameraTransform")
    self.actionContainer = VideoCameraRayIntersectionWidget.get(self.widget, "widget_ActionContainer")

    self.captureButton = VideoCameraRayIntersectionWidget.get(self.widget, "pushButton_Capture")
    self.resetButton = VideoCameraRayIntersectionWidget.get(self.widget, "pushButton_Reset")
    self.actionContainer = VideoCameraRayIntersectionWidget.get(self.widget, "widget_ActionContainer")

    self.resultsLabel = VideoCameraRayIntersectionWidget.get(self.widget, "label_Results")
    self.videoCameraTransformStatusLabel = VideoCameraRayIntersectionWidget.get(self.widget, "label_VideoCameraTransform_Status")

    # Disable capture as image processing isn't active yet
    self.actionContainer.setEnabled(False)

    # UI file method does not do mrml scene connections, do them manually
    self.videoCameraIntrinWidget.setMRMLScene(slicer.mrmlScene)
    self.imageSelector.setMRMLScene(slicer.mrmlScene)
    self.videoCameraTransformSelector.setMRMLScene(slicer.mrmlScene)

    # Connections
    self.videoCameraSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onVideoCameraSelected)
    self.imageSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
    self.videoCameraTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onVideoCameraTransformSelected)
    self.captureButton.connect('clicked(bool)', self.onCapture)
    self.resetButton.connect('clicked(bool)', self.onReset)

    # Adding an observer to scene to listen for mrml node
    self.sceneObserverTag = slicer.mrmlScene.AddObserver(slicer.mrmlScene.NodeAddedEvent, self.onNodeAdded)

    # Choose red slice only
    lm = slicer.app.layoutManager()
    lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutOneUpRedSliceView)

    # Refresh Apply button state
    self.onSelect()

  def onVideoCameraSelected(self):
    if self.videoCameraNode is not None:
      self.videoCameraNode.RemoveObserver(self.videoCameraObserverTag)

    self.videoCameraNode = self.videoCameraSelector.currentNode()

    if self.videoCameraNode is not None:
      self.videoCameraObserverTag = self.videoCameraNode.AddObserver(vtk.vtkCommand.ModifiedEvent, self.onVideoCameraModified)

    self.checkVideoCamera()

  def cleanup(self):
    self.videoCameraSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onVideoCameraSelected)
    self.imageSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
    self.videoCameraTransformSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onVideoCameraTransformSelected)
    self.captureButton.disconnect('clicked(bool)', self.onCapture)
    self.resetButton.disconnect('clicked(bool)', self.onReset)

    slicer.mrmlScene.RemoveObserver(self.sceneObserverTag)

  @vtk.calldata_type(vtk.VTK_OBJECT)
  def onVideoCameraModified(self, caller, event):
    self.checkVideoCamera()

  def checkVideoCamera(self):
    if self.videoCameraNode is None:
      self.validVideoCamera = False
      return()

    self.validVideoCamera = True
    string = ""
    # Check state of selected videoCamera
    if VideoCameraRayIntersectionWidget.areSameVTK3x3(self.videoCameraNode.GetIntrinsicMatrix(), self.identity3x3):
      string += "No videoCamera intrinsics! "
      self.validVideoCamera = False
    if VideoCameraRayIntersectionWidget.emptyOrZeros(self.videoCameraNode.GetDistortionCoefficients()):
      string += "No distortion coefficients! "
      self.validVideoCamera = False
    if VideoCameraRayIntersectionWidget.areSameVTK4x4(self.videoCameraNode.GetMarkerToImageSensorTransform(), self.identity4x4):
      string += "No tracker calibration performed! "

    if len(string) > 0:
      self.resultsLabel.text = string
    else:
      self.resultsLabel.text = "VideoCamera ok!"

    self.onSelect()

  def onImageSelected(self):
    # Set red slice to the copy node
    if self.imageSelector.currentNode() is not None:
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.imageSelector.currentNode().GetID())
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().FitSliceToAll()

      # Check pixel spacing, x and y must be 1px = 1mm in order for markups to produce correct pixel locations
      spacing = self.imageSelector.currentNode().GetImageData().GetSpacing()
      if spacing[0] != 1.0 or spacing[1] != 1.0:
        message = "Image does not have 1.0 spacing in x or y, markup fiducials will not represent pixels exactly!"
        logging.error(message)
        self.resultsLabel.text = message
        self.canSelectFiducials = False
      else:
        self.canSelectFiducials = True

    self.onSelect()

  def onReset(self):
    self.resultsLabel.text = "Reset."
    self.logic.reset()

  def onSelect(self):
    self.actionContainer.enabled = self.imageSelector.currentNode() \
                                   and self.videoCameraTransformSelector.currentNode() \
                                   and self.videoCameraSelector.currentNode() \
                                   and self.canSelectFiducials \
                                   and self.validVideoCamera

  def onCapture(self):
    if self.isManualCapturing:
      # Cancel button hit
      self.endManualCapturing()
      slicer.modules.annotations.logic().StopPlaceMode()
      return()

    # Record tracker data at time of freeze and store
    videoCameraToReferenceVtk = vtk.vtkMatrix4x4()
    self.videoCameraTransformSelector.currentNode().GetMatrixTransformToParent(videoCameraToReferenceVtk)
    self.videoCameraToReference = VideoCameraRayIntersectionWidget.vtk4x4ToNumpy(videoCameraToReferenceVtk)

    if VideoCameraRayIntersectionWidget.areSameVTK4x4(videoCameraToReferenceVtk, self.identity4x4):
      self.resultsLabel.text = "Invalid transform. Please try again with sensor in view."
      return()

    # Reset view so that capture button always works
    if self.imageSelector.currentNode() is not None:
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.imageSelector.currentNode().GetID())
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().FitSliceToAll()
    self.onSelect()

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

    # Disable resetting while capture is active
    self.resetButton.setEnabled(False)
    self.isManualCapturing = True
    self.captureButton.setText('Cancel')

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

      # Get videoCamera parameters
      mtx = VideoCameraRayIntersectionWidget.vtk3x3ToNumpy(self.videoCameraSelector.currentNode().GetIntrinsicMatrix())

      if self.videoCameraSelector.currentNode().GetDistortionCoefficients().GetNumberOfValues() != 0:
        dist = np.asarray(np.zeros((1, self.videoCameraSelector.currentNode().GetDistortionCoefficients().GetNumberOfValues()), dtype=np.float64))
        for i in range(0, self.videoCameraSelector.currentNode().GetDistortionCoefficients().GetNumberOfValues()):
          dist[0, i] = self.videoCameraSelector.currentNode().GetDistortionCoefficients().GetValue(i)
      else:
        dist = np.asarray([], dtype=np.float64)

      # Calculate the direction vector for the given pixel (after undistortion)
      undistPoint = cv2.undistortPoints(point, mtx, dist, P=mtx)
      pixel = np.asarray([[undistPoint[0,0,0]], [undistPoint[0,0,1]], [1.0]], dtype=np.float64)

      # Get the direction based on selected pixel
      origin_sensor = np.asmatrix([[0.0],[0.0],[0.0],[1.0]], dtype=np.float64)
      directionVec_sensor = (np.linalg.inv(mtx) * pixel) / np.linalg.norm(np.linalg.inv(mtx) * pixel)
      directionVec_sensor = np.asmatrix([[directionVec_sensor[0,0]],[directionVec_sensor[1,0]],[directionVec_sensor[2,0]],[0.0]], dtype=np.float64)

      sensorToVideoCamera = VideoCameraRayIntersectionWidget.vtk4x4ToNumpy(self.videoCameraSelector.currentNode().GetMarkerToImageSensorTransform())
      sensorToVideoCamera = np.linalg.inv(sensorToVideoCamera)

      sensorToReference = self.videoCameraToReference * sensorToVideoCamera

      origin_ref = sensorToReference * origin_sensor
      directionVec_ref = sensorToReference * directionVec_sensor

      if self.developerMode:
        logging.debug("origin_ref: " + str(origin_ref).replace('\n',''))
        logging.debug("dir_ref: " + str(directionVec_ref).replace('\n',''))

      result = self.logic.addRay([origin_ref[0,0], origin_ref[1,0], origin_ref[2,0]], [directionVec_ref[0,0], directionVec_ref[1,0], directionVec_ref[2,0]])
      if result is not None:
        self.resultsLabel.text = "Point: " + str(result[0]) + "," + str(result[1]) + "," + str(result[2]) + ". Error: " + str(self.logic.getError())
        if self.developerMode:
          # For ease of copy pasting multiple entries, print it to the python console
          print "Intersection|" + str(result[0]) + "," + str(result[1]) + "," + str(result[2]) + "|" + str(self.logic.getError())

      # Allow markups module some time to process the new markup, but then quickly delete it
      # Avoids VTK errors in log
      self.tempMarkupNode = callData
      qt.QTimer.singleShot(10, self.removeMarkup)

  def endManualCapturing(self):
    self.isManualCapturing = False
    self.captureButton.setText('Capture')

    # Resume playback
    slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.centerFiducialSelectionNode.GetID())
    slicer.mrmlScene.RemoveNode(self.copyNode)
    self.copyNode = None

    # Re-enable UI
    self.resetButton.setEnabled(True)

  def removeMarkup(self):
    if self.tempMarkupNode is not None:
      self.tempMarkupNode.RemoveAllMarkups()
      slicer.mrmlScene.RemoveNode(self.tempMarkupNode)
      self.tempMarkupNode = None

  def onVideoCameraTransformSelected(self):
    if self.videoCameraTransformObserverTag is not None:
      self.videoCameraTransformNode.RemoveObserver(self.videoCameraTransformObserverTag)
      self.videoCameraTransformObserverTag = None

    self.videoCameraTransformNode = self.videoCameraTransformSelector.currentNode()
    if self.videoCameraTransformNode is not None:
      self.videoCameraTransformObserverTag = self.videoCameraTransformNode.AddObserver(slicer.vtkMRMLTransformNode.TransformModifiedEvent, self.onVideoCameraTransformModified)

    self.onSelect()

  @vtk.calldata_type(vtk.VTK_OBJECT)
  def onVideoCameraTransformModified(self, caller, event):
    mat = vtk.vtkMatrix4x4()
    self.videoCameraTransformNode.GetMatrixTransformToParent(mat)
    if VideoCameraRayIntersectionWidget.areSameVTK4x4(mat, self.identity4x4):
      self.videoCameraTransformStatusLabel.setPixmap(self.notOkPixmap)
      self.captureButton.enabled = False
    else:
      self.videoCameraTransformStatusLabel.setPixmap(self.okPixmap)
      self.captureButton.enabled = True

# VideoCameraRayIntersectionLogic
class VideoCameraRayIntersectionLogic(ScriptedLoadableModuleLogic):
  def __init__(self):
    self.linesRegistrationLogic = slicer.vtkSlicerLinesIntersectionLogic()

  def reset(self):
    # clear list of rays
    self.linesRegistrationLogic.Reset()

  def addRay(self, origin, direction):
    self.linesRegistrationLogic.AddLine(origin, direction)
    if self.linesRegistrationLogic.Count() > 2:
      return self.linesRegistrationLogic.Update()
    return None

  def getCount(self):
    return self.linesRegistrationLogic.Count()

  def getPoint(self):
    if self.linesRegistrationLogic.Count() > 2:
      return self.linesRegistrationLogic.Update()
    return None

  def getError(self):
    return self.linesRegistrationLogic.GetError()

# VideoCameraRayIntersectionTest
class VideoCameraRayIntersectionTest(ScriptedLoadableModuleTest):
  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough. """
    slicer.mrmlScene.Clear(0)

  def runTest(self):
    """ Run as few or as many tests as needed here. """
    self.setUp()
    self.test_VideoCameraRayIntersection1()

  def test_VideoCameraRayIntersection1(self):
    self.delayDisplay("Starting the test")
    self.delayDisplay('Test passed!')
