/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLPinholeCameraStorageNode.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __vtkMRMLPinholeCameraStorageNode_h
#define __vtkMRMLPinholeCameraStorageNode_h

// MRML includes
#include "vtkSlicerPinholeCamerasModuleMRMLExport.h"

// Slicer includes
#include "vtkMRMLStorageNode.h"

class vtkMRMLPinholeCameraNode;

/// \brief MRML node for camera storage on disk.
///
/// PinholeCameras storage nodes have methods to read/write camera calibration details to/from disk.
class VTK_SLICER_PINHOLECAMERAS_MODULE_MRML_EXPORT vtkMRMLPinholeCameraStorageNode : public vtkMRMLStorageNode
{
public:
  static vtkMRMLPinholeCameraStorageNode* New();
  vtkTypeMacro(vtkMRMLPinholeCameraStorageNode, vtkMRMLStorageNode);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  virtual vtkMRMLNode* CreateNodeInstance() override;

  ///
  /// Get node XML tag name (like Storage, Model)
  virtual const char* GetNodeTagName() override {return "PinholeCameraStorage";}

  /// Return true if the reference node can be read in
  virtual bool CanReadInReferenceNode(vtkMRMLNode* refNode) override;

protected:
  vtkMRMLPinholeCameraStorageNode();
  ~vtkMRMLPinholeCameraStorageNode();
  vtkMRMLPinholeCameraStorageNode(const vtkMRMLPinholeCameraStorageNode&);
  void operator=(const vtkMRMLPinholeCameraStorageNode&);

  /// Initialize all the supported read file types
  virtual void InitializeSupportedReadFileTypes() override;

  /// Initialize all the supported write file types
  virtual void InitializeSupportedWriteFileTypes() override;

  /// Get data node that is associated with this storage node
  vtkMRMLPinholeCameraNode* GetAssociatedDataNode();

  /// Read data and set it in the referenced node
  virtual int ReadDataInternal(vtkMRMLNode* refNode) override;

  /// Write data from a  referenced node
  virtual int WriteDataInternal(vtkMRMLNode* refNode) override;

};

#endif
