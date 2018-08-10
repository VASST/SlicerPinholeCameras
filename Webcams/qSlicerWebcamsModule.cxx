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

// Webcams Logic includes
#include <vtkSlicerWebcamsLogic.h>

// Slicer includes
#include <qSlicerApplication.h>
#include <qSlicerIOManager.h>
#include <qSlicerModuleManager.h>
#include <qSlicerNodeWriter.h>

// Slicer logic includes
#include <vtkSlicerApplicationLogic.h>

// Webcams includes
#include "qSlicerWebcamsModule.h"
#include "qSlicerWebcamsModuleWidget.h"
#include "qSlicerWebcamsReaderPlugin.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  #include <QtPlugin>
  Q_EXPORT_PLUGIN2(qSlicerWebcamsModule, qSlicerWebcamsModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerWebcamsModulePrivate
{
public:
  qSlicerWebcamsModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerWebcamsModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerWebcamsModulePrivate::qSlicerWebcamsModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerWebcamsModule methods

//-----------------------------------------------------------------------------
qSlicerWebcamsModule::qSlicerWebcamsModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerWebcamsModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerWebcamsModule::~qSlicerWebcamsModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerWebcamsModule::helpText() const
{
  return "This module exposes the details and operations that can be performed on a webcam.";
}

//-----------------------------------------------------------------------------
QString qSlicerWebcamsModule::acknowledgementText() const
{
  return "This module was developed with support from the Natural Sciences and Engineering Research Council of Canada, the Canadian Foundation for Innovation, and the Virtual Augmentation and Simulation for Surgery and Therapy laboratory, Western University.";
}

//-----------------------------------------------------------------------------
QStringList qSlicerWebcamsModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("Adam Rankin (Robarts Research Institute.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerWebcamsModule::icon() const
{
  return QIcon(":/Icons/Webcams.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerWebcamsModule::categories() const
{
  return QStringList() << "Webcams";
}

//-----------------------------------------------------------------------------
QStringList qSlicerWebcamsModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerWebcamsModule::setup()
{
  this->Superclass::setup();

  vtkSlicerWebcamsLogic* webcamsLogic = vtkSlicerWebcamsLogic::SafeDownCast(this->logic());

  if (qSlicerApplication::application())
  {
    // Register IOs
    qSlicerIOManager* ioManager = qSlicerApplication::application()->ioManager();
    ioManager->registerIO(new qSlicerWebcamsReaderPlugin(webcamsLogic, this));
    ioManager->registerIO(new qSlicerNodeWriter("Webcams", QString("WebcamFile"), QStringList() << "vtkMRMLWebcamNode", false, this));
  }
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerWebcamsModule::createWidgetRepresentation()
{
  return new qSlicerWebcamsModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerWebcamsModule::createLogic()
{
  return vtkSlicerWebcamsLogic::New();
}
