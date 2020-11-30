/*=auto=========================================================================

Portions (c) Copyright 2018 Robarts Research Institute. All Rights Reserved.

See COPYRIGHT.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: qMRMLVideoCameraIntrinsicsWidget.h,v $
Date:      $Date: 2018/6/16 10:54:09 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/

// Qt includes
#include <QAction>
#include <QClipboard>
#include <QDebug>
#include <QRadioButton>

// Local includes
#include "qMRMLVideoCameraIntrinsicsWidget.h"
#include "vtkSlicerVideoCamerasLogic.h"
#include "ui_qMRMLVideoCameraIntrinsicsWidget.h"

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
  bool FromString(vtkDoubleArray* arr, const std::string& str, std::string& remainString, const int count, const std::string delimiterExp = "(\\ |\\,|\\:|\\;|\t|\n|\\[|\\])")
  {
    if (!arr)
    {
      return false;
    }

    // Parse the string using the regular expression
    vtksys::RegularExpression delimiterRegex(delimiterExp);

    int _count = 0;
    // Convert each string token into a double and put into vector
    char* end;
    remainString = str;
    while (!remainString.empty() &&
           (_count < count || count == -1))
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
      _count++;
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
/// \ingroup Slicer_QtModules_VideoCamera
class qMRMLVideoCameraIntrinsicsWidgetPrivate : public Ui_qMRMLVideoCameraIntrinsicsWidget
{
  Q_DECLARE_PUBLIC(qMRMLVideoCameraIntrinsicsWidget);

protected:
  qMRMLVideoCameraIntrinsicsWidget* const q_ptr;

public:
  qMRMLVideoCameraIntrinsicsWidgetPrivate(qMRMLVideoCameraIntrinsicsWidget& object);

  QAction*               CopyAction;
  QAction*               PasteAction;

  vtkSlicerVideoCamerasLogic* logic();
};

//-----------------------------------------------------------------------------
// qMRMLVideoCameraIntrinsicsWidgetPrivate methods

//-----------------------------------------------------------------------------
qMRMLVideoCameraIntrinsicsWidgetPrivate::qMRMLVideoCameraIntrinsicsWidgetPrivate(qMRMLVideoCameraIntrinsicsWidget& object)
  : q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
vtkSlicerVideoCamerasLogic* qMRMLVideoCameraIntrinsicsWidgetPrivate::logic()
{
  Q_Q(qMRMLVideoCameraIntrinsicsWidget);
  return vtkSlicerVideoCamerasLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
// qMRMLVideoCameraIntrinsicsWidget methods

//------------------------------------------------------------------------------
qMRMLVideoCameraIntrinsicsWidget::qMRMLVideoCameraIntrinsicsWidget(QWidget* vparent)
  : qSlicerAbstractModuleWidget(vparent)
  , d_ptr(new qMRMLVideoCameraIntrinsicsWidgetPrivate(*this))
  , CurrentNode(nullptr)
{
  this->setup();
}

//------------------------------------------------------------------------------
qMRMLVideoCameraIntrinsicsWidget::~qMRMLVideoCameraIntrinsicsWidget()
{
  this->SetCurrentNode(nullptr);
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::onVideoCameraSelectorChanged(vtkMRMLNode* newNode)
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  vtkMRMLVideoCameraNode* camNode = vtkMRMLVideoCameraNode::SafeDownCast(newNode);

  this->SetCurrentNode(camNode);

  if (this->CurrentNode != nullptr)
  {
    d->collapsibleButton_Details->setEnabled(true);

    this->OnNodeIntrinsicsModified(newNode, vtkMRMLVideoCameraNode::IntrinsicsModifiedEvent, nullptr);
    this->OnNodeDistortionCoefficientsModified(newNode, vtkMRMLVideoCameraNode::DistortionCoefficientsModifiedEvent, nullptr);
    this->OnNodeMarkerTransformModified(newNode, vtkMRMLVideoCameraNode::MarkerToSensorTransformModifiedEvent, nullptr);
    this->OnCameraPlaneOffsetModified(newNode, vtkMRMLVideoCameraNode::CameraPlaneOffsetModifiedEvent, nullptr);
  }
  else
  {
    d->collapsibleButton_Details->setEnabled(false);
  }
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::onIntrinsicMatrixChanged()
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  if (this->CurrentNode->GetIntrinsicMatrix() == nullptr)
  {
    vtkSmartPointer<vtkMatrix3x3> mat = vtkSmartPointer<vtkMatrix3x3>::New();
    mat->Zero();
    this->CurrentNode->SetAndObserveIntrinsicMatrix(mat);
  }

  this->CurrentNode->RemoveObserver(this->IntrinsicObserverTag);
  QVector<double> vals = d->MatrixWidget_CameraMatrix->values();
  for (int i = 0; i < d->MatrixWidget_CameraMatrix->rowCount(); i++)
  {
    for (int j = 0; j < d->MatrixWidget_CameraMatrix->columnCount(); j++)
    {
      this->CurrentNode->GetIntrinsicMatrix()->SetElement(i, j, d->MatrixWidget_CameraMatrix->value(i, j));
    }
  }
  this->IntrinsicObserverTag = this->CurrentNode->AddObserver(vtkMRMLVideoCameraNode::IntrinsicsModifiedEvent, this, &qMRMLVideoCameraIntrinsicsWidget::OnNodeIntrinsicsModified);
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::onDistortionMatrixChanged()
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  if (this->CurrentNode->GetNumberOfDistortionCoefficients() != d->MatrixWidget_DistCoeffs->columnCount())
  {
    this->CurrentNode->SetNumberOfDistortionCoefficients(d->MatrixWidget_DistCoeffs->columnCount());
  }

  this->CurrentNode->RemoveObserver(this->DistortionObserverTag);
  for (int i = 0; i < d->MatrixWidget_DistCoeffs->columnCount(); ++i)
  {
    this->CurrentNode->SetDistortionCoefficientValue(i, d->MatrixWidget_DistCoeffs->value(0, i));
  }
  this->DistortionObserverTag = this->CurrentNode->AddObserver(vtkMRMLVideoCameraNode::DistortionCoefficientsModifiedEvent, this, &qMRMLVideoCameraIntrinsicsWidget::OnNodeDistortionCoefficientsModified);
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::onMarkerTransformMatrixChanged()
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  if (this->CurrentNode->GetMarkerToImageSensorTransform() == nullptr)
  {
    vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
    mat->Identity();
    this->CurrentNode->SetAndObserveMarkerToImageSensorTransform(mat);
  }

  this->CurrentNode->RemoveObserver(this->MarkerTransformObserverTag);
  QVector<double> vals = d->MatrixWidget_MarkerToImageSensor->values();
  for (int i = 0; i < d->MatrixWidget_MarkerToImageSensor->rowCount(); i++)
  {
    for (int j = 0; j < d->MatrixWidget_MarkerToImageSensor->columnCount(); j++)
    {
      this->CurrentNode->GetMarkerToImageSensorTransform()->SetElement(i, j, d->MatrixWidget_MarkerToImageSensor->value(i, j));
    }
  }
  this->MarkerTransformObserverTag = this->CurrentNode->AddObserver(vtkMRMLVideoCameraNode::MarkerToSensorTransformModifiedEvent, this, &qMRMLVideoCameraIntrinsicsWidget::OnNodeMarkerTransformModified);
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::onCameraPlaneOffsetMatrixChanged()
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  this->CurrentNode->RemoveObserver(this->CameraPlaneOffsetObserverTag);
  for (int i = 0; i < 3; ++i)
  {
    this->CurrentNode->SetCameraPlaneOffsetValue(i, d->MatrixWidget_CameraPlaneOffset->value(0, i));
  }
  this->CameraPlaneOffsetObserverTag = this->CurrentNode->AddObserver(vtkMRMLVideoCameraNode::CameraPlaneOffsetModifiedEvent, this, &qMRMLVideoCameraIntrinsicsWidget::OnCameraPlaneOffsetModified);
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::setup()
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);
  d->setupUi(this);
  this->Superclass::setup();

  // --------------------------------------------------
  // Connectors section
  connect(d->comboBox_CameraSelector, SIGNAL(currentNodeChanged(vtkMRMLNode*)), this, SLOT(onVideoCameraSelectorChanged(vtkMRMLNode*)));
  connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->comboBox_CameraSelector, SLOT(setMRMLScene(vtkMRMLScene*)));

  connect(d->MatrixWidget_CameraMatrix, SIGNAL(matrixChanged()), this, SLOT(onIntrinsicMatrixChanged()));
  connect(d->MatrixWidget_DistCoeffs, SIGNAL(matrixChanged()), this, SLOT(onDistortionMatrixChanged()));
  connect(d->MatrixWidget_MarkerToImageSensor, SIGNAL(matrixChanged()), this, SLOT(onMarkerTransformMatrixChanged()));
  connect(d->MatrixWidget_CameraPlaneOffset, SIGNAL(matrixChanged()), this, SLOT(onCameraPlaneOffsetMatrixChanged()));

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
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  this->Superclass::setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::copyData()
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  std::stringstream ss;
  vtkMatrix3x3* internalMatrix = this->CurrentNode->GetIntrinsicMatrix();
  std::string delimiter = " ";
  std::string rowDelimiter = "\n";
  ss << ToString(internalMatrix, delimiter, rowDelimiter) << " ";
  ss << std::endl;

  vtkMatrix4x4* markerToSensor = this->CurrentNode->GetMarkerToImageSensorTransform();
  ss << ToString(markerToSensor, delimiter, rowDelimiter) << " ";
  ss << std::endl;

  for (int i = 0; i < 3; ++i)
  {
    ss << this->CurrentNode->GetCameraPlaneOffsetValue(i) << " ";
  }
  ss << std::endl;

  for (int i = 0; i < this->CurrentNode->GetNumberOfDistortionCoefficients(); ++i)
  {
    ss << this->CurrentNode->GetDistortionCoefficientValue(i) << " ";
  }
  ss << std::endl;

  QApplication::clipboard()->setText(QString::fromStdString(ss.str()));
}

//-----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::pasteData()
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

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

  // Camera Plane Offset
  vtkNew<vtkDoubleArray> tempArray;
  success = FromString(tempArray.GetPointer(), remaining, remaining, 3);
  if (!success)
  {
    qWarning() << "Cannot convert values to camera plane offset.";
    return;
  }
  for (int i = 0; i < 3; ++i)
  {
    this->CurrentNode->SetCameraPlaneOffsetValue(i, tempArray->GetValue(i));
  }


  // Distortion Coefficients
  tempArray->Reset();
  success = FromString(tempArray.GetPointer(), remaining);
  if (!success)
  {
    qWarning() << "Cannot convert remaining values to distortion coefficients.";
    return;
  }
  this->CurrentNode->SetNumberOfDistortionCoefficients(tempArray->GetNumberOfValues());
  for (vtkIdType i = 0; i < tempArray->GetNumberOfValues(); ++i)
  {
    this->CurrentNode->SetDistortionCoefficientValue(i, tempArray->GetValue(i));
  }
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::OnNodeIntrinsicsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  vtkMRMLVideoCameraNode* camNode = vtkMRMLVideoCameraNode::SafeDownCast(caller);

  if (camNode != nullptr && camNode->GetIntrinsicMatrix() != nullptr)
  {
    bool oldState = d->MatrixWidget_CameraMatrix->blockSignals(true);
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        d->MatrixWidget_CameraMatrix->setValue(i, j, camNode->GetIntrinsicMatrix()->GetElement(i, j));
      }
    }
    d->MatrixWidget_CameraMatrix->blockSignals(oldState);
  }
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::OnNodeDistortionCoefficientsModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  vtkMRMLVideoCameraNode* camNode = vtkMRMLVideoCameraNode::SafeDownCast(caller);

  if (camNode != nullptr)
  {
    bool oldState = d->MatrixWidget_DistCoeffs->blockSignals(true);

    d->MatrixWidget_DistCoeffs->setRowCount(1);
    d->MatrixWidget_DistCoeffs->setColumnCount(camNode->GetNumberOfDistortionCoefficients());

    for (int i = 0; i < camNode->GetNumberOfDistortionCoefficients(); ++i)
    {
      d->MatrixWidget_DistCoeffs->setValue(0, i, camNode->GetDistortionCoefficientValue(i));
    }
    d->MatrixWidget_DistCoeffs->blockSignals(oldState);
  }
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::OnNodeMarkerTransformModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  vtkMRMLVideoCameraNode* camNode = vtkMRMLVideoCameraNode::SafeDownCast(caller);

  if (camNode != nullptr && camNode->GetMarkerToImageSensorTransform() != nullptr)
  {
    bool oldState = d->MatrixWidget_MarkerToImageSensor->blockSignals(true);
    for (int i = 0; i < 3; ++i)
    {
      for (int j = 0; j < 3; ++j)
      {
        d->MatrixWidget_MarkerToImageSensor->setValue(i, j, camNode->GetMarkerToImageSensorTransform()->GetElement(i, j));
      }
    }
    d->MatrixWidget_MarkerToImageSensor->blockSignals(oldState);
  }
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::OnCameraPlaneOffsetModified(vtkObject* caller, unsigned long event, void* data)
{
  Q_D(qMRMLVideoCameraIntrinsicsWidget);

  vtkMRMLVideoCameraNode* camNode = vtkMRMLVideoCameraNode::SafeDownCast(caller);

  if (camNode != nullptr)
  {
    bool oldState = d->MatrixWidget_CameraPlaneOffset->blockSignals(true);
    for (int i = 0; i < 3; ++i)
    {
      d->MatrixWidget_CameraPlaneOffset->setValue(0, i, camNode->GetCameraPlaneOffsetValue(i));
    }
    d->MatrixWidget_CameraPlaneOffset->blockSignals(oldState);
  }
}

//----------------------------------------------------------------------------
void qMRMLVideoCameraIntrinsicsWidget::SetCurrentNode(vtkMRMLVideoCameraNode* newNode)
{
  if (this->CurrentNode != nullptr && this->CurrentNode != newNode)
  {
    this->CurrentNode->RemoveAllObservers();
  }

  this->CurrentNode = newNode;

  if (this->CurrentNode != nullptr)
  {
    this->IntrinsicObserverTag = this->CurrentNode->AddObserver(vtkMRMLVideoCameraNode::IntrinsicsModifiedEvent, this, &qMRMLVideoCameraIntrinsicsWidget::OnNodeIntrinsicsModified);
    this->DistortionObserverTag = this->CurrentNode->AddObserver(vtkMRMLVideoCameraNode::DistortionCoefficientsModifiedEvent, this, &qMRMLVideoCameraIntrinsicsWidget::OnNodeDistortionCoefficientsModified);
    this->MarkerTransformObserverTag = this->CurrentNode->AddObserver(vtkMRMLVideoCameraNode::MarkerToSensorTransformModifiedEvent, this, &qMRMLVideoCameraIntrinsicsWidget::OnNodeMarkerTransformModified);
    this->CameraPlaneOffsetObserverTag = this->CurrentNode->AddObserver(vtkMRMLVideoCameraNode::CameraPlaneOffsetModifiedEvent, this, &qMRMLVideoCameraIntrinsicsWidget::OnCameraPlaneOffsetModified);
  }
}

//----------------------------------------------------------------------------
vtkMRMLVideoCameraNode* qMRMLVideoCameraIntrinsicsWidget::GetCurrentNode() const
{
  return this->CurrentNode;
}
