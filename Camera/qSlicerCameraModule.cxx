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

// Camera Logic includes
#include <vtkSlicerCameraLogic.h>

// Camera includes
#include "qSlicerCameraModule.h"
#include "qSlicerCameraModuleWidget.h"

//-----------------------------------------------------------------------------
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QtPlugin>
Q_EXPORT_PLUGIN2(qSlicerCameraModule, qSlicerCameraModule);
#endif

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerCameraModulePrivate
{
public:
  qSlicerCameraModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerCameraModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerCameraModulePrivate::qSlicerCameraModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerCameraModule methods

//-----------------------------------------------------------------------------
qSlicerCameraModule::qSlicerCameraModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerCameraModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerCameraModule::~qSlicerCameraModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerCameraModule::helpText() const
{
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerCameraModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCameraModule::contributors() const
{
  QStringList moduleContributors;
  moduleContributors << QString("John Doe (AnyWare Corp.)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerCameraModule::icon() const
{
  return QIcon(":/Icons/Camera.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerCameraModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerCameraModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerCameraModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerCameraModule
::createWidgetRepresentation()
{
  return new qSlicerCameraModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerCameraModule::createLogic()
{
  return vtkSlicerCameraLogic::New();
}
