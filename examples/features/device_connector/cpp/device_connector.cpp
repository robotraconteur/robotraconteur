// device_connector.cpp - Example of using the DeviceConnector class to connect to a device

#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include <RobotRaconteurCompanion/Util/DeviceConnector.h>
#include <RobotRaconteurCompanion/StdRobDef/StdRobDefAll.h>
#include <RobotRaconteurCompanion/Util/ImageUtil.h>
#include <RobotRaconteurCompanion/Util/IdentifierUtil.h>
#include <opencv2/highgui/highgui.hpp>

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;
namespace RRC = RobotRaconteur::Companion;
namespace RR_Util = RobotRaconteur::Companion::Util;
namespace cam = com::robotraconteur::imaging;
namespace img = com::robotraconteur::image;

int main(int argc, char* argv[])
{
    // Register the standard Robot Raconteur types
    RRC::RegisterStdRobDefServiceTypes();

    // Use RobotRaconteur::NodeSetup to initialize Robot Raconteur
    RR::ClientNodeSetup node_setup(std::vector<RR_SHARED_PTR<RR::ServiceFactory> >(), argc, argv);

    // Create a DeviceConnector
    RR_Util::DeviceConnector con;
    con.Init();

    // Create a DeviceConnectorDetails to connect to the camera
    RR_Util::DeviceConnectorDetails d1("camera", RR_Util::CreateIdentifierFromName("gazebo_camera"),
                                       {"com.robotraconteur.imaging.Camera"});

    // Add the camera to the DeviceConnector
    con.AddDevice(d1);

    // Get the subscription to the camera
    auto sub = con.GetDevice("camera");

    // Get the camera client
    auto c = sub->GetDefaultClientWait<cam::Camera>(5000);

    // Capture an image and convert to OpenCV image type
    cv::Mat frame1 = RR_Util::ImageToMat(c->capture_frame());

    // Show image
    cv::imshow("Image", frame1);
    cv::waitKey();
    cv::destroyAllWindows();

    return 0;
}
