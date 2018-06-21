/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLWebcamIntrinsicsWidget.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// Qt includes

// Local includes
#include "qMRMLWebcamIntrinsicsWidget.h"
#include "vtkSlicerWebcamsLogic.h"
#include "ui_qMRMLWebcamIntrinsicsWidget.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>

// VTK includes

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Webcam
class qMRMLWebcamIntrinsicsWidgetPrivate : public Ui_qMRMLWebcamIntrinsicsWidget
{
  Q_DECLARE_PUBLIC(qMRMLWebcamIntrinsicsWidget);

protected:
  qMRMLWebcamIntrinsicsWidget* const q_ptr;

public:
  qMRMLWebcamIntrinsicsWidgetPrivate(qMRMLWebcamIntrinsicsWidget& object);

  vtkSlicerWebcamsLogic* logic();
};

//-----------------------------------------------------------------------------
// qMRMLWebcamIntrinsicsWidgetPrivate methods

//-----------------------------------------------------------------------------
qMRMLWebcamIntrinsicsWidgetPrivate::qMRMLWebcamIntrinsicsWidgetPrivate(qMRMLWebcamIntrinsicsWidget& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
vtkSlicerWebcamsLogic* qMRMLWebcamIntrinsicsWidgetPrivate::logic()
{
  Q_Q(qMRMLWebcamIntrinsicsWidget);
  return vtkSlicerWebcamsLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
// qMRMLWebcamIntrinsicsWidget methods

//------------------------------------------------------------------------------
qMRMLWebcamIntrinsicsWidget::qMRMLWebcamIntrinsicsWidget(QWidget* vparent)
  : qSlicerAbstractModuleWidget(vparent)
  , d_ptr(new qMRMLWebcamIntrinsicsWidgetPrivate(*this))
  , CurrentNode(nullptr)
{
  this->setup();
}

//------------------------------------------------------------------------------
qMRMLWebcamIntrinsicsWidget::~qMRMLWebcamIntrinsicsWidget()
{
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::onWebcamSelectorChanged(vtkMRMLNode* newNode)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

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
void qMRMLWebcamIntrinsicsWidget::setup()
{
  Q_D(qMRMLWebcamIntrinsicsWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // --------------------------------------------------
  // Connectors section
  connect(d->comboBox_CameraSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onWebcamSelectorChanged(vtkMRMLNode*)));
  connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->comboBox_CameraSelector, SLOT(setMRMLScene(vtkMRMLScene*)));
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  this->Superclass::setMRMLScene(scene);
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::OnNodeIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

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
void qMRMLWebcamIntrinsicsWidget::OnNodeDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

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
void qMRMLWebcamIntrinsicsWidget::SetCurrentNode(vtkMRMLWebcamNode* newNode)
{
  if (this->CurrentNode != nullptr && this->CurrentNode != newNode)
  {
    this->CurrentNode->RemoveAllObservers();
  }

  this->CurrentNode = newNode;

  if (this->CurrentNode != nullptr)
  {
    this->CurrentNode->AddObserver(vtkMRMLWebcamNode::IntrinsicsModifiedEvent, this, &qMRMLWebcamIntrinsicsWidget::OnNodeIntrinsicsModified);
    this->CurrentNode->AddObserver(vtkMRMLWebcamNode::DistortionCoefficientsModifiedEvent, this, &qMRMLWebcamIntrinsicsWidget::OnNodeDistortionCoefficientsModified);
  }
}

//----------------------------------------------------------------------------
vtkMRMLWebcamNode* qMRMLWebcamIntrinsicsWidget::GetCurrentNode() const
{
  return this->CurrentNode;
}
