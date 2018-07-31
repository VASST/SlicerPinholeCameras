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

#ifndef __qSlicerWebcamsReader_h
#define __qSlicerWebcamsReader_h

// SlicerQt includes
#include "qSlicerFileReader.h"
class qSlicerWebcamsReaderPrivate;

// Slicer includes
class vtkSlicerWebcamsLogic;

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_Webcams
class qSlicerWebcamsReader
  : public qSlicerFileReader
{
  Q_OBJECT
public:
  typedef qSlicerFileReader Superclass;
  qSlicerWebcamsReader(vtkSlicerWebcamsLogic* WebcamsLogic = 0, QObject* parent = 0);
  virtual ~qSlicerWebcamsReader();

  void setWebcamsLogic(vtkSlicerWebcamsLogic* WebcamsLogic);
  vtkSlicerWebcamsLogic* WebcamsLogic()const;

  virtual QString description()const;
  virtual IOFileType fileType()const;
  virtual QStringList extensions()const;

  virtual bool load(const IOProperties& properties);

protected:
  QScopedPointer<qSlicerWebcamsReaderPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(qSlicerWebcamsReader);
  Q_DISABLE_COPY(qSlicerWebcamsReader);
};

#endif
