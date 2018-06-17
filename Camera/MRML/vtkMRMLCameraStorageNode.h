/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLCameraStorageNode.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __vtkMRMLCameraStorageNode_h
#define __vtkMRMLCameraStorageNode_h

// MRML includes
#include "vtkSlicerCameraModuleMRMLExport.h"

// Slicer includes
#include "vtkMRMLStorageNode.h"

class vtkMRMLCameraNode;

/// \brief MRML node for camera storage on disk.
///
/// Camera storage nodes have methods to read/write camera calibration details to/from disk.
class VTK_SLICER_CAMERA_MODULE_MRML_EXPORT vtkMRMLCameraStorageNode : public vtkMRMLStorageNode
{
public:
  static vtkMRMLCameraStorageNode* New();
  vtkTypeMacro(vtkMRMLCameraStorageNode, vtkMRMLStorageNode);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual vtkMRMLNode* CreateNodeInstance() VTK_OVERRIDE;

  ///
  /// Get node XML tag name (like Storage, Model)
  virtual const char* GetNodeTagName() VTK_OVERRIDE  {return "CameraStorage";}

  /// Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent) VTK_OVERRIDE;

  /// Return true if the reference node can be read in
  virtual bool CanReadInReferenceNode(vtkMRMLNode* refNode) VTK_OVERRIDE;

protected:
  vtkMRMLCameraStorageNode();
  ~vtkMRMLCameraStorageNode();
  vtkMRMLCameraStorageNode(const vtkMRMLCameraStorageNode&);
  void operator=(const vtkMRMLCameraStorageNode&);

  /// Initialize all the supported read file types
  virtual void InitializeSupportedReadFileTypes() VTK_OVERRIDE;

  /// Initialize all the supported write file types
  virtual void InitializeSupportedWriteFileTypes() VTK_OVERRIDE;

  /// Get data node that is associated with this storage node
  vtkMRMLCameraNode* GetAssociatedDataNode();

  /// Read data and set it in the referenced node
  virtual int ReadDataInternal(vtkMRMLNode* refNode) VTK_OVERRIDE;

  /// Write data from a  referenced node
  virtual int WriteDataInternal(vtkMRMLNode* refNode) VTK_OVERRIDE;

};

#endif
