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

// Webcams includes
#include "qSlicerWebcamsModule.h"
#include "qSlicerWebcamsModuleWidget.h"

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
  return "This is a loadable module that can be bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerWebcamsModule::acknowledgementText() const
{
  return "This work was partially funded by NIH grant NXNNXXNNNNNN-NNXN";
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
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation* qSlicerWebcamsModule
::createWidgetRepresentation()
{
  return new qSlicerWebcamsModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerWebcamsModule::createLogic()
{
  return vtkSlicerWebcamsLogic::New();
}
