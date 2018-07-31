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
#include <QAction>
#include <QClipboard>
#include <QDebug>

// Local includes
#include "qMRMLWebcamIntrinsicsWidget.h"
#include "vtkSlicerWebcamsLogic.h"
#include "ui_qMRMLWebcamIntrinsicsWidget.h"

// MRML includes
#include <vtkMRMLScene.h>
#include <vtkMRMLNode.h>

// VTK includes
#include <vtksys/RegularExpression.hxx>

namespace
{
  //----------------------------------------------------------------------------
  std::string ToString(const vtkMatrix3x3* mat, const std::string delimiter = " ", const std::string rowDelimiter = "")
  {
    if (!mat)
    {
      return "";
    }

    std::stringstream ss;
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        ss << mat->GetElement(i, j);
        ss << delimiter;
      }
      ss << rowDelimiter;
    }

    return ss.str();
  }

  //----------------------------------------------------------------------------
  std::string ToString(const vtkMatrix4x4* mat, const std::string delimiter = " ", const std::string rowDelimiter = "")
  {
    if (!mat)
    {
      return "";
    }

    std::stringstream ss;
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        ss << mat->GetElement(i, j);
        ss << delimiter;
      }
      ss << rowDelimiter;
    }

    return ss.str();
  }

  //----------------------------------------------------------------------------
  bool FromString(vtkMatrix3x3* mat, const std::string& str, std::string& remainString, const std::string delimiterExp = "(\\ |\\,|\\:|\\;|\t|\n|\\[|\\])")
  {
    if (!mat)
    {
      return false;
    }

    // Parse the string using the regular expression
    vtksys::RegularExpression delimiterRegex(delimiterExp);

    // Convert each string token into a double and put into vector
    char* end;
    remainString = str;
    std::vector<double> elements;
    int count = 0;
    while (!remainString.empty())
    {
      bool separatorFound = delimiterRegex.find(remainString);
      std::string::size_type tokenStartIndex = remainString.length();
      std::string::size_type tokenEndIndex = remainString.length();
      if (separatorFound)
      {
        tokenStartIndex = delimiterRegex.start(0);
        tokenEndIndex = delimiterRegex.end(0);
      }

      std::string valString = remainString.substr(0, tokenStartIndex);
      remainString = remainString.substr(tokenEndIndex);
      if (valString.empty()) // Handle back-to-back delimiters
      {
        continue;
      }

      // strtod is much faster (about 2x on some computers) than string stream
      // based string->number conversion
      double val = std::strtod(valString.c_str(), &end);
      if (*end != 0) // Parsing failed due to non-numeric character
      {
        return false;
      }

      elements.push_back(val);
      count++;
      if (count == 9)
      {
        break;
      }
    }

    if (elements.size() < 9)
    {
      return false;
    }

    // Put into matrix
    int linearIndex = 0;
    for (int row = 0; row < 3; row++)
    {
      for (int col = 0; col < 3; col++)
      {
        mat->SetElement(row, col, elements.at(linearIndex));
        linearIndex++;
      }
    }

    return true;
  }

  //----------------------------------------------------------------------------
  bool FromString(vtkMatrix4x4* mat, const std::string& str, std::string& remainString, const std::string delimiterExp = "(\\ |\\,|\\:|\\;|\t|\n|\\[|\\])")
  {
    if (!mat)
    {
      return false;
    }

    // Parse the string using the regular expression
    vtksys::RegularExpression delimiterRegex(delimiterExp);

    // Convert each string token into a double and put into vector
    char* end;
    remainString = str;
    std::vector<double> elements;
    int count = 0;
    while (!remainString.empty())
    {
      bool separatorFound = delimiterRegex.find(remainString);
      std::string::size_type tokenStartIndex = remainString.length();
      std::string::size_type tokenEndIndex = remainString.length();
      if (separatorFound)
      {
        tokenStartIndex = delimiterRegex.start(0);
        tokenEndIndex = delimiterRegex.end(0);
      }

      std::string valString = remainString.substr(0, tokenStartIndex);
      remainString = remainString.substr(tokenEndIndex);
      if (valString.empty()) // Handle back-to-back delimiters
      {
        continue;
      }

      // strtod is much faster (about 2x on some computers) than string stream
      // based string->number conversion
      double val = std::strtod(valString.c_str(), &end);
      if (*end != 0) // Parsing failed due to non-numeric character
      {
        return false;
      }

      elements.push_back(val);
      count++;
      if (count == 16)
      {
        break;
      }
    }

    if (elements.size() < 16)
    {
      return false;
    }

    // Put into matrix
    int linearIndex = 0;
    for (int row = 0; row < 4; row++)
    {
      for (int col = 0; col < 4; col++)
      {
        mat->SetElement(row, col, elements.at(linearIndex));
        linearIndex++;
      }
    }

    return true;
  }

  //----------------------------------------------------------------------------
  bool FromString(vtkDoubleArray* arr, const std::string& str, const std::string delimiterExp = "(\\ |\\,|\\:|\\;|\t|\n|\\[|\\])")
  {
    if (!arr)
    {
      return false;
    }

    // Parse the string using the regular expression
    vtksys::RegularExpression delimiterRegex(delimiterExp);

    // Convert each string token into a double and put into vector
    char* end;
    std::string remainString = str;
    while (!remainString.empty())
    {
      bool separatorFound = delimiterRegex.find(remainString);
      std::string::size_type tokenStartIndex = remainString.length();
      std::string::size_type tokenEndIndex = remainString.length();
      if (separatorFound)
      {
        tokenStartIndex = delimiterRegex.start(0);
        tokenEndIndex = delimiterRegex.end(0);
      }

      std::string valString = remainString.substr(0, tokenStartIndex);
      remainString = remainString.substr(tokenEndIndex);
      if (valString.empty()) // Handle back-to-back delimiters
      {
        continue;
      }

      // strtod is much faster (about 2x on some computers) than string stream
      // based string->number conversion
      double val = std::strtod(valString.c_str(), &end);
      if (*end != 0) // Parsing failed due to non-numeric character
      {
        return false;
      }

      arr->InsertNextValue(val);
    }

    return true;
  }
}

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Webcam
class qMRMLWebcamIntrinsicsWidgetPrivate : public Ui_qMRMLWebcamIntrinsicsWidget
{
  Q_DECLARE_PUBLIC(qMRMLWebcamIntrinsicsWidget);

protected:
  qMRMLWebcamIntrinsicsWidget* const q_ptr;

public:
  qMRMLWebcamIntrinsicsWidgetPrivate(qMRMLWebcamIntrinsicsWidget& object);

  QAction*               CopyAction;
  QAction*               PasteAction;

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
    d->collapsibleButton_Details->setEnabled(true);

    this->OnNodeIntrinsicsModified(newNode, vtkMRMLWebcamNode::IntrinsicsModifiedEvent, nullptr);
    this->OnNodeDistortionCoefficientsModified(newNode, vtkMRMLWebcamNode::DistortionCoefficientsModifiedEvent, nullptr);
    this->OnNodeMarkerTransformModified(newNode, vtkMRMLWebcamNode::MarkerToSensorTransformModifiedEvent, nullptr);
  }
  else
  {
    d->collapsibleButton_Details->setEnabled(false);
  }
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::onIntrinsicItemChanged(QTableWidgetItem* item)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  bool ok;
  double value = item->text().toDouble(&ok);
  if (ok)
  {
    if (this->CurrentNode->GetIntrinsicMatrix() == nullptr)
    {
      vtkSmartPointer<vtkMatrix3x3> mat = vtkSmartPointer<vtkMatrix3x3>::New();
      mat->Zero();
      this->CurrentNode->SetAndObserveIntrinsicMatrix(mat);
    }

    this->CurrentNode->RemoveObserver(this->IntrinsicObserverTag);
    this->CurrentNode->GetIntrinsicMatrix()->SetElement(item->row(), item->column(), value);
    this->IntrinsicObserverTag = this->CurrentNode->AddObserver(vtkMRMLWebcamNode::IntrinsicsModifiedEvent, this, &qMRMLWebcamIntrinsicsWidget::OnNodeIntrinsicsModified);
  }
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::onDistortionItemChanged(QTableWidgetItem* item)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  bool ok;
  double value = item->text().toDouble(&ok);

  if (ok)
  {
    if (this->CurrentNode->GetDistortionCoefficients() == nullptr)
    {
      vtkSmartPointer<vtkDoubleArray> arr = vtkSmartPointer<vtkDoubleArray>::New();
      arr->SetNumberOfValues(d->tableWidget_DistCoeffs->columnCount());
      arr->Fill(0);
      this->CurrentNode->SetAndObserveDistortionCoefficients(arr);
    }

    this->CurrentNode->RemoveObserver(this->DistortionObserverTag);
    this->CurrentNode->GetDistortionCoefficients()->SetValue(item->column(), value);
    this->DistortionObserverTag = this->CurrentNode->AddObserver(vtkMRMLWebcamNode::DistortionCoefficientsModifiedEvent, this, &qMRMLWebcamIntrinsicsWidget::OnNodeDistortionCoefficientsModified);
  }
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::onMarkerTransformItemChanged(QTableWidgetItem* item)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  bool ok;
  double value = item->text().toDouble(&ok);

  if (ok)
  {
    if (this->CurrentNode->GetMarkerToImageSensorTransform() == nullptr)
    {
      vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
      mat->Identity();
      this->CurrentNode->SetAndObserveMarkerToImageSensorTransform(mat);
    }

    this->CurrentNode->RemoveObserver(this->MarkerTransformObserverTag);
    this->CurrentNode->GetMarkerToImageSensorTransform()->SetElement(item->row(), item->column(), value);
    this->MarkerTransformObserverTag = this->CurrentNode->AddObserver(vtkMRMLWebcamNode::MarkerToSensorTransformModifiedEvent, this, &qMRMLWebcamIntrinsicsWidget::OnNodeMarkerTransformModified);
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

  connect(d->tableWidget_CameraMatrix, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onIntrinsicItemChanged(QTableWidgetItem*)));
  connect(d->tableWidget_DistCoeffs, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onDistortionItemChanged(QTableWidgetItem*)));
  connect(d->tableWidget_MarkerToImageSensor, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(onMarkerTransformItemChanged(QTableWidgetItem*)));

  d->CopyAction = new QAction(this);
  d->CopyAction->setIcon(QIcon(":Icons/Medium/SlicerEditCopy.png"));
  d->CopyAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  d->CopyAction->setShortcuts(QKeySequence::Copy);
  d->CopyAction->setToolTip(tr("Copy"));
  this->addAction(d->CopyAction);
  d->PasteAction = new QAction(this);
  d->PasteAction->setIcon(QIcon(":Icons/Medium/SlicerEditPaste.png"));
  d->PasteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  d->PasteAction->setShortcuts(QKeySequence::Paste);
  d->PasteAction->setToolTip(tr("Paste"));
  this->addAction(d->PasteAction);

  this->connect(d->CopyAction, SIGNAL(triggered()), SLOT(copyData()));
  this->connect(d->PasteAction, SIGNAL(triggered()), SLOT(pasteData()));

  d->tableWidget_CameraMatrix->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  d->tableWidget_CameraMatrix->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  d->tableWidget_DistCoeffs->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  d->tableWidget_DistCoeffs->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  d->tableWidget_MarkerToImageSensor->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  d->tableWidget_MarkerToImageSensor->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  this->Superclass::setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::copyData()
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  std::stringstream ss;
  vtkMatrix3x3* internalMatrix = this->CurrentNode->GetIntrinsicMatrix();
  std::string delimiter = " ";
  std::string rowDelimiter = "\n";
  ss << ToString(internalMatrix, delimiter, rowDelimiter) << " ";
  ss << std::endl;

  vtkMatrix4x4* markerToSensor = this->CurrentNode->GetMarkerToImageSensorTransform();
  ss << ToString(markerToSensor, delimiter, rowDelimiter) << " ";
  ss << std::endl;

  for (int i = 0; i < this->CurrentNode->GetDistortionCoefficients()->GetNumberOfValues(); ++i)
  {
    ss << this->CurrentNode->GetDistortionCoefficients()->GetValue(i) << " ";
  }
  ss << std::endl;

  QApplication::clipboard()->setText(QString::fromStdString(ss.str()));
}

//-----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::pasteData()
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  // Intrinsics
  vtkNew<vtkMatrix3x3> tempMatrix;
  std::string text = QApplication::clipboard()->text().toStdString();
  std::string remaining;
  bool success = FromString(tempMatrix.GetPointer(), text, remaining);
  if (!success)
  {
    qWarning() << "Cannot convert pasted string to matrix.";
    return;
  }
  this->CurrentNode->SetAndObserveIntrinsicMatrix(tempMatrix);
  this->CurrentNode->InvokeEvent(vtkMRMLWebcamNode::IntrinsicsModifiedEvent);

  // Marker to sensor calibration
  vtkNew<vtkMatrix4x4> tmpMatrix;
  text = QApplication::clipboard()->text().toStdString();
  success = FromString(tmpMatrix.GetPointer(), remaining, remaining);
  if (!success)
  {
    qWarning() << "Cannot convert pasted string to matrix.";
    return;
  }
  this->CurrentNode->SetAndObserveMarkerToImageSensorTransform(tmpMatrix);
  this->CurrentNode->InvokeEvent(vtkMRMLWebcamNode::MarkerToSensorTransformModifiedEvent);

  // Distortion Coefficients
  vtkNew<vtkDoubleArray> tempArray;
  success = FromString(tempArray.GetPointer(), remaining);
  if (!success)
  {
    qWarning() << "Cannot convert remaining values to distortion coefficients.";
    return;
  }
  this->CurrentNode->SetAndObserveDistortionCoefficients(tempArray);
  this->CurrentNode->InvokeEvent(vtkMRMLWebcamNode::DistortionCoefficientsModifiedEvent);
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::OnNodeIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  vtkMRMLWebcamNode* camNode = vtkMRMLWebcamNode::SafeDownCast(caller);

  if (camNode != nullptr && camNode->GetIntrinsicMatrix() != nullptr)
  {
    bool oldState = d->tableWidget_CameraMatrix->blockSignals(true);
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        d->tableWidget_CameraMatrix->setItem(i, j, new QTableWidgetItem(QString::number(camNode->GetIntrinsicMatrix()->GetElement(i, j))));
      }
    }
    d->tableWidget_CameraMatrix->blockSignals(oldState);
  }
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::OnNodeDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  vtkMRMLWebcamNode* camNode = vtkMRMLWebcamNode::SafeDownCast(caller);

  if (camNode != nullptr && camNode->GetDistortionCoefficients() != nullptr)
  {
    bool oldState = d->tableWidget_DistCoeffs->blockSignals(true);

    d->tableWidget_DistCoeffs->clear();
    d->tableWidget_DistCoeffs->setRowCount(0);
    d->tableWidget_DistCoeffs->setColumnCount(0);
    d->tableWidget_DistCoeffs->insertRow(0);

    for (int i = 0; i < camNode->GetDistortionCoefficients()->GetNumberOfValues(); ++i)
    {
      d->tableWidget_DistCoeffs->insertColumn(d->tableWidget_DistCoeffs->columnCount());
      d->tableWidget_DistCoeffs->setItem(0, i, new QTableWidgetItem(QString::number(camNode->GetDistortionCoefficients()->GetValue(i))));
    }
    d->tableWidget_DistCoeffs->blockSignals(oldState);
  }
}

//----------------------------------------------------------------------------
void qMRMLWebcamIntrinsicsWidget::OnNodeMarkerTransformModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLWebcamIntrinsicsWidget);

  vtkMRMLWebcamNode* camNode = vtkMRMLWebcamNode::SafeDownCast(caller);

  if (camNode != nullptr && camNode->GetMarkerToImageSensorTransform() != nullptr)
  {
    bool oldState = d->tableWidget_MarkerToImageSensor->blockSignals(true);
    for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 4; ++j)
      {
        d->tableWidget_MarkerToImageSensor->setItem(i, j, new QTableWidgetItem(QString::number(camNode->GetMarkerToImageSensorTransform()->GetElement(i, j))));
      }
    }
    d->tableWidget_MarkerToImageSensor->blockSignals(oldState);
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
    this->IntrinsicObserverTag = this->CurrentNode->AddObserver(vtkMRMLWebcamNode::IntrinsicsModifiedEvent, this, &qMRMLWebcamIntrinsicsWidget::OnNodeIntrinsicsModified);
    this->DistortionObserverTag = this->CurrentNode->AddObserver(vtkMRMLWebcamNode::DistortionCoefficientsModifiedEvent, this, &qMRMLWebcamIntrinsicsWidget::OnNodeDistortionCoefficientsModified);
    this->MarkerTransformObserverTag = this->CurrentNode->AddObserver(vtkMRMLWebcamNode::MarkerToSensorTransformModifiedEvent, this, &qMRMLWebcamIntrinsicsWidget::OnNodeMarkerTransformModified);
  }
}

//----------------------------------------------------------------------------
vtkMRMLWebcamNode* qMRMLWebcamIntrinsicsWidget::GetCurrentNode() const
{
  return this->CurrentNode;
}
