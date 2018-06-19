set(KIT ${PROJECT_NAME}Plugins)

set(${KIT}_EXPORT "Q_SLICER_MODULE_${MODULE_NAME_UPPER}_WIDGETS_PLUGINS_EXPORT")

set(${KIT}_SRCS
  qSlicerCameraModuleWidgetsAbstractPlugin.cxx
  qSlicerCameraModuleWidgetsAbstractPlugin.h
  qSlicerCameraModuleWidgetsPlugin.cxx
  qSlicerCameraModuleWidgetsPlugin.h
  qMRMLCameraIntrinsicsWidgetPlugin.cxx
  qMRMLCameraIntrinsicsWidgetPlugin.h
  )

set(${KIT}_MOC_SRCS
  qSlicerCameraModuleWidgetsPlugin.h

  qMRMLCameraIntrinsicsWidgetPlugin.h
  )

set(${KIT}_TARGET_LIBRARIES
  qSlicerCameraModuleWidgets
  )

#-----------------------------------------------------------------------------
# Workaround pulled from SlicerRt
# https://github.com/SlicerRt/SlicerRT/blob/master/Beams/Widgets/DesignerPlugins/CMakeLists.txt
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${Slicer_DIR}/bin)

#-----------------------------------------------------------------------------
ctkMacroBuildQtDesignerPlugin(
  NAME ${KIT}
  EXPORT_DIRECTIVE ${${KIT}_EXPORT}
  SRCS ${${KIT}_SRCS}
  MOC_SRCS ${${KIT}_MOC_SRCS}
  TARGET_LIBRARIES ${${KIT}_TARGET_LIBRARIES}
  )