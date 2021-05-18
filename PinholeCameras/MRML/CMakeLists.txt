project(vtkSlicer${MODULE_NAME}ModuleMRML)

set(KIT "${PROJECT_NAME}")

set(${KIT}_EXPORT_DIRECTIVE "VTK_SLICER_${MODULE_NAME_UPPER}_MODULE_MRML_EXPORT")

find_package(OpenCV REQUIRED)

set(${KIT}_INCLUDE_DIRECTORIES
  ${OpenCV_INCLUDE_DIRS}
  ${Slicer_Base_INCLUDE_DIRS}
  )

set(${KIT}_SRCS
  vtkMRMLPinholeCameraNode.cxx
  vtkMRMLPinholeCameraNode.h
  vtkMRMLPinholeCameraStorageNode.cxx
  vtkMRMLPinholeCameraStorageNode.h
  )

set(${KIT}_TARGET_LIBRARIES
  PRIVATE
    opencv_videoio
  PUBLIC
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
