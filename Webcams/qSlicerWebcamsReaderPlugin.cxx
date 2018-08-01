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
#include "qSlicerWebcamsReaderPlugin.h"

// Logic includes
#include "vtkSlicerWebcamsLogic.h"

// MRML includes
#include "vtkMRMLWebcamNode.h"
#include <vtkMRMLScene.h>

// VTK includes
#include <vtkSmartPointer.h>

//-----------------------------------------------------------------------------
class qSlicerWebcamsReaderPluginPrivate
{
public:
  vtkSmartPointer<vtkSlicerWebcamsLogic> WebcamsLogic;
};

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Webcams
qSlicerWebcamsReaderPlugin::qSlicerWebcamsReaderPlugin(vtkSlicerWebcamsLogic* _WebcamsLogic, QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerWebcamsReaderPluginPrivate)
{
  this->setWebcamsLogic(_WebcamsLogic);
}

//-----------------------------------------------------------------------------
qSlicerWebcamsReaderPlugin::~qSlicerWebcamsReaderPlugin()
{
}

//-----------------------------------------------------------------------------
void qSlicerWebcamsReaderPlugin::setWebcamsLogic(vtkSlicerWebcamsLogic* newWebcamsLogic)
{
  Q_D(qSlicerWebcamsReaderPlugin);
  d->WebcamsLogic = newWebcamsLogic;
}

//-----------------------------------------------------------------------------
vtkSlicerWebcamsLogic* qSlicerWebcamsReaderPlugin::WebcamsLogic()const
{
  Q_D(const qSlicerWebcamsReaderPlugin);
  return d->WebcamsLogic;
}

//-----------------------------------------------------------------------------
QString qSlicerWebcamsReaderPlugin::description()const
{
  return "Webcam";
}

//-----------------------------------------------------------------------------
qSlicerIO::IOFileType qSlicerWebcamsReaderPlugin::fileType()const
{
  return QString("WebcamFile");
}

//-----------------------------------------------------------------------------
QStringList qSlicerWebcamsReaderPlugin::extensions()const
{
  return QStringList()
         << "Webcam (*.xml)";
}

//-----------------------------------------------------------------------------
bool qSlicerWebcamsReaderPlugin::load(const IOProperties& properties)
{
  Q_D(qSlicerWebcamsReaderPlugin);
  Q_ASSERT(properties.contains("fileName"));
  QString fileName = properties["fileName"].toString();

  this->setLoadedNodes(QStringList());
  if (d->WebcamsLogic.GetPointer() == 0)
  {
    return false;
  }
  vtkMRMLWebcamNode* node = d->WebcamsLogic->AddWebcam(fileName.toLatin1());
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
