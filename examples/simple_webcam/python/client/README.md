# Simple Webcam Python Robot Raconteur Client Examples

Several examples are provided for clients communicating with the simple webcam service. These examples expect
either the `simple_webcam_service` or `simple_webcam_service_multi` services to be running depending on the example.

- `simple_webcam_client` - (`simple_webcam_service_multi`) Connects to the multi camera service and captures a single frame
   from one or two cameras using the `capture_frame()` function member.
   The captured images are displayed for the user. Select one of the windows and press enter
   to close the window and exit the program.
- `simple_webcam_client_streaming` - (`simple_webcam_service`) Connects to the single camera service and streams images
   from the camera using the `frame_stream` pipe member.
   The images are displayed for the user. Press escape to close the window and exit the program.
- `simple_webcam_client_memory` - (`simple_webcam_service`) Connects to the single camera service and captures
   a frame using the `capture_frame_to_buffer()` function member. This function captures a frame and stores it to
   the buffers used by the `buffer` and `multidimbuffer` memory members. Segments of the memory members are displayed.
   This example is useful for demonstrating how to use memory members in Robot Raconteur.

These examples assume that the corresponding service is running. Any of the simple webcam service
examples should work regardless of language. Pass the correct URL as the first argument on the
command line of a different URL is required for the connection, for example if the service is running on a different
machine.

The simple webcam example demonstrates using `property`, `function`, `pipe`, `objref`, and `memory` members. It also
demonstrates using constants and structures.

Instructions are provided for Windows and Ubuntu. These examples will work
on other platforms with minor modifications.

## Setup

Python and several packages are required to run the example.

### Windows

Install Python using Chocolatey or by downloading the installer from the [Python website](https://www.python.org/downloads/).

Once installed, run the following command in a command prompt to install the required Python packages using pip:

```cmd
python -m pip install --user robotraconteur opencv-contrib-python
```

### Ubuntu

Install the following packages using apt:

```bash
sudo apt install python3-pip python3-numpy
```

Install the required Python packages using pip:

```bash
python3 -m pip install --user robotraconteur opencv-contrib-python
```

The `robotraconteur` python package can also be installed using apt packages instead of pip. See
[Installation](https://github.com/robotraconteur/robotraconteur/blob/master/docs/common/installation.md) for more
information on additional installation methods.

## Run Example

### Windows

Open a command prompt and start the appropriate simple webcam service. For the single camera Python service run:

```cmd
cd robotraconteur\examples\simple_webcam\python\service
python simple_webcam_service.py
```

In another command prompt and run the client:

```cmd
cd robotraconteur\examples\simple_webcam\python\client
python simple_webcam_client_streaming.py
```

Replace `simple_webcam_client_streaming.py` with the appropriate client script for the example.

### Ubuntu

Open a terminal and start start the appropriate simple webcam service. For the single camera Python service run:

```bash
cd robotraconteur/examples/simple_webcam/python/service
python simple_webcam_service.py
```

In another terminal and run the client:

```bash
cd robotraconteur/examples/simple_webcam/python/client
python simple_webcam_client_streaming.py
```

Replace `simple_webcam_client_streaming.py` with the appropriate client script for the example.
