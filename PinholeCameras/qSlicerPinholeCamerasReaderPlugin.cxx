/*==============================================================================

  Program: 3D Slicer

  Copyright (c) Kitware Inc.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Adam Rankin, Robarts Research Institute

==============================================================================*/

// Qt includes
#include <QFileInfo>

// SlicerQt includes
#include "qSlicerPinholeCamerasReaderPlugin.h"

// Logic includes
#include "vtkSlicerPinholeCamerasLogic.h"

// MRML includes
#include "vtkMRMLPinholeCameraNode.h"
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class qSlicerPinholeCamerasReaderPluginPrivate
{
public:
  vtkSmartPointer<vtkSlicerPinholeCamerasLogic> PinholeCamerasLogic;
};

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_PinholeCameras
qSlicerPinholeCamerasReaderPlugin::qSlicerPinholeCamerasReaderPlugin(vtkSlicerPinholeCamerasLogic* _PinholeCamerasLogic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerPinholeCamerasReaderPluginPrivate)
{
  this->setPinholeCamerasLogic(_PinholeCamerasLogic);
}

//-----------------------------------------------------------------------------
qSlicerPinholeCamerasReaderPlugin::~qSlicerPinholeCamerasReaderPlugin()
{
}

//-----------------------------------------------------------------------------
void qSlicerPinholeCamerasReaderPlugin::setPinholeCamerasLogic(vtkSlicerPinholeCamerasLogic* newPinholeCamerasLogic)
{
  Q_D(qSlicerPinholeCamerasReaderPlugin);
  d->PinholeCamerasLogic = newPinholeCamerasLogic;
}

//-----------------------------------------------------------------------------
vtkSlicerPinholeCamerasLogic* qSlicerPinholeCamerasReaderPlugin::PinholeCamerasLogic()const
{
  Q_D(const qSlicerPinholeCamerasReaderPlugin);
  return d->PinholeCamerasLogic;
}

//-----------------------------------------------------------------------------
QString qSlicerPinholeCamerasReaderPlugin::description()const
{
  return "PinholeCamera";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerPinholeCamerasReaderPlugin::fileType()const
{
  return QString("PinholeCameraFile");
}

//-----------------------------------------------------------------------------
QStringList qSlicerPinholeCamerasReaderPlugin::extensions()const
{
  return QStringList()
         << "PinholeCamera (*.xml)";
}

//-----------------------------------------------------------------------------
bool qSlicerPinholeCamerasReaderPlugin::load(const IOProperties& properties)
{
  Q_D(qSlicerPinholeCamerasReaderPlugin);
  Q_ASSERT(properties.contains("fileName"));
  QString fileName = properties["fileName"].toString();

  this->setLoadedNodes(QStringList());
  if (d->PinholeCamerasLogic.GetPointer() == 0)
  {
    return false;
  }
  vtkMRMLPinholeCameraNode* node = d->PinholeCamerasLogic->AddPinholeCamera(fileName.toLatin1());
  if (!node)
  {
    return false;
  }
  this->setLoadedNodes(QStringList(QString(node->GetID())));
  if (properties.contains("name"))
  {
    std::string uname = this->mrmlScene()->GetUniqueNameByString(properties["name"].toString().toLatin1());
    node->SetName(uname.c_str());
  }
  return true;
}
