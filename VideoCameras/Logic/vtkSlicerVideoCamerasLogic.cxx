/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkSlicerVideoCamerasLogic.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// VideoCameras Logic includes
#include "vtkSlicerVideoCamerasLogic.h"
#include "vtkMRMLVideoCameraNode.h"
#include "vtkMRMLVideoCameraStorageNode.h"

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
vtkStandardNewMacro(vtkSlicerVideoCamerasLogic);

//----------------------------------------------------------------------------
vtkSlicerVideoCamerasLogic::vtkSlicerVideoCamerasLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerVideoCamerasLogic::~vtkSlicerVideoCamerasLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerVideoCamerasLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkMRMLVideoCameraNode* vtkSlicerVideoCamerasLogic::AddVideoCamera(const char* filename, const char* nodeName /*= NULL*/)
{
  if (this->GetMRMLScene() == NULL || filename == NULL)
  {
    return NULL;
  }
  vtkNew<vtkMRMLVideoCameraNode> videoCameraNode;
  vtkNew<vtkMRMLVideoCameraStorageNode> mStorageNode;
  vtkSmartPointer<vtkMRMLStorageNode> storageNode;

  mStorageNode->SetFileName(filename);

  const std::string fname(filename);
  // the VideoCamera node name is based on the file name (itksys call should work even if file is not on disk yet)
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
    videoCameraNode->SetName(uname.c_str());

    this->GetMRMLScene()->SaveStateForUndo();

    this->GetMRMLScene()->AddNode(storageNode.GetPointer());

    // Set the scene so that SetAndObserve[Display|Storage]NodeID can find the
    // node in the scene (so that DisplayNodes return something not empty)
    videoCameraNode->SetScene(this->GetMRMLScene());
    videoCameraNode->SetAndObserveStorageNodeID(storageNode->GetID());

    this->GetMRMLScene()->AddNode(videoCameraNode.GetPointer());

    // now set up the reading
    vtkDebugMacro("AddVideoCamera: calling read on the storage node");
    int retval = storageNode->ReadData(videoCameraNode.GetPointer());
    if (retval != 1)
    {
      vtkErrorMacro("AddVideoCamera: error reading " << filename);
      this->GetMRMLScene()->RemoveNode(videoCameraNode.GetPointer());
      return NULL;
    }
  }
  else
  {
    vtkErrorMacro("Couldn't read file: " << filename);
    return NULL;
  }

  return videoCameraNode.GetPointer();
}

//---------------------------------------------------------------------------
void vtkSlicerVideoCamerasLogic::SetMRMLSceneInternal(vtkMRMLScene* newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerVideoCamerasLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);

  vtkMRMLScene* scene = this->GetMRMLScene();

  // Nodes
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLVideoCameraNode>::New());
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLVideoCameraStorageNode>::New());
}

//---------------------------------------------------------------------------
void vtkSlicerVideoCamerasLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerVideoCamerasLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerVideoCamerasLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}
