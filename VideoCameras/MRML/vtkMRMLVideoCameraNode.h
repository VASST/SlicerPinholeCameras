/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLVideoCameraNode.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __vtkMRMLVideoCameraNode_h
#define __vtkMRMLVideoCameraNode_h

// MRML includes
#include "vtkSlicerVideoCamerasModuleMRMLExport.h"

// MRML includes
#include <vtkMRMLStorableNode.h>

// VTK includes
#include <vtkDoubleArray.h>
#include <vtkMatrix3x3.h>
#include <vtkMatrix4x4.h>

class VTK_SLICER_VIDEOCAMERAS_MODULE_MRML_EXPORT vtkMRMLVideoCameraNode : public vtkMRMLStorableNode
{
public:
  enum
  {
    IntrinsicsModifiedEvent = 404001,
    DistortionCoefficientsModifiedEvent,
    CameraPlaneOffsetModifiedEvent,
    MarkerToSensorTransformModifiedEvent
  };

public:
  static vtkMRMLVideoCameraNode* New();
  vtkTypeMacro(vtkMRMLVideoCameraNode, vtkMRMLStorableNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual vtkMRMLNode* CreateNodeInstance() override;

  ///
  /// Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode* node) override;

  ///
  /// Get node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() override {return "VideoCamera";};

  ///
  /// Set intrinsic matrix
  vtkGetObjectMacro(IntrinsicMatrix, vtkMatrix3x3);
  void SetAndObserveIntrinsicMatrix(vtkMatrix3x3* intrinsicMatrix);

  vtkGetObjectMacro(DistortionCoefficients, vtkDoubleArray);
  void SetAndObserveDistortionCoefficients(vtkDoubleArray* distCoeffs);

  vtkGetObjectMacro(CameraPlaneOffset, vtkDoubleArray);
  void SetAndObserveCameraPlaneOffset(vtkDoubleArray* planeOffset);

  vtkGetObjectMacro(MarkerToImageSensorTransform, vtkMatrix4x4);
  void SetAndObserveMarkerToImageSensorTransform(vtkMatrix4x4* markerToImageSensorTransform);

  virtual vtkMRMLStorageNode* CreateDefaultStorageNode() override;

  bool IsReprojectionErrorValid() const;
  vtkSetMacro(ReprojectionError, double);
  vtkGetMacro(ReprojectionError, double);

  bool IsRegistrationErrorValid() const;
  vtkSetMacro(RegistrationError, double);
  vtkGetMacro(RegistrationError, double);

protected:
  vtkSetObjectMacro(IntrinsicMatrix, vtkMatrix3x3);
  vtkSetObjectMacro(DistortionCoefficients, vtkDoubleArray);
  vtkSetObjectMacro(MarkerToImageSensorTransform, vtkMatrix4x4);
  vtkSetObjectMacro(CameraPlaneOffset, vtkDoubleArray);

  unsigned long IntrinsicObserverObserverTag;
  unsigned long DistortionCoefficientsObserverTag;
  unsigned long CameraPlaneOffsetObserverTag;
  unsigned long MarkerTransformObserverTag;

  void OnIntrinsicsModified(vtkObject* caller, unsigned long event, void* data);
  void OnDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data);
  void OnCameraPlaneOffsetModified(vtkObject* caller, unsigned long event, void* data);
  void OnMarkerTransformModified(vtkObject* caller, unsigned long event, void* data);

protected:
  vtkMRMLVideoCameraNode();
  ~vtkMRMLVideoCameraNode();
  vtkMRMLVideoCameraNode(const vtkMRMLVideoCameraNode&);
  void operator=(const vtkMRMLVideoCameraNode&);

  vtkMatrix3x3*       IntrinsicMatrix;
  vtkDoubleArray*     DistortionCoefficients;
  double              ReprojectionError;
  double              RegistrationError;
  vtkDoubleArray*     CameraPlaneOffset;
  vtkMatrix4x4*       MarkerToImageSensorTransform;
};

#endif
