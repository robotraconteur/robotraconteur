
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

#include <opencv2/highgui/highgui.hpp>

// Do not use aliases in header files!
namespace RR = RobotRaconteur;
namespace cam = ::experimental::simplewebcam3;

// Simple client to read images from a Webcam server
// and display the images

// This exmaple expects the simple_webcam_service_multi multi camera service to be running

// Convert WebcamImage to OpenCV format
cv::Mat webcam_image_to_mat(cam::WebcamImagePtr image)
{
    cv::Mat frame2(image->height, image->width, CV_8UC3);
    memcpy(frame2.data, &image->data->at(0), image->data->size());
    return frame2;
}

int main(int argc, char* argv[])
{
    std::string url = "rr+tcp://localhost:22355?service=multiwebcam";
    if (argc > 1)
    {
        url = argv[1];
    }

    try
    {
        // Use node setup to help initialize client node
        RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES);

        cam::WebcamHostPtr c_host = RR::rr_cast<cam::WebcamHost>(RR::RobotRaconteurNode::s()->ConnectService(
            url, "", nullptr, NULL, "experimental.simplewebcam3.WebcamHost"));

        // Get the Webcam objects from the "Webcams" objref
        cam::WebcamPtr c1 = c_host->get_webcams(0);
        cam::WebcamPtr c2 = c_host->get_webcams(1);

        // Capture an image and convert to OpenCV image type
        cv::Mat frame1 = webcam_image_to_mat(c1->capture_frame());
        cv::Mat frame2 = webcam_image_to_mat(c2->capture_frame());

        // Show image
        cv::imshow("Left", frame1);
        cv::imshow("Right", frame2);

        // Wait for enter key to be pressed
        cv::waitKey();

        // Close the image viewers
        cv::destroyAllWindows();

        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << "Error occured in client: " << std::string(e.what()) << std::endl;
        return 1;
    }
}
