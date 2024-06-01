
#include "simple_webcam_service_multi.h"

#include <drekar_launch_process_cpp/drekar_launch_process_cpp.h>

// This program provides a simple Robot Raconteur server for viewing
// multiple webcams

// Namespace alias for convenience. Do not use in header files!
namespace RR = RobotRaconteur;
namespace cam = ::experimental::simplewebcam3;

int main(int argc, char* argv[])
{

    // Initialize the host by giving it a vector of names
    std::vector<Webcam_name> names;
    Webcam_name n1 = {0, "Left"};
    names.push_back(n1);
    Webcam_name n2 = {1, "Right"};
    names.push_back(n2);

    RR_SHARED_PTR<WebcamHost_impl> webcamhost = RR_MAKE_SHARED<WebcamHost_impl>(names);

    // Use node setup to help initialize server node
    RR::ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "experimental.simplewebcam3_multi", 22355);

    // Register the service
    auto ctx = RR::RobotRaconteurNode::s()->RegisterService("multiwebcam", "experimental.simplewebcam3", webcamhost);

    // Print out some info for the user
    std::cout << "Simple Multi Webcam Service Started" << std::endl << std::endl;

    std::cout << "Candidate connection urls:" << std::endl;
    ctx->PrintCandidateConnectionURLs();
    std::cout << std::endl;
    std::cout << "Press Ctrl-C to quit" << std::endl;

    // Use drekar_launch_process_cpp package to wait for exit
    drekar_launch_process_cpp::CWaitForExit wait_exit;
    wait_exit.WaitForExit();

    // Shutdown the webcams
    webcamhost->shutdown();

    return 0;
}

// Initialize the webcams
WebcamHost_impl::WebcamHost_impl(std::vector<Webcam_name> names)
{
    for (auto e : names)
    {
        cameras.push_back(RR_MAKE_SHARED<Webcam_impl>(e.index, e.name));
    }
}

WebcamHost_impl::~WebcamHost_impl() { shutdown(); }

// Shutdown all webcams
void WebcamHost_impl::shutdown()
{
    boost::mutex::scoped_lock lock(this_lock);

    for (auto& e : cameras)
    {
        e->shutdown();
    }
}

// Return the indices and names of the available webcams
RR::RRMapPtr<int32_t, RR::RRArray<char> > WebcamHost_impl::get_webcam_names()
{
    auto o = RR::AllocateEmptyRRMap<int32_t, RR::RRArray<char> >();

    for (size_t i = 0; i < cameras.size(); i++)
    {
        auto rr_camname = RR::stringToRRArray(cameras.at(i)->get_name());
        o->insert(std::make_pair(i, rr_camname));
    }

    return o;
}

// Function to implement the "Webcams" objref.  Return the
// object for the selected webcam
RR_SHARED_PTR<cam::Webcam> WebcamHost_impl::get_webcams(int32_t ind)
{
    boost::mutex::scoped_lock lock(this_lock);

    if (!(ind < cameras.size()))
        throw RR::InvalidOperationException("Invalid camera");

    return cameras.at(ind);
}
