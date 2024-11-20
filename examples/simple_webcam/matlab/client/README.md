# Simple Webcam Matlab Robot Raconteur Client Example

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

The Matlab Robot Raconteur add-on are required to run the example.

Install the Robot Raconteur Matlab add-on if not already installed. The add-on can be installed using
the [Add-On Explorer in Matlab](https://www.mathworks.com/products/matlab/add-on-explorer.html).
Search for "Robot Raconteur" and install the add-on. The homepage for the add-on
can be found [here](https://www.mathworks.com/matlabcentral/fileexchange/176028-robot-raconteur-matlab).

One of the other simple webcam service examples must be running. The instructions below use the Python
service examples. See the instructions for the other examples for additional setup steps.

## Run Example

Start the Python service by navigating to `examples/simple_webcam/python/service` directory and run:

```bash
python simple_webcam_service.py
```

Replace `python` with `python3` if necessary. Replace `simple_webcam_service.py` with `simple_webcam_service_multi.py`
if using the multi camera service.

Open Matlab and navigate to the `examples/simple_webcam/matlab/client` directory. Run one of the following commands:

- `simple_webcam_client.m` - Connects to the multi camera service and captures a single frame from one or two cameras.
- `simple_webcam_client_streaming.m` - Connects to the single camera service and streams images from the camera.
- `simple_webcam_client_memory.m` - Connects to the single camera service and captures a frame to memory members.
