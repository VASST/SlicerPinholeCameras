/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLPinholeCameraIntrinsicsWidget.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __qMRMLPinholeCameraIntrinsicsWidget_h
#define __qMRMLPinholeCameraIntrinsicsWidget_h

// qMRMLWidgets includes
#include <vtkMRMLPinholeCameraNode.h>
#include <qSlicerAbstractModuleWidget.h>

// PinholeCamera export includes
#include "qSlicerPinholeCamerasModuleWidgetsExport.h"

// Qt includes
#include <QWidget>

class QAbstractButton;
class QTableWidgetItem;
class qMRMLPinholeCameraIntrinsicsWidgetPrivate;

/// \ingroup Slicer_QtModules_PinholeCamera
class Q_SLICER_MODULE_PINHOLECAMERAS_WIDGETS_EXPORT qMRMLPinholeCameraIntrinsicsWidget : public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qMRMLPinholeCameraIntrinsicsWidget(QWidget* parent = 0);
  virtual ~qMRMLPinholeCameraIntrinsicsWidget();

  Q_INVOKABLE vtkMRMLPinholeCameraNode* GetCurrentNode() const;

public slots:
  virtual void setMRMLScene(vtkMRMLScene*);

  void copyData();
  void pasteData();

protected slots:
  void onPinholeCameraSelectorChanged(vtkMRMLNode* newNode);
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

  void SetCurrentNode(vtkMRMLPinholeCameraNode* newNode);

protected:
  QScopedPointer<qMRMLPinholeCameraIntrinsicsWidgetPrivate> d_ptr;
  vtkMRMLPinholeCameraNode* CurrentNode;

  unsigned long IntrinsicObserverTag;
  unsigned long DistortionObserverTag;
  unsigned long MarkerTransformObserverTag;
  unsigned long CameraPlaneOffsetObserverTag;

private:
  Q_DECLARE_PRIVATE(qMRMLPinholeCameraIntrinsicsWidget);
  Q_DISABLE_COPY(qMRMLPinholeCameraIntrinsicsWidget);
};

#endif
