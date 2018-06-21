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

// Handles the events
static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata)
{

}

//-----------------------------------------------------------------------------
vtkMRMLWebcamNode::vtkMRMLWebcamNode()
  : vtkMRMLStorableNode()
  , IntrinsicMatrix(NULL)
  , DistortionCoefficients(NULL)
{

}

//-----------------------------------------------------------------------------
vtkMRMLWebcamNode::~vtkMRMLWebcamNode()
{
  if (this->IntrinsicMatrix != NULL)
  {
    this->IntrinsicMatrix->RemoveAllObservers();
    this->IntrinsicMatrix->Delete();
  }
  if (this->DistortionCoefficients != NULL)
  {
    this->DistortionCoefficients->RemoveAllObservers();
    this->DistortionCoefficients->Delete();
  }
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::Copy(vtkMRMLNode* anode)
{
  int disabledModify = this->StartModify();
  Superclass::Copy(anode);
  vtkMRMLWebcamNode* node = vtkMRMLWebcamNode::SafeDownCast(anode);

  this->SetAndObserveIntrinsicMatrix(node->GetIntrinsicMatrix());
  this->SetAndObserveDistortionCoefficients(node->GetDistortionCoefficients());

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

  if (this->IntrinsicMatrix != NULL)
  {
    this->DistortionCoefficientsObserverTag = this->DistortionCoefficients->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLWebcamNode::OnDistortionCoefficientsModified);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::OnIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(IntrinsicsModifiedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::OnDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(DistortionCoefficientsModifiedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLWebcamNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << "Intrinsics: " << std::endl;
  this->IntrinsicMatrix->PrintSelf(os, indent);
  os << "Distortion Coefficients: " << std::endl;
  this->DistortionCoefficients->PrintSelf(os, indent);
}
