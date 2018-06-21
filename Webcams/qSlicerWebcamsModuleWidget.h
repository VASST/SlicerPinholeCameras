/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __qSlicerWebcamsModuleWidget_h
#define __qSlicerWebcamsModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerWebcamsModuleExport.h"

class qSlicerWebcamsModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_WEBCAMS_EXPORT qSlicerWebcamsModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:
  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerWebcamsModuleWidget(QWidget* parent = 0);
  virtual ~qSlicerWebcamsModuleWidget();

public slots:
  virtual void setMRMLScene(vtkMRMLScene* scene);

protected:
  QScopedPointer<qSlicerWebcamsModuleWidgetPrivate> d_ptr;

  virtual void setup();

private:
  Q_DECLARE_PRIVATE(qSlicerWebcamsModuleWidget);
  Q_DISABLE_COPY(qSlicerWebcamsModuleWidget);
};

#endif
