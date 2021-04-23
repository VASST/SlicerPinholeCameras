/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qSlicerPinholeCamerasModuleWidgetsAbstractPlugin.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "qSlicerPinholeCamerasModuleWidgetsAbstractPlugin.h"

//-----------------------------------------------------------------------------
qSlicerPinholeCamerasModuleWidgetsAbstractPlugin::qSlicerPinholeCamerasModuleWidgetsAbstractPlugin()
{
}

//-----------------------------------------------------------------------------
QString qSlicerPinholeCamerasModuleWidgetsAbstractPlugin::group() const
{
  return "Slicer [PinholeCameras Widgets]";
}

//-----------------------------------------------------------------------------
QIcon qSlicerPinholeCamerasModuleWidgetsAbstractPlugin::icon() const
{
  return QIcon();
}

//-----------------------------------------------------------------------------
QString qSlicerPinholeCamerasModuleWidgetsAbstractPlugin::toolTip() const
{
  return QString();
}

//-----------------------------------------------------------------------------
QString qSlicerPinholeCamerasModuleWidgetsAbstractPlugin::whatsThis() const
{
  return QString();
}