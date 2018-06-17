cmake_minimum_required(VERSION 3.5)

project(SlicerCamera)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "http://slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/SlicerCamera")
set(EXTENSION_CATEGORY "Camera")
set(EXTENSION_CONTRIBUTORS "Adam Rankin (Robarts Research Institute), Elvis Chen (Robarts Research Institute)")
set(EXTENSION_DESCRIPTION "This extension contains modules for interacting with cameras (calibration, capture, etc...)")
set(EXTENSION_ICONURL "http://www.example.com/Slicer/Extensions/SlicerCamera.png")
set(EXTENSION_SCREENSHOTURLS "http://www.example.com/Slicer/Extensions/SlicerCamera/Screenshots/1.png")
set(EXTENSION_DEPENDS "SlicerOpenCV;VASSTRegistration") # Specified as a space separated string, a list or 'NA' if any
SET(EXTENSION_TAGS "NA") # Specified as a space separated string, a list or 'NA' if any

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

find_package(SlicerOpenCV REQUIRED)
mark_as_superbuild(SlicerOpenCV_DIR)

find_package(VASSTRegistration REQUIRED)
mark_as_superbuild(VASSTRegistration_DIR)

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(CameraCalibration)
add_subdirectory(Camera)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_GENERATE_CONFIG})
include(${Slicer_EXTENSION_CPACK})
