/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLWebcamIntrinsicsWidget.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __qMRMLWebcamIntrinsicsWidget_h
#define __qMRMLWebcamIntrinsicsWidget_h

// qMRMLWidgets includes
#include <vtkMRMLWebcamNode.h>
#include <qSlicerAbstractModuleWidget.h>

// Webcam export includes
#include "qSlicerWebcamsModuleWidgetsExport.h"

// Qt includes
#include <QWidget>

class qMRMLWebcamIntrinsicsWidgetPrivate;

/// \ingroup Slicer_QtModules_Webcam
class Q_SLICER_MODULE_WEBCAMS_WIDGETS_EXPORT qMRMLWebcamIntrinsicsWidget : public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qMRMLWebcamIntrinsicsWidget(QWidget* parent = 0);
  virtual ~qMRMLWebcamIntrinsicsWidget();

public slots:
  virtual void setMRMLScene(vtkMRMLScene*);

protected slots:
  void onWebcamSelectorChanged(vtkMRMLNode* newNode);

protected:
  virtual void setup();

  void OnNodeIntrinsicsModified(vtkObject* caller, unsigned long event, void* data);
  void OnNodeDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data);

  void SetCurrentNode(vtkMRMLWebcamNode* newNode);
  vtkMRMLWebcamNode* GetCurrentNode() const;

protected:
  QScopedPointer<qMRMLWebcamIntrinsicsWidgetPrivate> d_ptr;
  vtkMRMLWebcamNode* CurrentNode;

private:
  Q_DECLARE_PRIVATE(qMRMLWebcamIntrinsicsWidget);
  Q_DISABLE_COPY(qMRMLWebcamIntrinsicsWidget);
};

#endif
