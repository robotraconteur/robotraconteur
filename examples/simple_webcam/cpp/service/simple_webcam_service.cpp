
#include "simple_webcam_impl.h"

#include <drekar_launch_process_cpp/drekar_launch_process_cpp.h>

// This program provides a simple Robot Raconteur server for viewing
// a single webcam

// Namespace alias for convenience. Do not use in header files!
namespace RR = RobotRaconteur;
namespace cam = ::experimental::simplewebcam3;

int main(int argc, char* argv[])
{

    RR_SHARED_PTR<Webcam_impl> webcam = RR_MAKE_SHARED<Webcam_impl>(0, "Camera");

    // Use node setup to help initialize server node
    RR::ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "experimental.simplewebcam3", 22355);

    // Register the service
    auto ctx = RR::RobotRaconteurNode::s()->RegisterService("webcam", "experimental.simplewebcam3", webcam);

    // Print out some info for the user
    std::cout << "Simple Webcam Service Started" << std::endl << std::endl;

    std::cout << "Candidate connection urls:" << std::endl;
    ctx->PrintCandidateConnectionURLs();
    std::cout << std::endl;
    std::cout << "Press Ctrl-C to quit" << std::endl;

    // Use drekar_launch_process_cpp package to wait for exit
    drekar_launch_process_cpp::CWaitForExit wait_exit;
    wait_exit.WaitForExit();

    // Shutdown the webcam
    webcam->shutdown();

    return 0;
}
