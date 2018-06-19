/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLCameraIntrinsicsWidgetPlugin.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "qMRMLCameraIntrinsicsWidgetPlugin.h"
#include "qMRMLCameraIntrinsicsWidget.h"

//------------------------------------------------------------------------------
qMRMLCameraIntrinsicsWidgetPlugin
::qMRMLCameraIntrinsicsWidgetPlugin(QObject* _parent)
  : QObject(_parent)
{

}

//------------------------------------------------------------------------------
QWidget* qMRMLCameraIntrinsicsWidgetPlugin
::createWidget(QWidget* _parent)
{
  qMRMLCameraIntrinsicsWidget* _widget
    = new qMRMLCameraIntrinsicsWidget(_parent);
  return _widget;
}

//------------------------------------------------------------------------------
QString qMRMLCameraIntrinsicsWidgetPlugin
::domXml() const
{
  return "<widget class=\"qMRMLCameraIntrinsicsWidget\" \
          name=\"CameraIntrinsicsWidget\">\n"
         "</widget>\n";
}

//------------------------------------------------------------------------------
QString qMRMLCameraIntrinsicsWidgetPlugin
::includeFile() const
{
  return "qMRMLCameraIntrinsicsWidget.h";
}

//------------------------------------------------------------------------------
bool qMRMLCameraIntrinsicsWidgetPlugin
::isContainer() const
{
  return false;
}

//------------------------------------------------------------------------------
QString qMRMLCameraIntrinsicsWidgetPlugin
::name() const
{
  return "qMRMLCameraIntrinsicsWidget";
}
