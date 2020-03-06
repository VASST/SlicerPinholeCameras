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
#include "qSlicerVideoCamerasModuleWidget.h"
#include "ui_qSlicerVideoCamerasModuleWidget.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVideoCamerasModuleWidgetPrivate: public Ui_qSlicerVideoCamerasModuleWidget
{
public:
  qSlicerVideoCamerasModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVideoCamerasModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerVideoCamerasModuleWidgetPrivate::qSlicerVideoCamerasModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVideoCamerasModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerVideoCamerasModuleWidget::qSlicerVideoCamerasModuleWidget(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVideoCamerasModuleWidgetPrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerVideoCamerasModuleWidget::~qSlicerVideoCamerasModuleWidget()
{
}

//----------------------------------------------------------------------------
void qSlicerVideoCamerasModuleWidget::setMRMLScene(vtkMRMLScene* scene)
{
  Q_D(qSlicerVideoCamerasModuleWidget);

  this->Superclass::setMRMLScene(scene);
}

//-----------------------------------------------------------------------------
void qSlicerVideoCamerasModuleWidget::setup()
{
  Q_D(qSlicerVideoCamerasModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  connect(this, SIGNAL(mrmlSceneChanged(vtkMRMLScene*)), d->videoCameraIntrinsicsWidget, SLOT(setMRMLScene(vtkMRMLScene*)));
}
