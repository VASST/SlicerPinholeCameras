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

// ITK includes
#include <itksys/Directory.hxx>
#include <itksys/SystemTools.hxx>

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

//----------------------------------------------------------------------------
vtkMRMLWebcamNode* vtkSlicerWebcamsLogic::AddWebcam(const char* filename, const char* nodeName /*= NULL*/)
{
  if (this->GetMRMLScene() == NULL || filename == NULL)
  {
    return NULL;
  }
  vtkNew<vtkMRMLWebcamNode> webcamNode;
  vtkNew<vtkMRMLWebcamStorageNode> mStorageNode;
  vtkSmartPointer<vtkMRMLStorageNode> storageNode;

  mStorageNode->SetFileName(filename);

  const std::string fname(filename);
  // the Webcam node name is based on the file name (itksys call should work even if file is not on disk yet)
  std::string name = itksys::SystemTools::GetFilenameName(fname);

  // check to see which node can read this type of file
  if (mStorageNode->SupportedFileType(name.c_str()))
  {
    storageNode = mStorageNode.GetPointer();
  }

  if (storageNode != NULL)
  {
    std::string baseName = itksys::SystemTools::GetFilenameWithoutExtension(fname);
    std::string uname(this->GetMRMLScene()->GetUniqueNameByString(baseName.c_str()));
    webcamNode->SetName(uname.c_str());

    this->GetMRMLScene()->SaveStateForUndo();

    this->GetMRMLScene()->AddNode(storageNode.GetPointer());

    // Set the scene so that SetAndObserve[Display|Storage]NodeID can find the
    // node in the scene (so that DisplayNodes return something not empty)
    webcamNode->SetScene(this->GetMRMLScene());
    webcamNode->SetAndObserveStorageNodeID(storageNode->GetID());

    this->GetMRMLScene()->AddNode(webcamNode.GetPointer());

    // now set up the reading
    vtkDebugMacro("AddWebcam: calling read on the storage node");
    int retval = storageNode->ReadData(webcamNode.GetPointer());
    if (retval != 1)
    {
      vtkErrorMacro("AddWebcam: error reading " << filename);
      this->GetMRMLScene()->RemoveNode(webcamNode.GetPointer());
      return NULL;
    }
  }
  else
  {
    vtkErrorMacro("Couldn't read file: " << filename);
    return NULL;
  }

  return webcamNode.GetPointer();
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
