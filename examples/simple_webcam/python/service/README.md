# Simple Webcam Python Robot Raconteur Service Examples

This directory contains examples of Robot Raconteur services with webcams. The services use OpenCV to capture
images from the webcam, and uses Robot Raconteur to provide the images to clients. Two services are provided,
a single camera service and a multi-camera service.

The simple webcam example demonstrates using `property`, `function`, `pipe`, `objref`, and `memory` members. It also
demonstrates using constants and structures.

Practical systems should use the standard camera driver
[robotraconteur_camera_driver](https://github.com/robotraconteur-contrib/robotraconteur_camera_driver)

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Connection Info

The single and multi camera services by default listen on TCP port 22355. Note that only
one of the services can be used at a time because they listen on the same TCP port. Use the following connection
information to connect to the services:

- Single Camera Service
  - URL: `rr+tcp://localhost:22355?service=webcam`
  - Node Name: `experimental.simplewebcam3`
  - Service Name: `webcam`
  - Root Object Type: `experimental.simplewebcam3.Webcam`
- Multi Camera Service
  - URL: `rr+tcp://localhost:22355?service=multiwebcam`
  - Node Name: `experimental.simplewebcam3_multi`
  - Service Name: `multiwebcam`
  - Root Object Type: `experimental.simplewebcam3.WebcamHost`

## Setup

Python and several packages are required to run the example.

### Windows

Install Python using Chocolatey or by downloading the installer from the [Python website](https://www.python.org/downloads/).

Once installed, run the following command in a command prompt to install the required Python packages using pip:

```cmd
python -m pip install --user robotraconteur drekar-launch-process opencv-contrib-python
```

### Ubuntu

Install the following packages using apt:

```bash
sudo apt install python3-pip python3-numpy
```

Install the required Python packages using pip:

```bash
python3 -m pip install --user robotraconteur drekar-launch-process opencv-contrib-python
```

The `robotraconteur` python package can also be installed using apt packages instead of pip. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md) for more
information on additional installation methods.

## Run Example

### Windows

Open a command prompt and navigate to the `examples/simple_webcam/python/service` directory.
Run the following command:

```cmd
python simple_webcam_service.py
```

Or

```cmd
python simple_webcam_service_multi.py
```

The example services listen on port 22355. Run one of the client examples:

```cmd
cd robotraconteur\examples\simple_webcam\python\client
python simple_webcam_client_streaming.py
```

### Ubuntu

Open a terminal and navigate to the `examples/simple_webcam/python/service` directory.
Run the following command:

```bash
python3 simple_webcam_service.py
```

Or

```bash
python3 simple_webcam_service_multi.py
```

The example services listen on port 22355. Run one of the client examples:

```bash
cd robotraconteur/examples/simple_webcam/python/client
python3 simple_webcam_client_streaming.py
```
