/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkSlicerWebcamsLogic.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// .NAME vtkSlicerWebcamsLogic - slicer logic class for volumes manipulation
// .SECTION Description
// This class manages the logic associated with reading, saving,
// and changing propertied of the volumes


#ifndef __vtkSlicerWebcamsLogic_h
#define __vtkSlicerWebcamsLogic_h

// Slicer includes
#include "vtkSlicerModuleLogic.h"

// MRML includes

// STD includes
#include <cstdlib>

#include "vtkSlicerWebcamsModuleLogicExport.h"

class vtkMRMLWebcamNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class VTK_SLICER_WEBCAMS_MODULE_LOGIC_EXPORT vtkSlicerWebcamsLogic :
  public vtkSlicerModuleLogic
{
public:
  static vtkSlicerWebcamsLogic* New();
  vtkTypeMacro(vtkSlicerWebcamsLogic, vtkSlicerModuleLogic);
  void PrintSelf(ostream& os, vtkIndent indent);

  ///
  /// Add into the scene a new mrml webcam node and
  /// read it's properties from a specified file
  /// A storage node is also added into the scene
  vtkMRMLWebcamNode* AddWebcam(const char* filename, const char* nodeName = NULL);

protected:
  vtkSlicerWebcamsLogic();
  virtual ~vtkSlicerWebcamsLogic();

  virtual void SetMRMLSceneInternal(vtkMRMLScene* newScene);
  /// Register MRML Node classes to Scene. Gets called automatically when the MRMLScene is attached to this logic class.
  virtual void RegisterNodes();
  virtual void UpdateFromMRMLScene();
  virtual void OnMRMLSceneNodeAdded(vtkMRMLNode* node);
  virtual void OnMRMLSceneNodeRemoved(vtkMRMLNode* node);
private:

  vtkSlicerWebcamsLogic(const vtkSlicerWebcamsLogic&); // Not implemented
  void operator=(const vtkSlicerWebcamsLogic&); // Not implemented
};

#endif
