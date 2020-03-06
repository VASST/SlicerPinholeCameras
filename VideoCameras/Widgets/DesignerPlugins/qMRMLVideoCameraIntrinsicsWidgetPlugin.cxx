/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLVideoCameraIntrinsicsWidgetPlugin.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "qMRMLVideoCameraIntrinsicsWidgetPlugin.h"
#include "qMRMLVideoCameraIntrinsicsWidget.h"

//------------------------------------------------------------------------------
qMRMLVideoCameraIntrinsicsWidgetPlugin
::qMRMLVideoCameraIntrinsicsWidgetPlugin(QObject* _parent)
  : QObject(_parent)
{

}

//------------------------------------------------------------------------------
QWidget* qMRMLVideoCameraIntrinsicsWidgetPlugin
::createWidget(QWidget* _parent)
{
  qMRMLVideoCameraIntrinsicsWidget* _widget
    = new qMRMLVideoCameraIntrinsicsWidget(_parent);
  return _widget;
}

//------------------------------------------------------------------------------
QString qMRMLVideoCameraIntrinsicsWidgetPlugin
::domXml() const
{
  return "<widget class=\"qMRMLVideoCameraIntrinsicsWidget\" \
          name=\"VideoCameraIntrinsicsWidget\">\n"
         "</widget>\n";
}

//------------------------------------------------------------------------------
QString qMRMLVideoCameraIntrinsicsWidgetPlugin
::includeFile() const
{
  return "qMRMLVideoCameraIntrinsicsWidget.h";
}

//------------------------------------------------------------------------------
bool qMRMLVideoCameraIntrinsicsWidgetPlugin
::isContainer() const
{
  return false;
}

//------------------------------------------------------------------------------
QString qMRMLVideoCameraIntrinsicsWidgetPlugin
::name() const
{
  return "qMRMLVideoCameraIntrinsicsWidget";
}
