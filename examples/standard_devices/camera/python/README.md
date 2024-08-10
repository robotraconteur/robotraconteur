# Camera Python Client Examples

This directory contains examples for capturing and displaying images from standard cameras.
Use with either the [robotraconteur_training_sim](https://github.com/robotraconteur-contrib/robotraconteur_training_sim)
or with a Robot Raconteur driver that exposes the standard camera service type such as the
[robotraconteur_camera_driver](https://github.com/robotraconteur-contrib/robotraconteur_camera_driver).
By default the examples will connect to the URL for the training
simulator simulated camera. Pass the URL of the real camera driver if using a real camera.
The real URL can be found using the
[Robot Raconteur Service Browser](https://github.com/robotraconteur/RobotRaconteur_ServiceBrowser).

See the [Simple Webcam client example readme](../../../simple_webcam/python/client/README.md)
for instructions to set up the Python environment.

The `robotraconteur_companion_python` package is also required. Install using:

```
python -m pip install --user robotraconteurcompanion opencv-contrib-python
```

Examples:

- `camera_client_capture_frame.py` - Capture and display a single image frame
- `camera_client_capture_frame_compressed.py` - Capture and display a lossless compressed image frame
- `camera_client_image.py` - Display streaming images from camera (full quality)
- `camera_client_image_preview.py` - Display streaming preview stream, a reduced quality/bandwidth MJPEG stream
