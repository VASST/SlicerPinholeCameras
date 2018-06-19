/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLCameraIntrinsicsWidget.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// Qt includes

// Local includes
#include "qMRMLCameraIntrinsicsWidget.h"
#include "vtkSlicerCameraLogic.h"
#include "ui_qMRMLCameraIntrinsicsWidget.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>

// VTK includes

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Camera
class qMRMLCameraIntrinsicsWidgetPrivate : public Ui_qMRMLCameraIntrinsicsWidget
{
  Q_DECLARE_PUBLIC(qMRMLCameraIntrinsicsWidget);
protected:
  qMRMLCameraIntrinsicsWidget* const q_ptr;
public:
  qMRMLCameraIntrinsicsWidgetPrivate(qMRMLCameraIntrinsicsWidget& object);

  vtkSlicerCameraLogic* logic();
};

//-----------------------------------------------------------------------------
// qMRMLCameraIntrinsicsWidgetPrivate methods

//-----------------------------------------------------------------------------
qMRMLCameraIntrinsicsWidgetPrivate::qMRMLCameraIntrinsicsWidgetPrivate(qMRMLCameraIntrinsicsWidget& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
vtkSlicerCameraLogic* qMRMLCameraIntrinsicsWidgetPrivate::logic()
{
  Q_Q(qMRMLCameraIntrinsicsWidget);
  return vtkSlicerCameraLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
// qMRMLCameraIntrinsicsWidget methods

//------------------------------------------------------------------------------
qMRMLCameraIntrinsicsWidget::qMRMLCameraIntrinsicsWidget(QWidget* vparent)
  : qSlicerAbstractModuleWidget(vparent)
{

}

//------------------------------------------------------------------------------
qMRMLCameraIntrinsicsWidget::~qMRMLCameraIntrinsicsWidget()
{
}

//----------------------------------------------------------------------------
void qMRMLCameraIntrinsicsWidget::onCameraSelectorChanged(vtkMRMLNode* newNode)
{
  Q_D(qMRMLCameraIntrinsicsWidget);

  vtkMRMLWebcamNode* camNode = vtkMRMLWebcamNode::SafeDownCast(newNode);

  SetCurrentNode(camNode);

  if (this->CurrentNode != nullptr)
  {
    if (camNode->GetIntrinsicMatrix() != nullptr)
    {
      for (int i = 0; i < 3; ++i)
      {
        for (int j = 0; j < 3; ++j)
        {
          d->tableWidget_CameraMatrix->itemAt(i, j)->setText(QString::number(camNode->GetIntrinsicMatrix()->GetElement(i, j)));
        }
      }
    }

    if (camNode->GetDistortionCoefficients())
    {
      d->tableWidget_DistCoeffs->clear();
      d->tableWidget_DistCoeffs->insertRow(0);

      for (int i = 0; i < camNode->GetDistortionCoefficients()->GetNumberOfValues(); ++i)
      {
        d->tableWidget_DistCoeffs->insertColumn(d->tableWidget_DistCoeffs->columnCount());
        d->tableWidget_DistCoeffs->setItem(0, i, new QTableWidgetItem(QString::number(camNode->GetDistortionCoefficients()->GetValue(i))));
      }
    }
  }
}

//----------------------------------------------------------------------------
void qMRMLCameraIntrinsicsWidget::setup()
{
  Q_D(qMRMLCameraIntrinsicsWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // --------------------------------------------------
  // Connectors section
  connect(d->comboBox_CameraSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onCameraSelectorChanged(vtkMRMLNode*)));
}

//----------------------------------------------------------------------------
void qMRMLCameraIntrinsicsWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qMRMLCameraIntrinsicsWidget);

  this->Superclass::setMRMLScene(scene);
  if (scene == NULL)
  {
    return;
  }

  d->comboBox_CameraSelector->setMRMLScene(scene);
}

//----------------------------------------------------------------------------
void qMRMLCameraIntrinsicsWidget::OnNodeIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLCameraIntrinsicsWidget);

  vtkMRMLWebcamNode* camNode = vtkMRMLWebcamNode::SafeDownCast(caller);

  if (camNode != nullptr && camNode->GetIntrinsicMatrix() != nullptr)
  {
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        d->tableWidget_CameraMatrix->itemAt(i, j)->setText(QString::number(camNode->GetIntrinsicMatrix()->GetElement(i, j)));
      }
    }
  }
}

//----------------------------------------------------------------------------
void qMRMLCameraIntrinsicsWidget::OnNodeDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLCameraIntrinsicsWidget);

  vtkMRMLWebcamNode* camNode = vtkMRMLWebcamNode::SafeDownCast(caller);

  if (camNode != nullptr && camNode->GetDistortionCoefficients() != nullptr)
  {
    d->tableWidget_DistCoeffs->clear();
    d->tableWidget_DistCoeffs->insertRow(0);

    for (int i = 0; i < camNode->GetDistortionCoefficients()->GetNumberOfValues(); ++i)
    {
      d->tableWidget_DistCoeffs->insertColumn(d->tableWidget_DistCoeffs->columnCount());
      d->tableWidget_DistCoeffs->setItem(0, i, new QTableWidgetItem(QString::number(camNode->GetDistortionCoefficients()->GetValue(i))));
    }
  }
}

//----------------------------------------------------------------------------
void qMRMLCameraIntrinsicsWidget::SetCurrentNode(vtkMRMLWebcamNode* newNode)
{
  if (this->CurrentNode != nullptr && this->CurrentNode != newNode)
  {
    this->CurrentNode->RemoveAllObservers();
  }

  this->CurrentNode = newNode;

  if (this->CurrentNode != nullptr)
  {
    this->CurrentNode->AddObserver(vtkMRMLWebcamNode::IntrinsicsModifiedEvent, this, &qMRMLCameraIntrinsicsWidget::OnNodeIntrinsicsModified);
    this->CurrentNode->AddObserver(vtkMRMLWebcamNode::DistortionCoefficientsModifiedEvent, this, &qMRMLCameraIntrinsicsWidget::OnNodeDistortionCoefficientsModified);
  }
}

//----------------------------------------------------------------------------
vtkMRMLWebcamNode* qMRMLCameraIntrinsicsWidget::GetCurrentNode() const
{
  return this->CurrentNode;
}
