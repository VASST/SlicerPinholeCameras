/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLCameraNode.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// Qt includes

// Local includes
#include "qSlicerCameraIntrinsicsWidget.h"
#include "vtkSlicerCameraLogic.h"
#include "ui_qSlicerCameraIntrinsicsWidget.h"

// MRML includes
#include <vtkMRMLCameraNode.h>

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>

// VTK includes

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Camera
class qSlicerCameraIntrinsicsWidgetPrivate : public Ui_qSlicerCameraIntrinsicsWidget
{
  Q_DECLARE_PUBLIC(qSlicerCameraIntrinsicsWidget);
protected:
  qSlicerCameraIntrinsicsWidget* const q_ptr;
public:
  qSlicerCameraIntrinsicsWidgetPrivate(qSlicerCameraIntrinsicsWidget& object);

  vtkSlicerCameraLogic* logic();
};

//-----------------------------------------------------------------------------
// qSlicerCameraIntrinsicsWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerCameraIntrinsicsWidgetPrivate::qSlicerCameraIntrinsicsWidgetPrivate(qSlicerCameraIntrinsicsWidget& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
vtkSlicerCameraLogic* qSlicerCameraIntrinsicsWidgetPrivate::logic()
{
  Q_Q(qSlicerCameraIntrinsicsWidget);
  return vtkSlicerCameraLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
// qSlicerCameraIntrinsicsWidget methods

//------------------------------------------------------------------------------
qSlicerCameraIntrinsicsWidget::qSlicerCameraIntrinsicsWidget(QWidget* vparent)
  : qSlicerAbstractModuleWidget(vparent)
{

}

//------------------------------------------------------------------------------
qSlicerCameraIntrinsicsWidget::~qSlicerCameraIntrinsicsWidget()
{
}

//----------------------------------------------------------------------------
void qSlicerCameraIntrinsicsWidget::onCameraSelectorChanged(vtkMRMLNode* newNode)
{
  Q_D(qSlicerCameraIntrinsicsWidget);

  vtkMRMLCameraNode* camNode = vtkMRMLCameraNode::SafeDownCast(newNode);

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
void qSlicerCameraIntrinsicsWidget::setup()
{
  Q_D(qSlicerCameraIntrinsicsWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // --------------------------------------------------
  // Connectors section
  connect(d->comboBox_CameraSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onCameraSelectorChanged(vtkMRMLNode*)));
}

//----------------------------------------------------------------------------
void qSlicerCameraIntrinsicsWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerCameraIntrinsicsWidget);

  this->Superclass::setMRMLScene(scene);
  if (scene == NULL)
  {
    return;
  }

  d->comboBox_CameraSelector->setMRMLScene(scene);
}

//----------------------------------------------------------------------------
void qSlicerCameraIntrinsicsWidget::OnNodeIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qSlicerCameraIntrinsicsWidget);

  vtkMRMLCameraNode* camNode = vtkMRMLCameraNode::SafeDownCast(caller);

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
void qSlicerCameraIntrinsicsWidget::OnNodeDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qSlicerCameraIntrinsicsWidget);

  vtkMRMLCameraNode* camNode = vtkMRMLCameraNode::SafeDownCast(caller);

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
void qSlicerCameraIntrinsicsWidget::SetCurrentNode(vtkMRMLCameraNode* newNode)
{
  if (this->CurrentNode != nullptr && this->CurrentNode != newNode)
  {
    this->CurrentNode->RemoveAllObservers();
  }

  this->CurrentNode = newNode;

  if (this->CurrentNode != nullptr)
  {
    this->CurrentNode->AddObserver(vtkMRMLCameraNode::IntrinsicsModifiedEvent, this, &qSlicerCameraIntrinsicsWidget::OnNodeIntrinsicsModified);
    this->CurrentNode->AddObserver(vtkMRMLCameraNode::DistortionCoefficientsModifiedEvent, this, &qSlicerCameraIntrinsicsWidget::OnNodeDistortionCoefficientsModified);
  }
}

//----------------------------------------------------------------------------
vtkMRMLCameraNode* qSlicerCameraIntrinsicsWidget::GetCurrentNode() const
{
  return this->CurrentNode;
}
