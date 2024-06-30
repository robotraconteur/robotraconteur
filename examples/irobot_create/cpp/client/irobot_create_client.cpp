#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

namespace RR = RobotRaconteur;
namespace c3 = ::experimental::create3;

// This program provides a simple client to the iRobotCreate service
// that connects, drives a bit, and then disconnects

int main(int argc, char* argv[])
{
    std::string url = "rr+tcp://localhost:22354?service=create";
    if (argc > 1)
    {
        url = argv[1];
    }

    try
    {
        // Use node setup to help initialize client node
        RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES);

        // Connect to the service
        c3::CreatePtr c = RR::rr_cast<c3::Create>(
            RR::RobotRaconteurNode::s()->ConnectService(url, "", nullptr, NULL, "experimental.create3.Create"));

        // Set an event handler for the "Bump" event. This can be any valid boost::function, use lambda here
        c->get_bump().connect([]() { std::cout << "Bump!!" << std::endl; });

        // Connect the "packets" wire and add a value changed event handler
        auto wire = c->get_create_state()->Connect();
        // Connect the event handler. This can be any valid boost::function, use lambda here
        wire->WireValueChanged.connect([](const RR::WireConnectionPtr<c3::CreateStatePtr>& wire_connection,
                                          const c3::CreateStatePtr& value, const RR::TimeSpec& time) {
            c3::CreateStatePtr value2 = wire_connection->GetInValue();

            // Uncomment this line to show the current service time
            // std::cout << ((int)value2->time) << std::endl;
        });

        // Set the callback function for the play_callback.  This can be
        // any valid boost::function, use lambda here
        c->get_play_callback()->SetFunction([](double dist, double angle) {
            uint8_t notes[] = {69, 16, 60, 16, 69, 16};
            return RR::AttachRRArrayCopy(notes, 6);
        });

        c->claim_play_callback();

        // Drive the robot a bit
        c->drive(0.2, 5.0);
        boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        c->stop();

        boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

        // Close the wire connection
        wire->Close();
    }
    catch (std::exception& exp)
    {
        // Report an error
        std::cout << "Error occurred in client: " << exp.what() << std::endl;
        return 1;
    }

    return 0;
}
