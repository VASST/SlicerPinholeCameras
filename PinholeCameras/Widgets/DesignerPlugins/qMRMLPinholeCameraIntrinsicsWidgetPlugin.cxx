/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLPinholeCameraIntrinsicsWidgetPlugin.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "qMRMLPinholeCameraIntrinsicsWidgetPlugin.h"
#include "qMRMLPinholeCameraIntrinsicsWidget.h"

//------------------------------------------------------------------------------
qMRMLPinholeCameraIntrinsicsWidgetPlugin
::qMRMLPinholeCameraIntrinsicsWidgetPlugin(QObject* _parent)
  : QObject(_parent)
{

}

//------------------------------------------------------------------------------
QWidget* qMRMLPinholeCameraIntrinsicsWidgetPlugin
::createWidget(QWidget* _parent)
{
  qMRMLPinholeCameraIntrinsicsWidget* _widget
    = new qMRMLPinholeCameraIntrinsicsWidget(_parent);
  return _widget;
}

//------------------------------------------------------------------------------
QString qMRMLPinholeCameraIntrinsicsWidgetPlugin
::domXml() const
{
  return "<widget class=\"qMRMLPinholeCameraIntrinsicsWidget\" \
          name=\"PinholeCameraIntrinsicsWidget\">\n"
         "</widget>\n";
}

//------------------------------------------------------------------------------
QString qMRMLPinholeCameraIntrinsicsWidgetPlugin
::includeFile() const
{
  return "qMRMLPinholeCameraIntrinsicsWidget.h";
}

//------------------------------------------------------------------------------
bool qMRMLPinholeCameraIntrinsicsWidgetPlugin
::isContainer() const
{
  return false;
}

//------------------------------------------------------------------------------
QString qMRMLPinholeCameraIntrinsicsWidgetPlugin
::name() const
{
  return "qMRMLPinholeCameraIntrinsicsWidget";
}
