/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLWebcamIntrinsicsWidgetPlugin.cxx,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

#include "qMRMLWebcamIntrinsicsWidgetPlugin.h"
#include "qMRMLWebcamIntrinsicsWidget.h"

//------------------------------------------------------------------------------
qMRMLWebcamIntrinsicsWidgetPlugin
::qMRMLWebcamIntrinsicsWidgetPlugin(QObject* _parent)
  : QObject(_parent)
{

}

//------------------------------------------------------------------------------
QWidget* qMRMLWebcamIntrinsicsWidgetPlugin
::createWidget(QWidget* _parent)
{
  qMRMLWebcamIntrinsicsWidget* _widget
    = new qMRMLWebcamIntrinsicsWidget(_parent);
  return _widget;
}

//------------------------------------------------------------------------------
QString qMRMLWebcamIntrinsicsWidgetPlugin
::domXml() const
{
  return "<widget class=\"qMRMLWebcamIntrinsicsWidget\" \
          name=\"WebcamIntrinsicsWidget\">\n"
         "</widget>\n";
}

//------------------------------------------------------------------------------
QString qMRMLWebcamIntrinsicsWidgetPlugin
::includeFile() const
{
  return "qMRMLWebcamIntrinsicsWidget.h";
}

//------------------------------------------------------------------------------
bool qMRMLWebcamIntrinsicsWidgetPlugin
::isContainer() const
{
  return false;
}

//------------------------------------------------------------------------------
QString qMRMLWebcamIntrinsicsWidgetPlugin
::name() const
{
  return "qMRMLWebcamIntrinsicsWidget";
}
