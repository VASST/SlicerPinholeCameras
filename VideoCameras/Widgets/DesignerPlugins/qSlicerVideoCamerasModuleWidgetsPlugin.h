/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qSlicerVideoCamerasModuleWidgetsPlugin.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#ifndef __qSlicerVideoCamerasModuleWidgetsPlugin_h
#define __qSlicerVideoCamerasModuleWidgetsPlugin_h

// Qt includes
#include "vtkSlicerConfigure.h"
#ifdef Slicer_HAVE_QT5
  #include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>
#else
  #include <QDesignerCustomWidgetCollectionInterface>
#endif

// VideoCameras includes
#include "qMRMLVideoCameraIntrinsicsWidgetPlugin.h"

// \class Group the plugins in one library
class Q_SLICER_MODULE_VIDEOCAMERAS_WIDGETS_PLUGINS_EXPORT qSlicerVideoCamerasModuleWidgetsPlugin
  : public QObject
  , public QDesignerCustomWidgetCollectionInterface
{
  Q_OBJECT
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetInterface")
#endif
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface);

public:
  QList<QDesignerCustomWidgetInterface*> customWidgets() const
  {
    QList<QDesignerCustomWidgetInterface*> plugins;
    plugins << new qMRMLVideoCameraIntrinsicsWidgetPlugin;
    return plugins;
  }
};

#endif
