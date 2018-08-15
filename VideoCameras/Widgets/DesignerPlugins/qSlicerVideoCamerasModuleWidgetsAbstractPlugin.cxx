/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qSlicerVideoCamerasModuleWidgetsAbstractPlugin.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "qSlicerVideoCamerasModuleWidgetsAbstractPlugin.h"

//-----------------------------------------------------------------------------
qSlicerVideoCamerasModuleWidgetsAbstractPlugin::qSlicerVideoCamerasModuleWidgetsAbstractPlugin()
{
}

//-----------------------------------------------------------------------------
QString qSlicerVideoCamerasModuleWidgetsAbstractPlugin::group() const
{
  return "Slicer [VideoCameras Widgets]";
}

//-----------------------------------------------------------------------------
QIcon qSlicerVideoCamerasModuleWidgetsAbstractPlugin::icon() const
{
  return QIcon();
}

//-----------------------------------------------------------------------------
QString qSlicerVideoCamerasModuleWidgetsAbstractPlugin::toolTip() const
{
  return QString();
}

//-----------------------------------------------------------------------------
QString qSlicerVideoCamerasModuleWidgetsAbstractPlugin::whatsThis() const
{
  return QString();
}