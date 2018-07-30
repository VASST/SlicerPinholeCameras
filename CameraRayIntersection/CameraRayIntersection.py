import os
import vtk
import qt
import slicer
import numpy as np
import logging
from slicer.ScriptedLoadableModule import ScriptedLoadableModule, ScriptedLoadableModuleWidget, ScriptedLoadableModuleLogic, ScriptedLoadableModuleTest

#
# CameraRayIntersection
#
class CameraRayIntersection(ScriptedLoadableModule):
  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "Camera Ray Intersection"
    self.parent.categories = ["Webcams"]
    self.parent.dependencies = ["LinesIntersection", "Annotations"]
    self.parent.contributors = ["Adam Rankin (Robarts Research Institute)"]
    self.parent.helpText = """This module calculates the offset between ray intersections on an object from multiple camera angles"""
    self.parent.helpText += self.getDefaultModuleDocumentationLink()
    self.parent.acknowledgementText = ""

#
# CameraRayIntersectionWidget
#
class CameraRayIntersectionWidget(ScriptedLoadableModuleWidget):
  @staticmethod
  def get(widget, objectName):
    if widget.objectName == objectName:
      return widget
    else:
      for w in widget.children():
        resulting_widget = CameraRayIntersectionWidget.get(w, objectName)
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

    self.logic = CameraRayIntersectionLogic()

    self.debugMode = False
    self.canSelectFiducials = False
    self.isManualCapturing = False
    self.validCamera = False

    self.centerFiducialSelectionNode = None
    self.copyNode = None
    self.widget = None
    self.cameraIntrinWidget = None
    self.cameraSelector = None

    # Inputs/Outputs
    self.imageSelector = None
    self.cameraTransformSelector = None

    # Actions
    self.captureButton = None
    self.resetButton = None
    self.actionContainer = None

    # Results
    self.resultsLabel = None

    self.tempMarkupNode = None
    self.sceneObserverTag = None
    self.cameraToReference = None
    self.cameraOriginInReference = None


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

      self.cameraIntrinWidget = CameraRayIntersectionWidget.get(self.widget, "cameraIntrinsicsWidget")

      # Workaround for camera selector
      self.cameraSelector = self.cameraIntrinWidget.children()[1].children()[1]

      # Inputs/Outputs
      self.imageSelector = CameraRayIntersectionWidget.get(self.widget, "comboBox_ImageSelector")
      self.cameraTransformSelector = CameraRayIntersectionWidget.get(self.widget, "comboBox_CameraTransform")
      self.actionContainer = CameraRayIntersectionWidget.get(self.widget, "widget_ActionContainer")

      self.captureButton = CameraRayIntersectionWidget.get(self.widget, "pushButton_Capture")
      self.resetButton = CameraRayIntersectionWidget.get(self.widget, "pushButton_Reset")
      self.actionContainer = CameraRayIntersectionWidget.get(self.widget, "widget_ActionContainer")

      self.resultsLabel = CameraRayIntersectionWidget.get(self.widget, "label_Results")

      # Disable capture as image processing isn't active yet
      self.actionContainer.setEnabled(False)

      # UI file method does not do mrml scene connections, do them manually
      self.cameraIntrinWidget.setMRMLScene(slicer.mrmlScene)
      self.imageSelector.setMRMLScene(slicer.mrmlScene)
      self.cameraTransformSelector.setMRMLScene(slicer.mrmlScene)

      # Connections
      self.cameraSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onCameraSelected)
      self.imageSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
      self.cameraTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
      self.captureButton.connect('clicked(bool)', self.onCapture)
      self.resetButton.connect('clicked(bool)', self.onReset)

      # Adding an observer to scene to listen for mrml node
      self.sceneObserverTag = slicer.mrmlScene.AddObserver(slicer.mrmlScene.NodeAddedEvent, self.onNodeAdded)

      # Choose red slice only
      lm = slicer.app.layoutManager()
      lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutOneUpRedSliceView)

      # Refresh Apply button state
      self.onSelect()

  def onCameraSelected(self):
    node = self.cameraSelector.currentNode()
    if node is not None:
      string = ""
      # Check state of selected camera
      if CameraRayIntersectionWidget.areSameVTK3x3(node.GetIntrinsicMatrix(), vtk.vtkMatrix3x3.Identity()):
        string += "No camera intrinsics! "
      if CameraRayIntersectionWidget.emptyOrZeros(node.GetDistortionCoefficients()):
        string += "No distortion coefficients! "
      if CameraRayIntersectionWidget.areSameVTK4x4(node.GetMarkerToImageSensorTransform(), vtk.vtkMatrix4x4.Identity()):
        string += "No tracker calibration performed! "

      if len(string) > 0:
        self.resultsLabel.text = string
        logging.error(string)
        self.validCamera = False
      else:
        self.validCamera = True

  def cleanup(self):
    self.cameraSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onCameraSelected)
    self.imageSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
    self.cameraTransformSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.captureButton.disconnect('clicked(bool)', self.onCapture)
    self.resetButton.disconnect('clicked(bool)', self.onReset)

    slicer.mrmlScene.RemoveObserver(self.sceneObserverTag)

  def onImageSelected(self):
    # Set red slice to the copy node
    if self.imageSelector.currentNode() is not None:
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.imageSelector.currentNode().GetID())
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().FitSliceToAll()

      # Check pixel spacing, x and y must be 1px = 1mm in order for markups to produce correct pixel locations
      spacing = self.imageSelector.currentNode().GetImageData().GetSpacing()
      if spacing[0] != 1.0 or spacing[1] != 0:
        message = "Image does not have 1.0 spacing in x or y, markup fiducials will not represent pixels exactly!"
        logging.error(message)
        self.resultsLabel = message
        self.canSelectFiducials = False
      else:
        self.canSelectFiducials = True

    self.onSelect()

  def onReset(self):
    self.labelResult.text = "Reset."
    self.logic.reset()

  def onSelect(self):
    self.actionContainer.enabled = self.imageSelector.currentNode() \
                                   and self.cameraTransformSelector.currentNode() \
                                   and self.cameraSelector.currentNode() \
                                   and self.canSelectFiducials \
                                   and self.validCamera

  def onCapture(self):
    if self.isManualCapturing:
      # Cancel button hit
      self.endManualCapturing()
      slicer.modules.annotations.logic().StopPlaceMode()
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

    # Record tracker data at time of freeze and store
    cameraToReferenceVtk = vtk.vtkMatrix4x4()
    self.cameraTransformSelector.currentNode().GetMatrixTransformToParent(cameraToReferenceVtk)
    self.cameraToReference = CameraRayIntersectionWidget.vtk4x4ToNumpy(cameraToReferenceVtk)
    self.cameraOriginInReference = [cameraToReferenceVtk.GetElement(0, 3), cameraToReferenceVtk.GetElement(1, 3), cameraToReferenceVtk.GetElement(2, 3)]

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

      # Get camera parameters
      mtx = CameraRayIntersectionWidget.vtk3x3ToNumpy(self.cameraSelector.currentNode().GetIntrinsicMatrix())

      dist = np.asarray(np.zeros((1, self.cameraSelector.currentNode().GetDistortionCoefficients().GetNumberOfValues()), dtype=np.float64))
      for i in range(0, self.cameraSelector.currentNode().GetDistortionCoefficients().GetNumberOfValues()):
        dist[0, i] = self.cameraSelector.currentNode().GetDistortionCoefficients().GetValue(i)
      else:
        dist = np.asarray([], dtype=np.float64)

      # Calculate the direction vector for the given pixel (after undistortion)
      undistPoint = cv2.undistortPoints(point, mtx, dist, P=mtx)
      pixel = np.asarray([[undistPoint[0,0,0]], [undistPoint[0,0,1]], [1.0]], dtype=np.float64)

      # Get the direction based on selected pixel
      origin_sensor = np.asarray([[0.0],[0.0],[0.0]], dtype=np.float64)
      directionVec_sensor = (np.linalg.inv(mtx) * pixel) / np.linalg.norm(np.linalg.inv(mtx) * pixel)

      # left multiply by marker to imagesensor^-1, then marker to reference
      sensorToMarker = vtk.vtkMatrix4x4.Invert(self.cameraSelector.currentNode().GetMarkerToImageSensorTransform())
      markerToReference = self.cameraTransformSelector.currentNode().GetMatrixTransformToParent()

      sensorToRef = vtk.vtkTransform()
      sensorToRef.PostMultiply()
      sensorToRef.Identity();
      sensorToRef.Concatenate(sensorToMarker)
      sensorToRef.Concatenate(markerToReference)
      mat = CameraRayIntersectionWidget.vtk4x4ToNumpy(sensorToRef.GetMatrix())

      origin_ref = mat * origin_sensor
      directionVec_ref = mat * directionVec_sensor

      result = self.logic.addRay(origin_ref, directionVec_ref)
      if result is not None:
        self.resultsLabel.text = "Point: " + str(result[0]) + "," + str(result[1]) + "," + str(result[2]) + ". Error: " + str(self.logic.getError())
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

#
# CameraRayIntersectionLogic
#
class CameraRayIntersectionLogic(ScriptedLoadableModuleLogic):
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

  def getPoint(self):
    if self.linesRegistrationLogic.Count() > 2:
      return self.linesRegistrationLogic.Update()
    return None

  def getError(self):
    return self.linesRegistrationLogic.GetError()

#
# CameraRayIntersectionTest
#
class CameraRayIntersectionTest(ScriptedLoadableModuleTest):
  """
  This is the test case for your scripted module.
  Uses ScriptedLoadableModuleTest base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def setUp(self):
    """ Do whatever is needed to reset the state - typically a scene clear will be enough.
    """
    slicer.mrmlScene.Clear(0)

  def runTest(self):
    """Run as few or as many tests as needed here.
    """
    self.setUp()
    self.test_CameraRayIntersection1()

  def test_CameraRayIntersection1(self):
    self.delayDisplay("Starting the test")
    self.delayDisplay('Test passed!')
