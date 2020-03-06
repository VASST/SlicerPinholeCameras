/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLVideoCameraStorageNode.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __vtkMRMLVideoCameraStorageNode_h
#define __vtkMRMLVideoCameraStorageNode_h

// MRML includes
#include "vtkSlicerVideoCamerasModuleMRMLExport.h"

// Slicer includes
#include "vtkMRMLStorageNode.h"

class vtkMRMLVideoCameraNode;

/// \brief MRML node for camera storage on disk.
///
/// VideoCameras storage nodes have methods to read/write camera calibration details to/from disk.
class VTK_SLICER_VIDEOCAMERAS_MODULE_MRML_EXPORT vtkMRMLVideoCameraStorageNode : public vtkMRMLStorageNode
{
public:
  static vtkMRMLVideoCameraStorageNode* New();
  vtkTypeMacro(vtkMRMLVideoCameraStorageNode, vtkMRMLStorageNode);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  virtual vtkMRMLNode* CreateNodeInstance() VTK_OVERRIDE;

  ///
  /// Get node XML tag name (like Storage, Model)
  virtual const char* GetNodeTagName() VTK_OVERRIDE  {return "VideoCameraStorage";}

  /// Return true if the reference node can be read in
  virtual bool CanReadInReferenceNode(vtkMRMLNode* refNode) VTK_OVERRIDE;

protected:
  vtkMRMLVideoCameraStorageNode();
  ~vtkMRMLVideoCameraStorageNode();
  vtkMRMLVideoCameraStorageNode(const vtkMRMLVideoCameraStorageNode&);
  void operator=(const vtkMRMLVideoCameraStorageNode&);

  /// Initialize all the supported read file types
  virtual void InitializeSupportedReadFileTypes() VTK_OVERRIDE;

  /// Initialize all the supported write file types
  virtual void InitializeSupportedWriteFileTypes() VTK_OVERRIDE;

  /// Get data node that is associated with this storage node
  vtkMRMLVideoCameraNode* GetAssociatedDataNode();

  /// Read data and set it in the referenced node
  virtual int ReadDataInternal(vtkMRMLNode* refNode) VTK_OVERRIDE;

  /// Write data from a  referenced node
  virtual int WriteDataInternal(vtkMRMLNode* refNode) VTK_OVERRIDE;

};

#endif
