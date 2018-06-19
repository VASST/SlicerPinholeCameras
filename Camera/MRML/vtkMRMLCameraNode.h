/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLCameraNode.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __vtkMRMLCameraNode_h
#define __vtkMRMLCameraNode_h

// MRML includes
#include "vtkSlicerCameraModuleMRMLExport.h"

// MRML includes
#include <vtkMRMLStorableNode.h>

// VTK includes
#include <vtkMatrix3x3.h>
#include <vtkDoubleArray.h>

class VTK_SLICER_CAMERA_MODULE_MRML_EXPORT vtkMRMLCameraNode : public vtkMRMLStorableNode
{
public:

  // BTX
  enum
  {
    IntrinsicsModifiedEvent = 404001,
    DistortionCoefficientsModifiedEvent
  };
  //ETX

public:
  static vtkMRMLCameraNode* New();
  vtkTypeMacro(vtkMRMLCameraNode, vtkMRMLStorableNode);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual vtkMRMLNode* CreateNodeInstance() VTK_OVERRIDE;

  ///
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode* node) VTK_OVERRIDE;

  ///
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() VTK_OVERRIDE {return "Camera";};

  ///
  /// Set intrinsic matrix
  vtkGetObjectMacro(IntrinsicMatrix, vtkMatrix3x3);
  void SetAndObserveIntrinsicMatrix(vtkMatrix3x3* intrinsicMatrix);

  vtkGetObjectMacro(DistortionCoefficients, vtkDoubleArray);
  void SetAndObserveDistortionCoefficients(vtkDoubleArray* distCoeffs);

protected:
  vtkSetObjectMacro(IntrinsicMatrix, vtkMatrix3x3);
  vtkSetObjectMacro(DistortionCoefficients, vtkDoubleArray);

  unsigned long IntrinsicObserverObserverTag;
  unsigned long DistortionCoefficientsObserverTag;

  void OnIntrinsicsModified(vtkObject* caller, unsigned long event, void* data);
  void OnDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data);

protected:
  vtkMRMLCameraNode();
  ~vtkMRMLCameraNode();
  vtkMRMLCameraNode(const vtkMRMLCameraNode&);
  void operator=(const vtkMRMLCameraNode&);

  vtkMatrix3x3*       IntrinsicMatrix;
  vtkDoubleArray*     DistortionCoefficients;
};

#endif
