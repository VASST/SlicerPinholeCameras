import os
import unittest
import vtk, qt, ctk, slicer
from slicer.ScriptedLoadableModule import *
import logging
import numpy as np
import cv2

#
# CameraRayIntersection
#

class CameraRayIntersection(ScriptedLoadableModule):
  """Uses ScriptedLoadableModule base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """

  def __init__(self, parent):
    ScriptedLoadableModule.__init__(self, parent)
    self.parent.title = "Camera Ray Intersection"
    self.parent.categories = ["Webcams"]
    self.parent.dependencies = ["PointToLineRegistration", "Annotations"]
    self.parent.contributors = ["Adam Rankin (Robarts Research Institute)"]
    self.parent.helpText = """This module calculates the offset between ray intersections on an object from multiple camera angles"""
    self.parent.helpText += self.getDefaultModuleDocumentationLink()
    self.parent.acknowledgementText = ""

#
# CameraRayIntersectionWidget
#

class CameraRayIntersectionWidget(ScriptedLoadableModuleWidget):
  """Uses ScriptedLoadableModuleWidget base class, available at:
  https://github.com/Slicer/Slicer/blob/master/Base/Python/slicer/ScriptedLoadableModule.py
  """
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
  def vtk4x4tonumpy(vtk4x4):
    if vtk4x4 is None:
      return

    val = np.asmatrix(np.eye(4, 4, dtype=np.float64))
    for i in range(0, 4):
      for j in range(0, 4):
        val[i, j] = vtk4x4.GetElement(i, j)

    return val

  @staticmethod
  def vtk3x3tonumpy(vtk3x3):
    if vtk3x3 is None:
      return

    val = np.asmatrix(np.eye(3, 3, dtype=np.float64))
    for i in range(0, 3):
      for j in range(0, 3):
        val[i, j] = vtk3x3.GetElement(i, j)

    return val
  def __init__(self, parent):
    ScriptedLoadableModuleWidget.__init__(self, parent)

    self.logic = CameraRayIntersectionLogic()

    self.debugMode = False

    self.isManualCapturing = False

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
    self.imageSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
    self.cameraTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.outputTransformSelector.connect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.captureButton.connect('clicked(bool)', self.onCapture)
    self.resetButton.connect('clicked(bool)', self.onReset)

    # Adding an observer to scene to listen for mrml node
    self.sceneObserverTag = slicer.mrmlScene.AddObserver(slicer.mrmlScene.NodeAddedEvent, self.onNodeAdded)

    # Choose red slice only
    lm = slicer.app.layoutManager()
    lm.setLayout(slicer.vtkMRMLLayoutNode.SlicerLayoutOneUpRedSliceView)

    # Refresh Apply button state
    self.onSelect()

  def cleanup(self):
    self.imageSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onImageSelected)
    self.cameraTransformSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.outputTransformSelector.disconnect("currentNodeChanged(vtkMRMLNode*)", self.onSelect)
    self.captureButton.disconnect('clicked(bool)', self.onCapture)
    self.resetButton.disconnect('clicked(bool)', self.onReset)

    slicer.mrmlScene.RemoveObserver(self.sceneObserverTag)

  def onReset(self):
    self.labelResult.text = "Reset."
    self.logic.reset()

  def onSelect(self):
    self.actionContainer.enabled = self.imageSelector.currentNode() \
                                   and self.cameraTransformSelector.currentNode() \
                                   and self.cameraSelector.currentNode()

  def onCapture(self):
    # Set red slice to the copy node
    if self.imageSelector.currentNode() is not None:
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().GetSliceCompositeNode().SetBackgroundVolumeID(self.imageSelector.currentNode().GetID())
      slicer.app.layoutManager().sliceWidget('Red').sliceLogic().FitSliceToAll()
    self.onSelect()

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
    cameraToReferenceVtk = vtk.vtkMatrix4x4()
    self.cameraTransformSelector.currentNode().GetMatrixTransformToParent(cameraToReferenceVtk)
    self.cameraToReference = CameraRayIntersectionWidget.vtk4x4tonumpy(cameraToReferenceVtk)
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
      point[0,0,0] = arr[0]
      point[0,0,1] = arr[1]

      # Get camera parameters
      mtx = CameraRayIntersectionWidget.vtk3x3tonumpy(self.cameraSelector.currentNode().GetIntrinsicMatrix())

      dist = np.asarray(np.zeros((1, self.cameraSelector.currentNode().GetDistortionCoefficients().GetNumberOfValues()), dtype=np.float64))
      for i in range(0, self.cameraSelector.currentNode().GetDistortionCoefficients().GetNumberOfValues()):
        dist[0, i] = self.cameraSelector.currentNode().GetDistortionCoefficients().GetValue(i)

      # Calculate the direction vector for the given pixel (after undistortion)
      undistPoint = cv2.undistortPoints(point, mtx, dist, P=mtx)
      pixel = np.asarray([[undistPoint[0,0,0]], [undistPoint[0,0,1]], [1.0]], dtype=np.float64)

      # Find the inverse of the camera intrinsic param matrix
      # Calculate direction vector  by multiplying the inverse of the
      # intrinsic param matrix by the pixel
      directionVec = np.linalg.inv(mtx) * pixel

      # Normalize the direction vector
      origin = [0,0,0]
      directionVecNormalized = directionVec / np.linalg.norm(directionVec)

      # TODO: stuff

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
  rays = []

  def reset(self):
    # clear list of rays
    self.rays = []

  def addRay(self, origin, direction):
    self.rays.append([origin, direction])

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
