// device_connector_yaml.cpp - Example of using the DeviceConnector class to connect to a device using a YAML file

// To run the example:
//    device_connector_yaml --devices devices.yml

#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include <RobotRaconteurCompanion/Util/DeviceConnector.h>
#include <RobotRaconteurCompanion/StdRobDef/StdRobDefAll.h>
#include <RobotRaconteurCompanion/Util/ImageUtil.h>
#include <RobotRaconteurCompanion/Util/IdentifierUtil.h>
#include <opencv2/highgui/highgui.hpp>
#include <boost/program_options.hpp>
#include <fstream>

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;
namespace RRC = RobotRaconteur::Companion;
namespace RR_Util = RobotRaconteur::Companion::Util;
namespace cam = com::robotraconteur::imaging;
namespace img = com::robotraconteur::image;

int main(int argc, char* argv[])
{

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()("devices", boost::program_options::value<std::string>(),
                       "YAML file containing device definitions");

    // Parse known options. Use allow_unregistered() to allow unknown options due to
    // possible --robotraconteur-* options being passed to the node
    boost::program_options::variables_map vm;
    boost::program_options::store(
        boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), vm);

    // Open the YAML file
    std::ifstream file(vm["devices"].as<std::string>());
    if (!file.is_open())
    {
        std::cerr << "Error opening devices file" << std::endl;
        return 1;
    }

    // Register the standard Robot Raconteur types
    RRC::RegisterStdRobDefServiceTypes();

    // Use RobotRaconteur::NodeSetup to initialize Robot Raconteur
    RR::ClientNodeSetup node_setup(std::vector<RR_SHARED_PTR<RR::ServiceFactory> >(), argc, argv);

    // Create a DeviceConnector and initialize from the YAML file
    RR_Util::DeviceConnector con;
    con.InitFromYamlFile(file);

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
