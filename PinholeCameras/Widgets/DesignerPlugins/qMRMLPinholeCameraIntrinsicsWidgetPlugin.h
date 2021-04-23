/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLPinholeCameraIntrinsicsWidgetPlugin.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __qMRMLPinholeCameraIntrinsicsWidgetPlugin_h
#define __qMRMLPinholeCameraIntrinsicsWidgetPlugin_h

#include "qSlicerPinholeCamerasModuleWidgetsAbstractPlugin.h"

class Q_SLICER_MODULE_PINHOLECAMERAS_WIDGETS_PLUGINS_EXPORT
  qMRMLPinholeCameraIntrinsicsWidgetPlugin
  : public QObject, public qSlicerPinholeCamerasModuleWidgetsAbstractPlugin
{
  Q_OBJECT

public:
  qMRMLPinholeCameraIntrinsicsWidgetPlugin(QObject* _parent = 0);

  QWidget* createWidget(QWidget* _parent);
  QString domXml() const;
  QString includeFile() const;
  bool isContainer() const;
  QString name() const;

};

#endif
