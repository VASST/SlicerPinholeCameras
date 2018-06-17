#include "vtkMRMLCameraNode.h"

// VTK includes
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkObjectFactory.h>
#include <vtkXMLUtilities.h>

// STL includes
#include <sstream>

//----------------------------------------------------------------------------

vtkMRMLNodeNewMacro(vtkMRMLCameraNode);

//----------------------------------------------------------------------------

// Handles the events
static void ProcessEvents(vtkObject* object, unsigned long event, void* clientdata, void* calldata)
{

}

//-----------------------------------------------------------------------------
vtkMRMLCameraNode::vtkMRMLCameraNode()
  : vtkMRMLStorableNode()
  , IntrinsicMatrix(NULL)
  , DistortionCoefficients(NULL)
{

}

//-----------------------------------------------------------------------------
vtkMRMLCameraNode::~vtkMRMLCameraNode()
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
void vtkMRMLCameraNode::Copy(vtkMRMLNode* anode)
{
  int disabledModify = this->StartModify();
  Superclass::Copy(anode);
  vtkMRMLCameraNode* node = vtkMRMLCameraNode::SafeDownCast(anode);

  this->SetAndObserveIntrinsicMatrix(node->GetIntrinsicMatrix());
  this->SetAndObserveDistortionCoefficients(node->GetDistortionCoefficients());

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
void vtkMRMLCameraNode::SetAndObserveIntrinsicMatrix(vtkMatrix3x3* intrinsicMatrix)
{
  if (this->IntrinsicMatrix != NULL)
  {
    this->IntrinsicMatrix->RemoveObserver(this->IntrinsicObserverObserverTag);
  }

  this->SetIntrinsicMatrix(intrinsicMatrix);

  if (this->IntrinsicMatrix != NULL)
  {
    this->IntrinsicObserverObserverTag = this->IntrinsicMatrix->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLCameraNode::OnIntrinsicsModified);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLCameraNode::SetAndObserveDistortionCoefficients(vtkDoubleArray* distCoeffs)
{
  if (this->DistortionCoefficients != NULL)
  {
    this->DistortionCoefficients->RemoveObserver(this->DistortionCoefficientsObserverTag);
  }

  this->SetDistortionCoefficients(distCoeffs);

  if (this->IntrinsicMatrix != NULL)
  {
    this->DistortionCoefficientsObserverTag = this->DistortionCoefficients->AddObserver(vtkCommand::ModifiedEvent, this, &vtkMRMLCameraNode::OnDistortionCoefficientsModified);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLCameraNode::OnIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(IntrinsicsModifiedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLCameraNode::OnDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data)
{
  this->InvokeEvent(DistortionCoefficientsModifiedEvent);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkMRMLCameraNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);

  os << "Intrinsics: " << std::endl;
  this->IntrinsicMatrix->PrintSelf(os, indent);
  os << "Distortion Coefficients: " << std::endl;
  this->DistortionCoefficients->PrintSelf(os, indent);
}
