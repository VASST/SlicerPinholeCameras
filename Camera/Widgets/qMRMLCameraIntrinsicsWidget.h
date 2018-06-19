/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLCameraIntrinsicsWidget.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __qMRMLCameraIntrinsicsWidget_h
#define __qMRMLCameraIntrinsicsWidget_h

// qMRMLWidgets includes
#include <vtkMRMLWebcamNode.h>
#include <qSlicerAbstractModuleWidget.h>

// Camera export includes
#include "qSlicerCameraModuleWidgetsExport.h"

// Qt includes
#include <QWidget>

class qMRMLCameraIntrinsicsWidgetPrivate;

/// \ingroup Slicer_QtModules_Camera
class Q_SLICER_MODULE_CAMERA_WIDGETS_EXPORT qMRMLCameraIntrinsicsWidget : public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qMRMLCameraIntrinsicsWidget(QWidget* parent = 0);
  virtual ~qMRMLCameraIntrinsicsWidget();

protected slots:
  void onCameraSelectorChanged(vtkMRMLNode* newNode);

protected:
  virtual void setup();
  virtual void setMRMLScene(vtkMRMLScene*);

  void OnNodeIntrinsicsModified(vtkObject* caller, unsigned long event, void* data);
  void OnNodeDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data);

  void SetCurrentNode(vtkMRMLWebcamNode* newNode);
  vtkMRMLWebcamNode* GetCurrentNode() const;

protected:
  QScopedPointer<qMRMLCameraIntrinsicsWidgetPrivate> d_ptr;
  vtkMRMLWebcamNode* CurrentNode;

private:
  Q_DECLARE_PRIVATE(qMRMLCameraIntrinsicsWidget);
  Q_DISABLE_COPY(qMRMLCameraIntrinsicsWidget);
};

#endif
