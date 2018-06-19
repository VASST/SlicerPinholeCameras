/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qSlicerCameraModuleWidgetsAbstractPlugin.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "qSlicerCameraModuleWidgetsAbstractPlugin.h"

//-----------------------------------------------------------------------------
qSlicerCameraModuleWidgetsAbstractPlugin::qSlicerCameraModuleWidgetsAbstractPlugin()
{
}

//-----------------------------------------------------------------------------
QString qSlicerCameraModuleWidgetsAbstractPlugin::group() const
{
  return "Slicer [Camera Widgets]";
}

//-----------------------------------------------------------------------------
QIcon qSlicerCameraModuleWidgetsAbstractPlugin::icon() const
{
  return QIcon();
}

//-----------------------------------------------------------------------------
QString qSlicerCameraModuleWidgetsAbstractPlugin::toolTip() const
{
  return QString();
}

//-----------------------------------------------------------------------------
QString qSlicerCameraModuleWidgetsAbstractPlugin::whatsThis() const
{
  return QString();
}