/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkSlicerPinholeCamerasLogic.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// PinholeCameras Logic includes
#include "vtkSlicerPinholeCamerasLogic.h"
#include "vtkMRMLPinholeCameraNode.h"
#include "vtkMRMLPinholeCameraStorageNode.h"

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
vtkStandardNewMacro(vtkSlicerPinholeCamerasLogic);

//----------------------------------------------------------------------------
vtkSlicerPinholeCamerasLogic::vtkSlicerPinholeCamerasLogic()
{
}

//----------------------------------------------------------------------------
vtkSlicerPinholeCamerasLogic::~vtkSlicerPinholeCamerasLogic()
{
}

//----------------------------------------------------------------------------
void vtkSlicerPinholeCamerasLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
vtkMRMLPinholeCameraNode* vtkSlicerPinholeCamerasLogic::AddPinholeCamera(const char* filename, const char* nodeName /*= NULL*/)
{
  if (this->GetMRMLScene() == NULL || filename == NULL)
  {
    return NULL;
  }
  vtkNew<vtkMRMLPinholeCameraNode> videoCameraNode;
  vtkNew<vtkMRMLPinholeCameraStorageNode> mStorageNode;
  vtkSmartPointer<vtkMRMLStorageNode> storageNode;

  mStorageNode->SetFileName(filename);

  const std::string fname(filename);
  // the PinholeCamera node name is based on the file name (itksys call should work even if file is not on disk yet)
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
    vtkDebugMacro("AddPinholeCamera: calling read on the storage node");
    int retval = storageNode->ReadData(videoCameraNode.GetPointer());
    if (retval != 1)
    {
      vtkErrorMacro("AddPinholeCamera: error reading " << filename);
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
void vtkSlicerPinholeCamerasLogic::SetMRMLSceneInternal(vtkMRMLScene* newScene)
{
  vtkNew<vtkIntArray> events;
  events->InsertNextValue(vtkMRMLScene::NodeAddedEvent);
  events->InsertNextValue(vtkMRMLScene::NodeRemovedEvent);
  events->InsertNextValue(vtkMRMLScene::EndBatchProcessEvent);
  this->SetAndObserveMRMLSceneEventsInternal(newScene, events.GetPointer());
}

//-----------------------------------------------------------------------------
void vtkSlicerPinholeCamerasLogic::RegisterNodes()
{
  assert(this->GetMRMLScene() != 0);

  vtkMRMLScene* scene = this->GetMRMLScene();

  // Nodes
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLPinholeCameraNode>::New());
  scene->RegisterNodeClass(vtkSmartPointer<vtkMRMLPinholeCameraStorageNode>::New());
}

//---------------------------------------------------------------------------
void vtkSlicerPinholeCamerasLogic::UpdateFromMRMLScene()
{
  assert(this->GetMRMLScene() != 0);
}

//---------------------------------------------------------------------------
void vtkSlicerPinholeCamerasLogic
::OnMRMLSceneNodeAdded(vtkMRMLNode* vtkNotUsed(node))
{
}

//---------------------------------------------------------------------------
void vtkSlicerPinholeCamerasLogic
::OnMRMLSceneNodeRemoved(vtkMRMLNode* vtkNotUsed(node))
{
}
