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

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerWebcamsModuleWidget.h"
#include "ui_qSlicerWebcamsModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerWebcamsModuleWidgetPrivate: public Ui_qSlicerWebcamsModuleWidget
{
public:
  qSlicerWebcamsModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerWebcamsModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerWebcamsModuleWidgetPrivate::qSlicerWebcamsModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerWebcamsModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerWebcamsModuleWidget::qSlicerWebcamsModuleWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerWebcamsModuleWidgetPrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerWebcamsModuleWidget::~qSlicerWebcamsModuleWidget()
{
}

//----------------------------------------------------------------------------
void qSlicerWebcamsModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerWebcamsModuleWidget);

  this->Superclass::setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qSlicerWebcamsModuleWidget::setup()
{
  Q_D(qSlicerWebcamsModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->webcamIntrinsicsWidget, SLOT(setMRMLScene(vtkMRMLScene*)));
}
