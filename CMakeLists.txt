cmake_minimum_required(VERSION 3.5)

project(VideoCameras)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "https://github.com/VASST/SlicerVideoCameras")
set(EXTENSION_CATEGORY "VideoCameras")
set(EXTENSION_CONTRIBUTORS "Adam Rankin (Robarts Research Institute), Elvis Chen (Robarts Research Institute)")
set(EXTENSION_DESCRIPTION "This extension contains modules for interacting with still and video cameras (calibration, capture, etc...)")
set(EXTENSION_ICONURL "https://github.com/VASST/SlicerCamera/blob/master/SlicerVideoCameras_128.png")
set(EXTENSION_SCREENSHOTURLS "https://github.com/VASST/SlicerVideoCameras/blob/master/Documentation/screenshot.png")
set(EXTENSION_DEPENDS "SlicerIGT;SlicerOpenCV;VASSTAlgorithms")
SET(EXTENSION_TAGS "NA")

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

find_package(SlicerIGT REQUIRED)
mark_as_superbuild(SlicerOpenCV_DIR)

find_package(SlicerOpenCV REQUIRED)
mark_as_superbuild(SlicerOpenCV_DIR)

find_package(VASSTAlgorithms REQUIRED)
mark_as_superbuild(VASSTAlgorithms_DIR)

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(VideoCameras)
add_subdirectory(VideoCameraCalibration)
add_subdirectory(VideoCameraRayIntersection)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
