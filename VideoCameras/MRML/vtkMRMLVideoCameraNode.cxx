/*=auto=========================================================================

CLortions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLVideoCameraNode.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "vtkMRMLVideoCameraNode.h"
#include "vtkMRMLVideoCameraStorageNode.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkXMLUtilities.h>

// STL includes
#include <sstream>

//----------------------------------------------------------------------------

vtkMRMLNodeNewMacro(vtkMRMLVideoCameraNode);

//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
vtkMRMLVideoCameraNode::vtkMRMLVideoCameraNode()
  : vtkMRMLStorableNode()
  , IntrinsicMatrix(nullptr)
  , DistortionCoefficients(nullptr)
  , MarkerToImageSensorTransform(nullptr)
{
  this->SetAndObserveIntrinsicMatrix(vtkMatrix3x3::New());
  this->SetAndObserveDistortionCoefficients(vtkDoubleArray::New());
  this->GetDistortionCoefficients()->SetNumberOfValues(5);
  this->GetDistortionCoefficients()->FillValue(0.0);
  this->SetAndObserveMarkerToImageSensorTransform(vtkMatrix4x4::New());
}

//-----------------------------------------------------------------------------
vtkMRMLVideoCameraNode::~vtkMRMLVideoCameraNode()
{
  this->SetAndObserveIntrinsicMatrix(nullptr);
  this->SetAndObserveDistortionCoefficients(nullptr);
  this->SetAndObserveMarkerToImageSensorTransform(nullptr);
}

//----------------------------------------------------------------------------
void vtkMRMLVideoCameraNode::Copy(vtkMRMLNode* anode)
{
  int disabledModify = this->StartModify();
  Superclass::Copy(anode);
  vtkMRMLVideoCameraNode* node = vtkMRMLVideoCameraNode::SafeDownCast(anode);

  this->SetAndObserveIntrinsicMatrix(node->GetIntrinsicMatrix());
  this->SetAndObserveDistortionCoefficients(node->GetDistortionCoefficients());
  this->SetAndObserveMarkerToImageSensorTransform(node->GetMarkerToImageSensorTransform());

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLVideoCameraNode::SetAndObserveIntrinsicMatrix(vtkMatrix3x3* intrinsicMatrix)
{
  if (this->IntrinsicMatrix != NULL)
  {
    this->IntrinsicMatrix->RemoveObserver(this->IntrinsicObserverObserverTag);
  }

  this->SetIntrinsicMatrix(intrinsicMatrix);

  if (this->IntrinsicMatrix != NULL)
  {
    this->IntrinsicObserverObserverTag = this->IntrinsicMatrix->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLVideoCameraNode::OnIntrinsicsModified);
  }

  this->InvokeEvent(vtkMRMLVideoCameraNode::IntrinsicsModifiedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLVideoCameraNode::SetAndObserveDistortionCoefficients(vtkDoubleArray* distCoeffs)
{
  if (this->DistortionCoefficients != NULL)
  {
    this->DistortionCoefficients->RemoveObserver(this->DistortionCoefficientsObserverTag);
  }

  this->SetDistortionCoefficients(distCoeffs);

  if (this->DistortionCoefficients != NULL)
  {
    this->DistortionCoefficientsObserverTag = this->DistortionCoefficients->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLVideoCameraNode::OnDistortionCoefficientsModified);
  }

  this->InvokeEvent(vtkMRMLVideoCameraNode::DistortionCoefficientsModifiedEvent);
}

//----------------------------------------------------------------------------
void vtkMRMLVideoCameraNode::SetAndObserveMarkerToImageSensorTransform(vtkMatrix4x4* markerToImageSensorTransform)
{
  if (this->MarkerToImageSensorTransform != NULL)
  {
    this->MarkerToImageSensorTransform->RemoveObserver(this->MarkerTransformObserverTag);
  }

  this->SetMarkerToImageSensorTransform(markerToImageSensorTransform);

  if (this->MarkerToImageSensorTransform != NULL)
  {
    this->MarkerTransformObserverTag = this->MarkerToImageSensorTransform->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLVideoCameraNode::OnMarkerTransformModified);
  }

  this->InvokeEvent(vtkMRMLVideoCameraNode::MarkerToSensorTransformModifiedEvent);
}

//----------------------------------------------------------------------------
vtkMRMLStorageNode* vtkMRMLVideoCameraNode::CreateDefaultStorageNode()
{
  return vtkMRMLVideoCameraStorageNode::New();
}

//----------------------------------------------------------------------------
void vtkMRMLVideoCameraNode::OnIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(IntrinsicsModifiedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLVideoCameraNode::OnDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(DistortionCoefficientsModifiedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLVideoCameraNode::OnMarkerTransformModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(MarkerToSensorTransformModifiedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLVideoCameraNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << "Intrinsics: " << std::endl;
  this->IntrinsicMatrix->PrintSelf(os, indent);
  os << "Distortion Coefficients: " << std::endl;
  this->DistortionCoefficients->PrintSelf(os, indent);
  os << "MarkerToSensor Transform: " << std::endl;
  this->MarkerToImageSensorTransform->PrintSelf(os, indent);
}
