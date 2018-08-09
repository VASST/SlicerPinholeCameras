/*=auto=========================================================================

CLortions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLWebcamNode.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "vtkMRMLWebcamNode.h"
#include "vtkMRMLWebcamStorageNode.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkXMLUtilities.h>

// STL includes
#include <sstream>

//----------------------------------------------------------------------------

vtkMRMLNodeNewMacro(vtkMRMLWebcamNode);

//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
vtkMRMLWebcamNode::vtkMRMLWebcamNode()
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
vtkMRMLWebcamNode::~vtkMRMLWebcamNode()
{
  this->SetAndObserveIntrinsicMatrix(nullptr);
  this->SetAndObserveDistortionCoefficients(nullptr);
  this->SetAndObserveMarkerToImageSensorTransform(nullptr);
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::Copy(vtkMRMLNode* anode)
{
  int disabledModify = this->StartModify();
  Superclass::Copy(anode);
  vtkMRMLWebcamNode* node = vtkMRMLWebcamNode::SafeDownCast(anode);

  this->SetAndObserveIntrinsicMatrix(node->GetIntrinsicMatrix());
  this->SetAndObserveDistortionCoefficients(node->GetDistortionCoefficients());
  this->SetAndObserveMarkerToImageSensorTransform(node->GetMarkerToImageSensorTransform());

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::SetAndObserveIntrinsicMatrix(vtkMatrix3x3* intrinsicMatrix)
{
  if (this->IntrinsicMatrix != NULL)
  {
    this->IntrinsicMatrix->RemoveObserver(this->IntrinsicObserverObserverTag);
  }

  this->SetIntrinsicMatrix(intrinsicMatrix);

  if (this->IntrinsicMatrix != NULL)
  {
    this->IntrinsicObserverObserverTag = this->IntrinsicMatrix->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLWebcamNode::OnIntrinsicsModified);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::SetAndObserveDistortionCoefficients(vtkDoubleArray* distCoeffs)
{
  if (this->DistortionCoefficients != NULL)
  {
    this->DistortionCoefficients->RemoveObserver(this->DistortionCoefficientsObserverTag);
  }

  this->SetDistortionCoefficients(distCoeffs);

  if (this->DistortionCoefficients != NULL)
  {
    this->DistortionCoefficientsObserverTag = this->DistortionCoefficients->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLWebcamNode::OnDistortionCoefficientsModified);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::SetAndObserveMarkerToImageSensorTransform(vtkMatrix4x4* markerToImageSensorTransform)
{
  if (this->MarkerToImageSensorTransform != NULL)
  {
    this->MarkerToImageSensorTransform->RemoveObserver(this->MarkerTransformObserverTag);
  }

  this->SetMarkerToImageSensorTransform(markerToImageSensorTransform);

  if (this->MarkerToImageSensorTransform != NULL)
  {
    this->MarkerTransformObserverTag = this->MarkerToImageSensorTransform->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLWebcamNode::OnMarkerTransformModified);
  }
}

//----------------------------------------------------------------------------
vtkMRMLStorageNode* vtkMRMLWebcamNode::CreateDefaultStorageNode()
{
  return vtkMRMLWebcamStorageNode::New();
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::OnIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(IntrinsicsModifiedEvent);
  this->Modified();

  vtkErrorMacro("intrin mod");
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::OnDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(DistortionCoefficientsModifiedEvent);
  this->Modified();

  vtkErrorMacro("dist mod");
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::OnMarkerTransformModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(MarkerToSensorTransformModifiedEvent);
  this->Modified();

  vtkErrorMacro("marker mod");
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << "Intrinsics: " << std::endl;
  this->IntrinsicMatrix->PrintSelf(os, indent);
  os << "Distortion Coefficients: " << std::endl;
  this->DistortionCoefficients->PrintSelf(os, indent);
  os << "MarkerToSensor Transform: " << std::endl;
  this->MarkerToImageSensorTransform->PrintSelf(os, indent);
}
