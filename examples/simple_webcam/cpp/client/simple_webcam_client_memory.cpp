
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

#include <opencv2/highgui/highgui.hpp>

// Do not use aliases in header files!
namespace RR = RobotRaconteur;
namespace cam = ::experimental::simplewebcam3;

// Simple client to read images from a Webcam server
// and display the image.  This example uses the "memory"
// member type

// This exmaple expects the simple_webcam_service single camera service to be running

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

        // Capture an image to the "buffer" and "multidimbuffer"
        cam::WebcamImage_sizePtr size = c1->capture_frame_to_buffer();

        // Read the full image from the "buffer" memory
        uint64_t l = c1->get_buffer()->Length();
        RR::RRArrayPtr<uint8_t> data = RR::AllocateRRArray<uint8_t>(l);
        c1->get_buffer()->Read(0, data, 0, l);

        // Convert and show the image retrieved from the buffer memory
        cv::Mat frame1(size->height, size->width, CV_8UC3);
        memcpy(frame1.data, &data->at(0), data->size());
        cv::imshow("buffer", frame1);

        // Read the dimensions of the "multidimbuffer" member
        std::vector<uint64_t> bufsize = c1->get_multidimbuffer()->Dimensions();

        // Retrieve the data from the "multidimbuffer"
        RR::RRMultiDimArrayPtr<uint8_t> segdata = RR::AllocateEmptyRRMultiDimArray<uint8_t>({100, 100, 1});

        c1->get_multidimbuffer()->Read({10, 10, 0}, segdata, {0, 0, 0}, {100, 100, 1});

        // Create a new image and copy data from the WebcamImage
        cv::Mat frame2(100, 100, CV_8U);
        memcpy(frame2.data, &segdata->Array->at(0), segdata->Array->size());

        // Take the transpose because of array ordering difference
        cv::Mat frame3 = frame2.t();

        // Show the Image
        cv::imshow("multidimbuffer", frame3);

        // Wait for user and then close the window
        cv::waitKey();
        cv::destroyAllWindows();

        return 0;
    }
    catch (std::exception& e)
    {
        std::cout << "Error occured in client: " << std::string(e.what()) << std::endl;
        return 1;
    }
}
