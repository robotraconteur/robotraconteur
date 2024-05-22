// C++ example Robot Raconteur service for Reynard the Robot using the ASCII Socket intreface to communicate
// with the robot.

// Note that Reynard the Robot uses mm and degrees, while Robot Raconteur services are expected
// to use meters and radians. The service will convert the units as needed.

#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

#include "drekar_launch_process_cpp/drekar_launch_process_cpp.h"

#include <boost/asio/ip/tcp.hpp>

// Only use the RR alias in cpp files. Do not use it in header files.

namespace RR = RobotRaconteur;

#define M_PI 3.14159265358979323846

// Define the class that implements the service. Use the generated class from the Robot Raconteur service definition
// in experimental__reynard_the_robot_stubskel.h to extend the Reynard_default_impl class. Override the functions of
// interest to implement the service. Reynard_default_impl extends the Reynard interface and provides default
// implementations for the members. Note the functions with "override" keyword are overriding the functions in the
// Reynard interface.

// Underscores are used in the class name to avoid conflicts with the generated class name. This is not required,
// but is a common practice.

// This example uses a simple ASCII socket interface to communicate with the robot. Many industrial devices
// will use an ASCII socket interface or serial port interface. Many other devices will use a binary interface.
// Binary interfaces are more efficient but are also more complicated. See the iRobot Create example for a
// binary serial port interface example. Many devices that use a binary socket interface will provide a client
// library that implements the protocol.

class Reynard_impl : public virtual experimental::reynard_the_robot::Reynard_default_impl,
                     public RR_ENABLE_SHARED_FROM_THIS<Reynard_impl>
{
  protected:
    boost::asio::ip::tcp::endpoint reynard_socket_endpoint;
    RR::TimerPtr state_timer;

  public:
    Reynard_impl(const boost::asio::ip::tcp::endpoint& reynard_socket_endpoint)
    {
        this->reynard_socket_endpoint = reynard_socket_endpoint;
    }

    // _communicate is a simple request and response function that opens a socket, sends a request, and reads a
    // response. More sophisticated devices may keep the socket open and reuse it for multiple requests. This is more
    // efficient, but requires more complicated logic. This function is also synchronous, which is less efficient but
    // simpler for this example.
    std::vector<std::string> _communicate(const std::string& text_request, const std::string& expected_response_op)
    {
        // Create and connect a socket stream
        boost::asio::ip::tcp::iostream s;
        s.connect(reynard_socket_endpoint);

        if (!s)
        {
            throw RR::ConnectionException("Could not connect to Reynard the Robot");
        }

        // Send the request text and newline
        s << text_request << "\n";

        // Read the response line
        std::string response;
        std::getline(s, response);
        boost::trim(response);

        // Check the response
        if (boost::starts_with(response, "ERROR"))
        {
            throw RR::OperationFailedException("Error from robot: " + response);
        }

        // OK has no return arguments
        if (expected_response_op == "OK")
        {
            if (response != "OK")
            {
                throw RR::OperationFailedException("Unexpected response from robot: " + response);
            }
            return std::vector<std::string>();
        }

        // Split the response into arguments
        std::vector<std::string> response_args;
        boost::split(response_args, response, boost::is_any_of(" "));
        if (response_args.size() == 0)
        {
            throw RR::OperationFailedException("Invalid response from robot: " + response);
        }

        // Check the operation name
        if (response_args[0] != expected_response_op)
        {
            throw RR::OperationFailedException("Unexpected response from robot: " + response);
        }

        std::vector<std::string> ret(response_args.begin() + 1, response_args.end());

        s.close();
        return ret;
    }

    RR::RRArrayPtr<double> get_robot_position() override
    {
        auto socket_res = _communicate("STATE", "STATE");

        auto ret = RR::AllocateRRArray<double>(2);
        ret->at(0) = boost::lexical_cast<double>(socket_res[0]) * 1.0e-3; // Convert to meters
        ret->at(1) = boost::lexical_cast<double>(socket_res[1]) * 1.0e-3; // Convert to meters
        return ret;
    }
    RR::RRArrayPtr<double> get_color() override
    {
        auto socket_res = _communicate("COLORGET", "COLOR");

        auto ret = RR::AllocateRRArray<double>(3);
        ret->at(0) = boost::lexical_cast<double>(socket_res[0]);
        ret->at(1) = boost::lexical_cast<double>(socket_res[1]);
        ret->at(2) = boost::lexical_cast<double>(socket_res[2]);
        return ret;
    }
    void set_color(const RR::RRArrayPtr<double>& value) override
    {
        if (value->size() != 3)
        {
            throw RR::InvalidArgumentException("Color must be a 3 element array");
        }

        std::stringstream ss;
        ss << "COLORSET " << value->at(0) << " " << value->at(1) << " " << value->at(2);
        _communicate(ss.str(), "OK");
    }

    void teleport(double x, double y) override
    {
        x = x * 1.0e3; // Convert to mm
        y = y * 1.0e3; // Convert to mm
        std::stringstream ss;
        ss << "TELEPORT " << x << " " << y;
        _communicate(ss.str(), "OK");
    }

    void setf_arm_position(double q1, double q2, double q3) override
    {
        q1 = q1 * (180.0 / M_PI); // Convert to degrees
        q2 = q2 * (180.0 / M_PI); // Convert to degrees
        q3 = q3 * (180.0 / M_PI); // Convert to degrees
        std::stringstream ss;
        ss << "SETARM " << q1 << " " << q2 << " " << q3;
        _communicate(ss.str(), "OK");
    }

    RR::RRArrayPtr<double> getf_arm_position() override
    {

        auto socket_res = _communicate("STATE", "STATE");

        double q1 = boost::lexical_cast<double>(socket_res[2]);
        double q2 = boost::lexical_cast<double>(socket_res[3]);
        double q3 = boost::lexical_cast<double>(socket_res[4]);

        auto ret = RR::AllocateRRArray<double>(3);
        ret->at(0) = q1 * (M_PI / 180.0); // Convert to radians
        ret->at(1) = q2 * (M_PI / 180.0); // Convert to radians
        ret->at(2) = q3 * (M_PI / 180.0); // Convert to radians
        return ret;
    }

    void drive_robot(double vel_x, double vel_y, double timeout, RobotRaconteur::rr_bool wait) override
    {
        vel_x = vel_x * 1.0e3; // Convert to mm/s
        vel_y = vel_y * 1.0e3; // Convert to mm/s

        std::stringstream ss;
        ss << "DRIVE " << vel_x << " " << vel_y << " " << timeout << " " << (wait.value ? "1" : "0");
        _communicate(ss.str(), "OK");
    }

    void drive_arm(double q1, double q2, double q3, double timeout, RobotRaconteur::rr_bool wait) override
    {
        q1 = q1 * (180.0 / M_PI); // Convert to degrees
        q2 = q2 * (180.0 / M_PI); // Convert to degrees
        q3 = q3 * (180.0 / M_PI); // Convert to degrees

        std::stringstream ss;
        ss << "DRIVEARM " << q1 << " " << q2 << " " << q3 << " " << timeout << " " << (wait.value ? "1" : "0");
        _communicate(ss.str(), "OK");
    }

    void say(const std::string& message) override
    {
        std::stringstream ss;
        ss << "SAY \"" << message << "\"";
        _communicate(ss.str(), "OK");
    }

    void _timer_cb(const RR::TimerEvent& ev)
    {
        try
        {
            auto socket_res = _communicate("STATE", "STATE");
            double t = boost::lexical_cast<double>(socket_res.at(0));
            double x = boost::lexical_cast<double>(socket_res.at(1));
            double y = boost::lexical_cast<double>(socket_res.at(2));
            double q1 = boost::lexical_cast<double>(socket_res.at(3));
            double q2 = boost::lexical_cast<double>(socket_res.at(4));
            double q3 = boost::lexical_cast<double>(socket_res.at(5));

            experimental::reynard_the_robot::ReynardStatePtr state(new experimental::reynard_the_robot::ReynardState());
            state->time = t;
            state->robot_position = RR::AllocateRRArray<double>(2);
            state->robot_position->at(0) = x * 1.0e-3; // Convert to meters
            state->robot_position->at(1) = y * 1.0e-3; // Convert to meters
            state->arm_position = RR::AllocateRRArray<double>(3);
            state->arm_position->at(0) = q1 * (M_PI / 180.0); // Convert to radians
            state->arm_position->at(1) = q2 * (M_PI / 180.0); // Convert to radians
            state->arm_position->at(2) = q3 * (M_PI / 180.0); // Convert to radians
            // Velocity is not available in the ASCII interface
            state->robot_velocity = RR::AllocateRRArray<double>(0);
            state->arm_velocity = RR::AllocateRRArray<double>(0);

            if (rrvar_state)
            {
                rrvar_state->SetOutValue(state);
            }
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_WARNING_COMPONENT(RR::RobotRaconteurNode::weak_sp(), UserService, 0,
                                                 "Error updating state: " << exp.what());
        }
    }

    void _start()
    {
        RR_WEAK_PTR<Reynard_impl> weak_this = shared_from_this();
        state_timer = RR::RobotRaconteurNode::s()->CreateTimer(
            boost::posix_time::milliseconds(250), [weak_this](const RR::TimerEvent& ev) {
                RR_SHARED_PTR<Reynard_impl> strong_this = weak_this.lock();
                if (!strong_this)
                    return;
                strong_this->_timer_cb(ev);
            });
        state_timer->Start();
    }

    void _close()
    {
        if (state_timer)
        {
            state_timer->TryStop();
            state_timer.reset();
        }
    }
};

int main(int argc, char* argv[])
{

    // Resolve the socket endpoint
    std::string reynard_host = "localhost";
    int reynard_port = 29202;
    boost::asio::io_context resolve_io_context;
    boost::asio::ip::tcp::resolver resolver(resolve_io_context);
    boost::asio::ip::tcp::resolver::query resolver_query(boost::asio::ip::tcp::v4(), reynard_host,
                                                         std::to_string(reynard_port));
    boost::asio::ip::tcp::resolver::iterator endpoints_iterator = resolver.resolve(resolver_query);
    if (endpoints_iterator == boost::asio::ip::tcp::resolver::iterator())
    {
        std::cerr << "Could not resolve Reynard the Robot host" << std::endl;
        return 1;
    }
    boost::asio::ip::tcp::endpoint reynard_endpoint = *endpoints_iterator;
    std::cout << "Connecting to Reynard the Robot ASCII socket at " << reynard_endpoint << std::endl;

    // Use RobotRaconteur::NodeSetup to initialize Robot Raconteur
    RR::ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "experimental.reynard_the_robot_cpp_socket", 59201,
                                   argc, argv);

    // Create the Reynard service instance
    auto reynard_obj = RR_MAKE_SHARED<Reynard_impl>(reynard_endpoint);

    // Register the service with Robot Raconteur
    auto ctx = RR::RobotRaconteurNode::s()->RegisterService("reynard", "experimental.reynard_the_robot", reynard_obj);

    // Start the service timer for updating the state
    reynard_obj->_start();

    // Print out some info for the user
    std::cout << "Reynard the Robot CPP Service Started" << std::endl << std::endl;

    std::cout << "Candidate connection urls:" << std::endl;
    ctx->PrintCandidateConnectionURLs();
    std::cout << std::endl;
    std::cout << "Press Ctrl-C to quit" << std::endl;

    // Use drekar_launch_process_cpp package to wait for exit
    drekar_launch_process_cpp::CWaitForExit wait_exit;
    wait_exit.WaitForExit();

    // Stop the service timer
    reynard_obj->_close();
}
