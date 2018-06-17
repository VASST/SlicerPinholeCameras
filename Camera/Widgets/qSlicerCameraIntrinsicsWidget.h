/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLCameraNode.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __qSlicerCameraIntrinsicsWidget_h
#define __qSlicerCameraIntrinsicsWidget_h

// qMRMLWidgets includes
#include <vtkMRMLCameraNode.h>
#include <qSlicerAbstractModuleWidget.h>

// Camera export includes
#include "qSlicerCameraModuleWidgetsExport.h"

// Qt includes
#include <QWidget>

class qSlicerCameraIntrinsicsWidgetPrivate;

/// \ingroup Slicer_QtModules_Camera
class Q_SLICER_MODULE_CAMERA_WIDGETS_EXPORT qSlicerCameraIntrinsicsWidget : public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerCameraIntrinsicsWidget(QWidget* parent = 0);
  virtual ~qSlicerCameraIntrinsicsWidget();

protected slots:
  void onCameraSelectorChanged(vtkMRMLNode* newNode);

protected:
  virtual void setup();
  virtual void setMRMLScene(vtkMRMLScene*);

  void OnNodeIntrinsicsModified(vtkObject* caller, unsigned long event, void* data);
  void OnNodeDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data);

  void SetCurrentNode(vtkMRMLCameraNode* newNode);
  vtkMRMLCameraNode* GetCurrentNode() const;

protected:
  QScopedPointer<qSlicerCameraIntrinsicsWidgetPrivate> d_ptr;
  vtkMRMLCameraNode* CurrentNode;

private:
  Q_DECLARE_PRIVATE(qSlicerCameraIntrinsicsWidget);
  Q_DISABLE_COPY(qSlicerCameraIntrinsicsWidget);
};

#endif
