/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLVideoCameraIntrinsicsWidget.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __qMRMLVideoCameraIntrinsicsWidget_h
#define __qMRMLVideoCameraIntrinsicsWidget_h

// qMRMLWidgets includes
#include <vtkMRMLVideoCameraNode.h>
#include <qSlicerAbstractModuleWidget.h>

// VideoCamera export includes
#include "qSlicerVideoCamerasModuleWidgetsExport.h"

// Qt includes
#include <QWidget>

class QAbstractButton;
class QTableWidgetItem;
class qMRMLVideoCameraIntrinsicsWidgetPrivate;

/// \ingroup Slicer_QtModules_VideoCamera
class Q_SLICER_MODULE_VIDEOCAMERAS_WIDGETS_EXPORT qMRMLVideoCameraIntrinsicsWidget : public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qMRMLVideoCameraIntrinsicsWidget(QWidget* parent = 0);
  virtual ~qMRMLVideoCameraIntrinsicsWidget();

  Q_INVOKABLE vtkMRMLVideoCameraNode* GetCurrentNode() const;

public slots:
  virtual void setMRMLScene(vtkMRMLScene*);

  void copyData();
  void pasteData();

protected slots:
  void onVideoCameraSelectorChanged(vtkMRMLNode* newNode);
  void onIntrinsicMatrixChanged();
  void onDistortionMatrixChanged();
  void onMarkerTransformMatrixChanged();
  void onCameraPlaneOffsetMatrixChanged();

protected:
  virtual void setup();

  void OnNodeIntrinsicsModified(vtkObject* caller, unsigned long event, void* data);
  void OnNodeDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data);
  void OnNodeMarkerTransformModified(vtkObject* caller, unsigned long event, void* data);
  void OnCameraPlaneOffsetModified(vtkObject* caller, unsigned long event, void* data);

  void SetCurrentNode(vtkMRMLVideoCameraNode* newNode);

protected:
  QScopedPointer<qMRMLVideoCameraIntrinsicsWidgetPrivate> d_ptr;
  vtkMRMLVideoCameraNode* CurrentNode;

  unsigned long IntrinsicObserverTag;
  unsigned long DistortionObserverTag;
  unsigned long MarkerTransformObserverTag;
  unsigned long CameraPlaneOffsetObserverTag;

private:
  Q_DECLARE_PRIVATE(qMRMLVideoCameraIntrinsicsWidget);
  Q_DISABLE_COPY(qMRMLVideoCameraIntrinsicsWidget);
};

#endif
