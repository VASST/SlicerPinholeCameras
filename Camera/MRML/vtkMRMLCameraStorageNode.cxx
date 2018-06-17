/*=auto=========================================================================

  Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLCameraStorageNode.cxx,v $
  Date:      $Date: 2018/6/16 10:54:09 $
  Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "vtkMRMLCameraNode.h"
#include "vtkMRMLCameraStorageNode.h"
#include "vtkMRMLScene.h"

// VTK includes
#include <vtkObjectFactory.h>
#include <vtkStringArray.h>
#include <vtkVersion.h>
#include <vtksys/SystemTools.hxx>

// OpenCV includes
#include <opencv2/videoio.hpp>
#include <opencv2/core/persistence.hpp>

//----------------------------------------------------------------------------

vtkMRMLNodeNewMacro(vtkMRMLCameraStorageNode);

//----------------------------------------------------------------------------
vtkMRMLCameraStorageNode::vtkMRMLCameraStorageNode()
{
  this->DefaultWriteFileExtension = "xml";
}

//----------------------------------------------------------------------------
vtkMRMLCameraStorageNode::~vtkMRMLCameraStorageNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLCameraStorageNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLStorageNode::PrintSelf(os, indent);
}

//----------------------------------------------------------------------------
void vtkMRMLCameraStorageNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
bool vtkMRMLCameraStorageNode::CanReadInReferenceNode(vtkMRMLNode* refNode)
{
  return refNode->IsA("vtkMRMLCameraStorageNode");
}

//----------------------------------------------------------------------------
int vtkMRMLCameraStorageNode::ReadDataInternal(vtkMRMLNode* refNode)
{
  vtkMRMLCameraNode* cameraNode = dynamic_cast <vtkMRMLCameraNode*>(refNode);

  std::string fullName = this->GetFullNameFromFileName();
  if (fullName.empty())
  {
    vtkErrorMacro("File name not specified");
    return 0;
  }

  // check that the file exists
  if (vtksys::SystemTools::FileExists(fullName.c_str()) == false)
  {
    vtkErrorMacro("Camera file '" << fullName.c_str() << "' not found.");
    return 0;
  }

  // compute file prefix
  cv::FileStorage fs(fullName, cv::FileStorage::READ);
  if (!fs.isOpened())
  {
    vtkErrorMacro("File cannot be opened for reading.");
    return 0;
  }

  cv::Mat intrinMat;
  cv::Mat distCoeffs;
  cv::FileNode intrinNode = fs["IntrinsicMatrix"];
  if (intrinNode.empty())
  {
    vtkErrorMacro("Intrinsics file does not contain IntrinsicMatrix cv::Mat.");
    return 0;
  }
  intrinNode >> intrinMat;

  cv::FileNode distCoeffsNode = fs["DistortionCoefficients"];
  if (distCoeffsNode.empty())
  {
    vtkErrorMacro("Intrinsics file does not contain DistortionCoefficients cv::Mat.");
    return 0;
  }
  distCoeffsNode >> distCoeffs;

  intrinMat.convertTo(intrinMat, CV_64F);
  distCoeffs.convertTo(distCoeffs, CV_64F);

  vtkNew<vtkMatrix3x3> mat;
  for (int i = 0; i < 3; ++i)
  {
    for (int j = 0; j < 3; ++j)
    {
      mat->SetElement(i, j, intrinMat.at<double>(i, j));
    }
  }
  vtkNew<vtkDoubleArray> array;
  for (int i = 0; i < distCoeffs.rows; ++i)
  {
    array->InsertNextValue(distCoeffs.at<double>(i, 0));
  }

  cameraNode->SetAndObserveIntrinsicMatrix(mat);
  cameraNode->SetAndObserveDistortionCoefficients(array);

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLCameraStorageNode::WriteDataInternal(vtkMRMLNode* refNode)
{
  vtkMRMLCameraNode* modelNode = vtkMRMLCameraNode::SafeDownCast(refNode);

  std::string fullName = this->GetFullNameFromFileName();
  if (fullName.empty())
  {
    vtkErrorMacro("File name not specified");
    return 0;
  }

  vtkMRMLCameraNode* node = this->GetAssociatedDataNode();

  cv::FileStorage fs(fullName, cv::FileStorage::WRITE);

  if (!fs.isOpened())
  {
    vtkErrorMacro("Cannot open " << fullName << " for writing.");
    return 0;
  }

  if (node->GetIntrinsicMatrix() == NULL)
  {
    vtkInfoMacro("Intrinsincs have not been determined for this camera.");
  }
  else
  {
    cv::Mat intrinMat(3, 3, CV_64F);
    vtkNew<vtkMatrix3x3> mat;
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        intrinMat.at<double>(i, j) = node->GetIntrinsicMatrix()->GetElement(i, j);
      }
    }
    fs << "IntrinsicMatrix" << intrinMat;
  }

  if (node->GetDistortionCoefficients() == NULL)
  {
    vtkInfoMacro("Distortion coefficients have not been determined for this camera.");
  }
  else
  {
    cv::Mat distCoeffs(node->GetDistortionCoefficients()->GetSize(), 1, CV_64F);
    for (int i = 0; i < node->GetDistortionCoefficients()->GetSize(); ++i)
    {
      distCoeffs.at<double>(i, 0) = node->GetDistortionCoefficients()->GetValue(i);
    }
    fs << "DistortionCoefficients" << distCoeffs;
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkMRMLCameraStorageNode::InitializeSupportedReadFileTypes()
{
  this->SupportedReadFileTypes->InsertNextValue("OpenCV XML (.xml)");
}

//----------------------------------------------------------------------------
void vtkMRMLCameraStorageNode::InitializeSupportedWriteFileTypes()
{
  this->SupportedWriteFileTypes->InsertNextValue("OpenCV XML (.xml)");
}

//----------------------------------------------------------------------------
vtkMRMLCameraNode* vtkMRMLCameraStorageNode::GetAssociatedDataNode()
{
  if (!this->GetScene())
  {
    return NULL;
  }

  std::vector<vtkMRMLNode*> nodes;
  unsigned int numberOfNodes = this->GetScene()->GetNodesByClass("vtkMRMLCameraNode", nodes);
  for (unsigned int nodeIndex = 0; nodeIndex < numberOfNodes; nodeIndex++)
  {
    vtkMRMLCameraNode* node = vtkMRMLCameraNode::SafeDownCast(nodes[nodeIndex]);
    if (node)
    {
      const char* storageNodeID = node->GetStorageNodeID();
      if (storageNodeID && !strcmp(storageNodeID, this->ID))
      {
        return vtkMRMLCameraNode::SafeDownCast(node);
      }
    }
  }

  return NULL;
}
