
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

#include <opencv2/highgui/highgui.hpp>

namespace RR = RobotRaconteur;
namespace cam = ::experimental::simplewebcam3;

// Simple client to read streaming images from the Webcam pipe to show
// a live view from the cameras

// This exmaple expects the simple_webcam_service single camera service to be running

// Convert WebcamImage to OpenCV format
cv::Mat webcam_image_to_mat(cam::WebcamImagePtr image)
{
    cv::Mat frame2(image->height, image->width, CV_8UC3);
    memcpy(frame2.data, &image->data->at(0), image->data->size());
    return frame2;
}

cv::Mat current_frame;

// Function to handle when a new frame is received
// This function will be called by a separate thread by
// Robot Raconteur.
void new_frame(RR::PipeEndpointPtr<cam::WebcamImagePtr> pipe_ep)
{
    while (pipe_ep->Available() > 0)
    {
        cam::WebcamImagePtr image = pipe_ep->ReceivePacket();
        if (image->data)
        {
            current_frame = webcam_image_to_mat(image);
        }
    }
}

int main(int argc, char* argv[])
{
    std::string url = "rr+tcp://localhost:22355?service=webcam";
    if (argc > 1)
    {
        url = argv[1];
    }

    try
    {
        // Use node setup to help initialize client node
        RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES);

        // Connect to the service
        cam::WebcamPtr c1 = RR::rr_cast<cam::Webcam>(
            RR::RobotRaconteurNode::s()->ConnectService(url, "", nullptr, NULL, "experimental.simplewebcam3.Webcam"));

        // Connect to the frame_stream pipe and receive a PipeEndpoint
        // PipeEndpoints a symmetric on client and service meaning that
        // you can send and receive on both ends
        RR::PipeEndpointPtr<cam::WebcamImagePtr> p = c1->get_frame_stream()->Connect(-1);
        // Add a callback for when a new pipe packet is received
        p->PacketReceivedEvent.connect([](RR::PipeEndpointPtr<cam::WebcamImagePtr> ep) { new_frame(ep); });

        // Show a named window
        cv::namedWindow("Image");

        try
        {
            // Start streaming image packets
            c1->start_streaming();
        }
        catch (std::exception&)
        {}

        // Loop through and show the new image if available
        while (true)
        {
            if (!current_frame.empty())
            {
                cv::imshow("Image", current_frame);
            }
            // Break the loop if "enter" is pressed on a window
            if (cv::waitKey(50) != -1)
                break;
        }

        // Stop streaming images
        c1->stop_streaming();

        // Close the window
        cv::destroyAllWindows();

        // Close the PipeEndpoint
        p->Close();

        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << "Error occured in client: " << std::string(e.what()) << std::endl;
        return -1;
    }
}
