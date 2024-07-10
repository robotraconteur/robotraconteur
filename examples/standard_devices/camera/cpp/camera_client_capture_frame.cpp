// Simple example Robot Raconteur standard camera client
// This program will capture a single frame and
// display it.

#include <RobotRaconteur.h>

#include <RobotRaconteurCompanion/StdRobDef/StdRobDefAll.h>
#include <RobotRaconteurCompanion/Util/ImageUtil.h>

#include <opencv2/highgui/highgui.hpp>

// Do not use aliases in header files!
namespace RR = RobotRaconteur;
namespace cam = com::robotraconteur::imaging;
namespace img = com::robotraconteur::image;
namespace RRC_Util = RobotRaconteur::Companion::Util;

int main(int argc, char* argv[])
{
    std::string url = "rr+tcp://localhost:59823?service=camera";
    if (argc > 1)
    {
        url = argv[1];
    }

    try
    {

        // Register the standard Robot Raconteur types
        RobotRaconteur::Companion::RegisterStdRobDefServiceTypes();

        // Use node setup to help initialize client node
        RR::ClientNodeSetup node_setup(std::vector<RR::ServiceFactoryPtr>(), argc, argv);

        cam::CameraPtr c = RR::rr_cast<cam::Camera>(
            RR::RobotRaconteurNode::s()->ConnectService(url, "", nullptr, NULL, "com.robotraconteur.imaging.Camera"));

        // Capture an image and convert to OpenCV image type
        cv::Mat frame1 = RRC_Util::ImageToMat(c->capture_frame());

        // Show image
        cv::imshow("Image", frame1);

        // Wait for enter key to be pressed
        cv::waitKey();

        // Close the image viewers
        cv::destroyAllWindows();

        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << "Error occurred in client: " << std::string(e.what()) << std::endl;
        return 1;
    }
}
