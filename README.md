# SlicerPinholeCameras
An extension for interacting and calibrating videoCameras using [3D Slicer](https://www.github.com/Slicer/Slicer) streamed from a [PlusServer](http://perk-software.cs.queensu.ca/plus/doc/nightly/user/ApplicationPlusServer.html).

## Current Modules
The following modules are currently implemented:

### PinholeCamera Calibration
* Intrinsic calibration: Assuming a [pinhole model](http://opencv-python-tutroals.readthedocs.io/en/latest/py_tutorials/py_calib3d/py_calibration/py_calibration.html), discover the camera parameters using a [6x9  checkerboard](https://github.com/VASST/SlicerPinholeCameras/blob/master/Documentation/checkerboardPattern.png) or [4x11 circle pattern](https://github.com/VASST/SlicerPinholeCameras/blob/master/Documentation/circles_pattern.png) image.
  * Requires a videoCamera node be created
  * Select the input image streamed from a PlusServer, via the [OpenIGTLinkIF](https://github.com/openigtlink/SlicerOpenIGTLink) module.
  * Select the StylusTipToCamera transform streamed from a PlusServer
* Tracker registration: This module enables the registration of an external tracker marker attached to the camera and the camera coordinate system
  * Uses a tracked stylus that has been pivot calibrated in order to determine the pose of the stylus tip.
  * User must manually identify the location of the stylus tip in the image by clicking 'Capture' and then clicking on the tip in the image.

### PinholeCamera Ray Intersection
* This module collects a number of rays in external tracker space and calculates the intersection point and mean distance error.

## Future Work
The following ideas may be implemented in the future:

### Stereo Cameras
A number of modules for calibrating and registering stereo cameras from within Slicer
