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

#ifndef __qSlicerPinholeCamerasReaderPlugin_h
#define __qSlicerPinholeCamerasReaderPlugin_h

// SlicerQt includes
#include "qSlicerFileReader.h"
class qSlicerPinholeCamerasReaderPluginPrivate;

// Slicer includes
class vtkSlicerPinholeCamerasLogic;

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_PinholeCameras
class qSlicerPinholeCamerasReaderPlugin
  : public qSlicerFileReader
{
  Q_OBJECT
public:
  typedef qSlicerFileReader Superclass;
  qSlicerPinholeCamerasReaderPlugin(vtkSlicerPinholeCamerasLogic* PinholeCamerasLogic = 0, QObject* parent = 0);
  virtual ~qSlicerPinholeCamerasReaderPlugin();

  void setPinholeCamerasLogic(vtkSlicerPinholeCamerasLogic* PinholeCamerasLogic);
  vtkSlicerPinholeCamerasLogic* PinholeCamerasLogic()const;

  virtual QString description()const;
  virtual IOFileType fileType()const;
  virtual QStringList extensions()const;

  virtual bool load(const IOProperties& properties);

protected:
  QScopedPointer<qSlicerPinholeCamerasReaderPluginPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerPinholeCamerasReaderPlugin);
  Q_DISABLE_COPY(qSlicerPinholeCamerasReaderPlugin);
};

#endif
