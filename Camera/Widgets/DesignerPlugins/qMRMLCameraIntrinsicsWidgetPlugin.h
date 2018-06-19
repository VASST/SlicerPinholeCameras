/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLCameraIntrinsicsWidgetPlugin.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __qMRMLCameraIntrinsicsWidgetPlugin_h
#define __qMRMLCameraIntrinsicsWidgetPlugin_h

#include "qSlicerCameraModuleWidgetsAbstractPlugin.h"

class Q_SLICER_MODULE_CAMERA_WIDGETS_PLUGINS_EXPORT
  qMRMLCameraIntrinsicsWidgetPlugin
  : public QObject, public qSlicerCameraModuleWidgetsAbstractPlugin
{
  Q_OBJECT

public:
  qMRMLCameraIntrinsicsWidgetPlugin(QObject* _parent = 0);

  QWidget* createWidget(QWidget* _parent);
  QString domXml() const;
  QString includeFile() const;
  bool isContainer() const;
  QString name() const;

};

#endif
