/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qSlicerWebcamsModuleWidgetsAbstractPlugin.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "qSlicerWebcamsModuleWidgetsAbstractPlugin.h"

//-----------------------------------------------------------------------------
qSlicerWebcamsModuleWidgetsAbstractPlugin::qSlicerWebcamsModuleWidgetsAbstractPlugin()
{
}

//-----------------------------------------------------------------------------
QString qSlicerWebcamsModuleWidgetsAbstractPlugin::group() const
{
  return "Slicer [Webcams Widgets]";
}

//-----------------------------------------------------------------------------
QIcon qSlicerWebcamsModuleWidgetsAbstractPlugin::icon() const
{
  return QIcon();
}

//-----------------------------------------------------------------------------
QString qSlicerWebcamsModuleWidgetsAbstractPlugin::toolTip() const
{
  return QString();
}

//-----------------------------------------------------------------------------
QString qSlicerWebcamsModuleWidgetsAbstractPlugin::whatsThis() const
{
  return QString();
}