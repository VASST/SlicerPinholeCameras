/*=auto=========================================================================

CLortions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLPinholeCameraNode.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "vtkMRMLPinholeCameraNode.h"
#include "vtkMRMLPinholeCameraStorageNode.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkXMLUtilities.h>

// STL includes
#include <sstream>

//----------------------------------------------------------------------------

vtkMRMLNodeNewMacro(vtkMRMLPinholeCameraNode);

//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
vtkMRMLPinholeCameraNode::vtkMRMLPinholeCameraNode()
  : vtkMRMLStorableNode()
  , IntrinsicMatrix(nullptr)
  , DistortionCoefficients(nullptr)
  , MarkerToImageSensorTransform(nullptr)
  , CameraPlaneOffset(nullptr)
  , ReprojectionError(-1.0)
  , RegistrationError(-1.0)
{
  this->SetAndObserveIntrinsicMatrix(vtkSmartPointer<vtkMatrix3x3>::New());
  this->SetDistortionCoefficients(vtkSmartPointer<vtkDoubleArray>::New());
  this->GetDistortionCoefficients()->SetNumberOfValues(5);
  this->GetDistortionCoefficients()->FillValue(0.0);
  this->SetAndObserveMarkerToImageSensorTransform(vtkSmartPointer<vtkMatrix4x4>::New());
  this->SetCameraPlaneOffset(vtkSmartPointer<vtkDoubleArray>::New());
  this->GetCameraPlaneOffset()->SetNumberOfValues(3);
  this->GetCameraPlaneOffset()->FillValue(0.0);
}

//-----------------------------------------------------------------------------
vtkMRMLPinholeCameraNode::~vtkMRMLPinholeCameraNode()
{
  this->SetAndObserveIntrinsicMatrix(nullptr);
  this->SetDistortionCoefficients(nullptr);
  this->SetAndObserveMarkerToImageSensorTransform(nullptr);
  this->SetCameraPlaneOffset(nullptr);
}

//----------------------------------------------------------------------------
void vtkMRMLPinholeCameraNode::Copy(vtkMRMLNode* anode)
{
  int disabledModify = this->StartModify();
  Superclass::Copy(anode);
  vtkMRMLPinholeCameraNode* node = vtkMRMLPinholeCameraNode::SafeDownCast(anode);

  this->GetIntrinsicMatrix()->DeepCopy(node->GetIntrinsicMatrix());
  this->GetDistortionCoefficients()->DeepCopy(node->GetDistortionCoefficients());
  this->GetMarkerToImageSensorTransform()->DeepCopy(node->GetMarkerToImageSensorTransform());
  this->GetCameraPlaneOffset()->DeepCopy(node->GetCameraPlaneOffset());
  this->SetReprojectionError(node->GetReprojectionError());
  this->SetRegistrationError(node->GetRegistrationError());

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLPinholeCameraNode::SetAndObserveIntrinsicMatrix(vtkMatrix3x3* intrinsicMatrix)
{
  if (this->IntrinsicMatrix != NULL)
  {
    this->IntrinsicMatrix->RemoveObserver(this->IntrinsicObserverObserverTag);
  }

  this->SetIntrinsicMatrix(intrinsicMatrix);

  if (this->IntrinsicMatrix != NULL)
  {
    this->IntrinsicObserverObserverTag = this->IntrinsicMatrix->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLPinholeCameraNode::OnIntrinsicsModified);
  }

  this->InvokeEvent(vtkMRMLPinholeCameraNode::IntrinsicsModifiedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLPinholeCameraNode::SetAndObserveMarkerToImageSensorTransform(vtkMatrix4x4* markerToImageSensorTransform)
{
  if (this->MarkerToImageSensorTransform != NULL)
  {
    this->MarkerToImageSensorTransform->RemoveObserver(this->MarkerTransformObserverTag);
  }

  this->SetMarkerToImageSensorTransform(markerToImageSensorTransform);

  if (this->MarkerToImageSensorTransform != NULL)
  {
    this->MarkerTransformObserverTag = this->MarkerToImageSensorTransform->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLPinholeCameraNode::OnMarkerTransformModified);
  }

  this->InvokeEvent(vtkMRMLPinholeCameraNode::MarkerToSensorTransformModifiedEvent);
}

//----------------------------------------------------------------------------
bool vtkMRMLPinholeCameraNode::HasDistortionCoefficents() const
{
  return this->DistortionCoefficientsExist;
}

//----------------------------------------------------------------------------
void vtkMRMLPinholeCameraNode::SetNumberOfDistortionCoefficients(vtkIdType num)
{
  this->DistortionCoefficients->SetNumberOfValues(num);
}

//----------------------------------------------------------------------------
vtkIdType vtkMRMLPinholeCameraNode::GetNumberOfDistortionCoefficients() const
{
  return this->DistortionCoefficients->GetNumberOfValues();
}

//----------------------------------------------------------------------------
void vtkMRMLPinholeCameraNode::SetDistortionCoefficientValue(vtkIdType idx, double value)
{
  if (this->DistortionCoefficients->GetNumberOfValues() > idx)
  {
    this->DistortionCoefficients->SetValue(idx, value);
    this->DistortionCoefficientsExist = true;
    this->InvokeEvent(vtkMRMLPinholeCameraNode::DistortionCoefficientsModifiedEvent);
  }
}

//----------------------------------------------------------------------------
double vtkMRMLPinholeCameraNode::GetDistortionCoefficientValue(vtkIdType idx)
{
  return this->DistortionCoefficients->GetValue(idx);
}

//----------------------------------------------------------------------------
void vtkMRMLPinholeCameraNode::SetCameraPlaneOffsetValue(vtkIdType idx, double value)
{
  if (this->CameraPlaneOffset->GetNumberOfValues() > idx)
  {
    this->CameraPlaneOffset->SetValue(idx, value);
    this->InvokeEvent(vtkMRMLPinholeCameraNode::CameraPlaneOffsetModifiedEvent);
  }
}

//----------------------------------------------------------------------------
double vtkMRMLPinholeCameraNode::GetCameraPlaneOffsetValue(vtkIdType idx)
{
  return this->CameraPlaneOffset->GetValue(idx);
}

//----------------------------------------------------------------------------
vtkMRMLStorageNode* vtkMRMLPinholeCameraNode::CreateDefaultStorageNode()
{
  return vtkMRMLPinholeCameraStorageNode::New();
}

//----------------------------------------------------------------------------
bool vtkMRMLPinholeCameraNode::IsReprojectionErrorValid() const
{
  return this->ReprojectionError != -1.0;
}

//----------------------------------------------------------------------------
bool vtkMRMLPinholeCameraNode::IsRegistrationErrorValid() const
{
  return this->RegistrationError != -1.0;
}

//----------------------------------------------------------------------------
void vtkMRMLPinholeCameraNode::OnIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(IntrinsicsModifiedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLPinholeCameraNode::OnMarkerTransformModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(MarkerToSensorTransformModifiedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLPinholeCameraNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << "Intrinsics: " << std::endl;
  this->IntrinsicMatrix->PrintSelf(os, indent);
  os << "Distortion Coefficients: " << std::endl;
  this->DistortionCoefficients->PrintSelf(os, indent);
  os << "MarkerToSensor Transform: " << std::endl;
  this->MarkerToImageSensorTransform->PrintSelf(os, indent);
  os << "Camera Plane Offset: " << std::endl;
  this->CameraPlaneOffset->PrintSelf(os, indent);
}
