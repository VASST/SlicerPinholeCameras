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

// VideoCameras Logic includes
#include <vtkSlicerVideoCamerasLogic.h>

// Slicer includes
#include <qSlicerApplication.h>
#include <qSlicerIOManager.h>
#include <qSlicerModuleManager.h>
#include <qSlicerNodeWriter.h>

// Slicer logic includes
#include <vtkSlicerApplicationLogic.h>

// VideoCameras includes
#include "qSlicerVideoCamerasModule.h"
#include "qSlicerVideoCamerasModuleWidget.h"
#include "qSlicerVideoCamerasReaderPlugin.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  #include <QtPlugin>
  Q_EXPORT_PLUGIN2(qSlicerVideoCamerasModule, qSlicerVideoCamerasModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerVideoCamerasModulePrivate
{
public:
  qSlicerVideoCamerasModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerVideoCamerasModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerVideoCamerasModulePrivate::qSlicerVideoCamerasModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerVideoCamerasModule methods

//-----------------------------------------------------------------------------
qSlicerVideoCamerasModule::qSlicerVideoCamerasModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVideoCamerasModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerVideoCamerasModule::~qSlicerVideoCamerasModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerVideoCamerasModule::helpText() const
{
  return "This module exposes the details and operations that can be performed on a videoCamera.";
}

//-----------------------------------------------------------------------------
QString qSlicerVideoCamerasModule::acknowledgementText() const
{
  return "This module was developed with support from the Natural Sciences and Engineering Research Council of Canada, the Canadian Foundation for Innovation, and the Virtual Augmentation and Simulation for Surgery and Therapy laboratory, Western University.";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVideoCamerasModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Adam Rankin (Robarts Research Institute.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerVideoCamerasModule::icon() const
{
  return QIcon(":/Icons/VideoCameras.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerVideoCamerasModule::categories() const
{
  return QStringList() << "VideoCameras";
}

//-----------------------------------------------------------------------------
QStringList qSlicerVideoCamerasModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerVideoCamerasModule::setup()
{
  this->Superclass::setup();

  vtkSlicerVideoCamerasLogic* videoCamerasLogic = vtkSlicerVideoCamerasLogic::SafeDownCast(this->logic());

  if (qSlicerApplication::application())
  {
    // Register IOs
    qSlicerIOManager* ioManager = qSlicerApplication::application()->ioManager();
    ioManager->registerIO(new qSlicerVideoCamerasReaderPlugin(videoCamerasLogic, this));
    ioManager->registerIO(new qSlicerNodeWriter("VideoCameras", QString("VideoCameraFile"), QStringList() << "vtkMRMLVideoCameraNode", false, this));
  }
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerVideoCamerasModule::createWidgetRepresentation()
{
  return new qSlicerVideoCamerasModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerVideoCamerasModule::createLogic()
{
  return vtkSlicerVideoCamerasLogic::New();
}
