/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkSlicerVideoCamerasLogic.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// .NAME vtkSlicerVideoCamerasLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerVideoCamerasLogic_h
#define __vtkSlicerVideoCamerasLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerVideoCamerasModuleLogicExport.h"

class vtkMRMLVideoCameraNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_VIDEOCAMERAS_MODULE_LOGIC_EXPORT vtkSlicerVideoCamerasLogic :
  public vtkSlicerModuleLogic
{
public:
  static vtkSlicerVideoCamerasLogic* New();
  vtkTypeMacro(vtkSlicerVideoCamerasLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  ///
  /// Add into the scene a new mrml videoCamera node and
  /// read it's properties from a specified file
  /// A storage node is also added into the scene
  vtkMRMLVideoCameraNode* AddVideoCamera(const char* filename, const char* nodeName = NULL);

protected:
  vtkSlicerVideoCamerasLogic();
  virtual ~vtkSlicerVideoCamerasLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerVideoCamerasLogic(const vtkSlicerVideoCamerasLogic&); // Not implemented
  void operator=(const vtkSlicerVideoCamerasLogic&); // Not implemented
};

#endif
