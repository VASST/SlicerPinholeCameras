/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkSlicerPinholeCamerasLogic.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// .NAME vtkSlicerPinholeCamerasLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerPinholeCamerasLogic_h
#define __vtkSlicerPinholeCamerasLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerPinholeCamerasModuleLogicExport.h"

class vtkMRMLPinholeCameraNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_PINHOLECAMERAS_MODULE_LOGIC_EXPORT vtkSlicerPinholeCamerasLogic :
  public vtkSlicerModuleLogic
{
public:
  static vtkSlicerPinholeCamerasLogic* New();
  vtkTypeMacro(vtkSlicerPinholeCamerasLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  ///
  /// Add into the scene a new mrml videoCamera node and
  /// read it's properties from a specified file
  /// A storage node is also added into the scene
  vtkMRMLPinholeCameraNode* AddPinholeCamera(const char* filename, const char* nodeName = NULL);

protected:
  vtkSlicerPinholeCamerasLogic();
  virtual ~vtkSlicerPinholeCamerasLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerPinholeCamerasLogic(const vtkSlicerPinholeCamerasLogic&); // Not implemented
  void operator=(const vtkSlicerPinholeCamerasLogic&); // Not implemented
};

#endif
