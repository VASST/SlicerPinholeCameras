/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkSlicerWebcamsLogic.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// Webcams Logic includes
#include "vtkSlicerWebcamsLogic.h"
#include "vtkMRMLWebcamNode.h"
#include "vtkMRMLWebcamStorageNode.h"

// MRML includes
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkIntArray.h>
#include <vtkNew.h>
#include <vtkObjectFactory.h>

// STD includes
#include <cassert>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkSlicerWebcamsLogic);

//----------------------------------------------------------------------------
vtkSlicerWebcamsLogic::vtkSlicerWebcamsLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerWebcamsLogic::~vtkSlicerWebcamsLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerWebcamsLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//---------------------------------------------------------------------------
void vtkSlicerWebcamsLogic::SetMRMLSceneInternal(vtkMRMLScene* newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerWebcamsLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);

  vtkMRMLScene* scene = this->GetMRMLScene();

  // Nodes
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLWebcamNode>::New());
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLWebcamStorageNode>::New());
}

//---------------------------------------------------------------------------
void vtkSlicerWebcamsLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerWebcamsLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerWebcamsLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}
