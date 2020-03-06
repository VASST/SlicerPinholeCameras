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
#include "qSlicerVideoCamerasReaderPlugin.h"

// Logic includes
#include "vtkSlicerVideoCamerasLogic.h"

// MRML includes
#include "vtkMRMLVideoCameraNode.h"
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class qSlicerVideoCamerasReaderPluginPrivate
{
public:
  vtkSmartPointer<vtkSlicerVideoCamerasLogic> VideoCamerasLogic;
};

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_VideoCameras
qSlicerVideoCamerasReaderPlugin::qSlicerVideoCamerasReaderPlugin(vtkSlicerVideoCamerasLogic* _VideoCamerasLogic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerVideoCamerasReaderPluginPrivate)
{
  this->setVideoCamerasLogic(_VideoCamerasLogic);
}

//-----------------------------------------------------------------------------
qSlicerVideoCamerasReaderPlugin::~qSlicerVideoCamerasReaderPlugin()
{
}

//-----------------------------------------------------------------------------
void qSlicerVideoCamerasReaderPlugin::setVideoCamerasLogic(vtkSlicerVideoCamerasLogic* newVideoCamerasLogic)
{
  Q_D(qSlicerVideoCamerasReaderPlugin);
  d->VideoCamerasLogic = newVideoCamerasLogic;
}

//-----------------------------------------------------------------------------
vtkSlicerVideoCamerasLogic* qSlicerVideoCamerasReaderPlugin::VideoCamerasLogic()const
{
  Q_D(const qSlicerVideoCamerasReaderPlugin);
  return d->VideoCamerasLogic;
}

//-----------------------------------------------------------------------------
QString qSlicerVideoCamerasReaderPlugin::description()const
{
  return "VideoCamera";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerVideoCamerasReaderPlugin::fileType()const
{
  return QString("VideoCameraFile");
}

//-----------------------------------------------------------------------------
QStringList qSlicerVideoCamerasReaderPlugin::extensions()const
{
  return QStringList()
         << "VideoCamera (*.xml)";
}

//-----------------------------------------------------------------------------
bool qSlicerVideoCamerasReaderPlugin::load(const IOProperties& properties)
{
  Q_D(qSlicerVideoCamerasReaderPlugin);
  Q_ASSERT(properties.contains("fileName"));
  QString fileName = properties["fileName"].toString();

  this->setLoadedNodes(QStringList());
  if (d->VideoCamerasLogic.GetPointer() == 0)
  {
    return false;
  }
  vtkMRMLVideoCameraNode* node = d->VideoCamerasLogic->AddVideoCamera(fileName.toLatin1());
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
