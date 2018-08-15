project(vtkSlicer${MODULE_NAME}ModuleMRML)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_MRML_EXPORT")

find_package(OpenCV REQUIRED)

set(${KIT}_INCLUDE_DIRECTORIES
  #${Slicer_Libs_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}
  ${Slicer_Base_INCLUDE_DIRS}
  )

set(${KIT}_SRCS
  vtkMRMLVideoCameraNode.cxx
  vtkMRMLVideoCameraNode.h
  vtkMRMLVideoCameraStorageNode.cxx
  vtkMRMLVideoCameraStorageNode.h
  )

set(${KIT}_TARGET_LIBRARIES
  opencv_videoio
  ${MRML_LIBRARIES}
  SlicerBaseLogic
  )

#-----------------------------------------------------------------------------
SlicerMacroBuildModuleMRML(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT_DIRECTIVE}
  INCLUDE_DIRECTORIES ${${KIT}_INCLUDE_DIRECTORIES}
  SRCS ${${KIT}_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )
