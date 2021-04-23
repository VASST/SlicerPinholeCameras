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

// PinholeCameras Logic includes
#include <vtkSlicerPinholeCamerasLogic.h>

// Slicer includes
#include <qSlicerApplication.h>
#include <qSlicerIOManager.h>
#include <qSlicerModuleManager.h>
#include <qSlicerNodeWriter.h>

// Slicer logic includes
#include <vtkSlicerApplicationLogic.h>

// PinholeCameras includes
#include "qSlicerPinholeCamerasModule.h"
#include "qSlicerPinholeCamerasModuleWidget.h"
#include "qSlicerPinholeCamerasReaderPlugin.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  #include <QtPlugin>
  Q_EXPORT_PLUGIN2(qSlicerPinholeCamerasModule, qSlicerPinholeCamerasModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerPinholeCamerasModulePrivate
{
public:
  qSlicerPinholeCamerasModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerPinholeCamerasModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerPinholeCamerasModulePrivate::qSlicerPinholeCamerasModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerPinholeCamerasModule methods

//-----------------------------------------------------------------------------
qSlicerPinholeCamerasModule::qSlicerPinholeCamerasModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerPinholeCamerasModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerPinholeCamerasModule::~qSlicerPinholeCamerasModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerPinholeCamerasModule::helpText() const
{
  return "This module exposes the details and operations that can be performed on a videoCamera.";
}

//-----------------------------------------------------------------------------
QString qSlicerPinholeCamerasModule::acknowledgementText() const
{
  return "This module was developed with support from the Natural Sciences and Engineering Research Council of Canada, the Canadian Foundation for Innovation, and the Virtual Augmentation and Simulation for Surgery and Therapy laboratory, Western University.";
}

//-----------------------------------------------------------------------------
QStringList qSlicerPinholeCamerasModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Adam Rankin (Robarts Research Institute.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerPinholeCamerasModule::icon() const
{
  return QIcon(":/Icons/PinholeCameras.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerPinholeCamerasModule::categories() const
{
  return QStringList() << "Computer Vision";
}

//-----------------------------------------------------------------------------
QStringList qSlicerPinholeCamerasModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerPinholeCamerasModule::setup()
{
  this->Superclass::setup();

  vtkSlicerPinholeCamerasLogic* videoCamerasLogic = vtkSlicerPinholeCamerasLogic::SafeDownCast(this->logic());

  if (qSlicerApplication::application())
  {
    // Register IOs
    qSlicerIOManager* ioManager = qSlicerApplication::application()->ioManager();
    ioManager->registerIO(new qSlicerPinholeCamerasReaderPlugin(videoCamerasLogic, this));
    ioManager->registerIO(new qSlicerNodeWriter("PinholeCameras", QString("PinholeCameraFile"), QStringList() << "vtkMRMLPinholeCameraNode", false, this));
  }
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerPinholeCamerasModule::createWidgetRepresentation()
{
  return new qSlicerPinholeCamerasModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerPinholeCamerasModule::createLogic()
{
  return vtkSlicerPinholeCamerasLogic::New();
}
