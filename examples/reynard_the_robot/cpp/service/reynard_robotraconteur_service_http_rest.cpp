// C++ example Robot Raconteur service for Reynard the Robot using the HTTP REST API to communicate with the robot.

// Note that Reynard the Robot uses mm and degrees, while Robot Raconteur services are expected
// to use meters and radians. The service will convert the units as needed.

#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

#include <nlohmann/json.hpp>
#include "json_body.hpp"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>

#include "drekar_launch_process_cpp/drekar_launch_process_cpp.h"

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

class Reynard_impl : public virtual experimental::reynard_the_robot::Reynard_default_impl,
                     public RR_ENABLE_SHARED_FROM_THIS<Reynard_impl>
{
  protected:
    std::string hostname;
    int32_t port;
    RR::TimerPtr state_timer;

  public:
    Reynard_impl(const std::string& hostname, int32_t port)
    {
        this->hostname = hostname;
        this->port = port;
    }

    nlohmann::json _get_json(const std::string& path)
    {
        // Based on https://www.boost.org/doc/libs/1_85_0/libs/beast/example/http/client/body/json_client.cpp
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::resolver resolver(ioc);
        boost::beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(hostname, std::to_string(port));
        stream.connect(results);

        boost::beast::http::request<boost::beast::http::empty_body> req{boost::beast::http::verb::get, path, 11};
        req.set(boost::beast::http::field::host, hostname);
        req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        boost::beast::http::write(stream, req);

        boost::beast::flat_buffer buffer;
        boost::beast::http::response<json_body> res;
        boost::beast::http::read(stream, buffer, res);

        nlohmann::json ret = res.body();

        stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);

        return ret;
    }

    nlohmann::json _post_json(const std::string& path, const nlohmann::json& obj)
    {
        // Based on https://www.boost.org/doc/libs/1_85_0/libs/beast/example/http/client/body/json_client.cpp
        boost::asio::io_context ioc;
        boost::asio::ip::tcp::resolver resolver(ioc);
        boost::beast::tcp_stream stream(ioc);

        auto const results = resolver.resolve(hostname, std::to_string(port));
        stream.connect(results);

        // Set up an HTTP POST request message
        boost::beast::http::request<json_body> req{boost::beast::http::verb::post, path, 11};
        req.set(boost::beast::http::field::host, hostname);
        req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(boost::beast::http::field::content_type, "application/json");
        req.body() = obj;
        req.prepare_payload();
        // Send the HTTP request to the remote host
        boost::beast::http::write(stream, req);

        boost::beast::flat_buffer buffer;
        boost::beast::http::response<json_body> res;

        nlohmann::json ret;

        boost::beast::http::read(stream, buffer, res);
        ret = res.body();

        stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);

        return ret;
    }

    RR::RRArrayPtr<double> get_robot_position() override
    {
        auto json_ret = _get_json("/api/state");
        double x = json_ret.at("x").get<double>();
        double y = json_ret.at("y").get<double>();

        auto ret = RR::AllocateRRArray<double>(2);
        ret->at(0) = x * 1.0e-3; // Convert to meters
        ret->at(1) = y * 1.0e-3; // Convert to meters
        return ret;
    }
    RR::RRArrayPtr<double> get_color() override
    {
        auto json_ret = _get_json("/api/color");
        double r = json_ret.at("r").get<double>();
        double g = json_ret.at("g").get<double>();
        double b = json_ret.at("b").get<double>();

        auto ret = RR::AllocateRRArray<double>(3);
        ret->at(0) = r;
        ret->at(1) = g;
        ret->at(2) = b;
        return ret;
    }
    void set_color(const RR::RRArrayPtr<double>& value) override
    {
        if (value->size() != 3)
        {
            throw RR::InvalidArgumentException("Color must be a 3 element array");
        }

        double r = value->at(0);
        double g = value->at(1);
        double b = value->at(2);

        nlohmann::json obj = {{"r", r}, {"g", g}, {"b", b}};

        _post_json("/api/color", obj);
    }

    void teleport(double x, double y) override
    {
        x = x * 1.0e3; // Convert to mm
        y = y * 1.0e3; // Convert to mm
        nlohmann::json obj = {{"x", x}, {"y", y}};

        _post_json("/api/teleport", obj);
    }

    void setf_arm_position(double q1, double q2, double q3) override
    {
        q1 = q1 * (180.0 / M_PI); // Convert to degrees
        q2 = q2 * (180.0 / M_PI); // Convert to degrees
        q3 = q3 * (180.0 / M_PI); // Convert to degrees
        nlohmann::json obj = {{"q1", q1}, {"q2", q2}, {"q3", q3}};

        _post_json("/api/arm", obj);
    }

    RR::RRArrayPtr<double> getf_arm_position() override
    {
        auto json_ret = _get_json("/api/state");
        double q1 = json_ret.at("q1").get<double>();
        double q2 = json_ret.at("q2").get<double>();
        double q3 = json_ret.at("q3").get<double>();

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
        nlohmann::json obj = {{"vel_x", vel_x}, {"vel_y", vel_y}, {"timeout", timeout}, {"wait", wait.value != 0}};

        _post_json("/api/drive_robot", obj);
    }

    void drive_arm(double q1, double q2, double q3, double timeout, RobotRaconteur::rr_bool wait) override
    {
        q1 = q1 * (180.0 / M_PI); // Convert to degrees
        q2 = q2 * (180.0 / M_PI); // Convert to degrees
        q3 = q3 * (180.0 / M_PI); // Convert to degrees

        nlohmann::json obj = {{"q1", q1}, {"q2", q2}, {"q3", q3}, {"timeout", timeout}, {"wait", wait.value != 0}};

        _post_json("/api/drive_arm", obj);
    }

    void say(const std::string& message) override
    {
        nlohmann::json obj = {{"message", message}};

        _post_json("/api/say", obj);
    }

    void _timer_cb(const RR::TimerEvent& ev)
    {

        // Many HTTP devices (such as ABB robot controllers) provide a WebSocket to provide asynchronous updates.
        // This is not available in the Reynard the Robot API, so we must poll the state.
        try
        {
            auto json_ret = _get_json("/api/state");
            // JSON response only contains the robot position and arm position
            double t = json_ret.at("time").get<double>();
            double x = json_ret.at("x").get<double>();
            double y = json_ret.at("y").get<double>();
            double q1 = json_ret.at("q1").get<double>();
            double q2 = json_ret.at("q2").get<double>();
            double q3 = json_ret.at("q3").get<double>();
            double vel_x = json_ret.at("vel_x").get<double>();
            double vel_y = json_ret.at("vel_y").get<double>();
            double vel_q1 = json_ret.at("vel_q1").get<double>();
            double vel_q2 = json_ret.at("vel_q2").get<double>();
            double vel_q3 = json_ret.at("vel_q3").get<double>();

            experimental::reynard_the_robot::ReynardStatePtr state(new experimental::reynard_the_robot::ReynardState());
            state->time = t;
            state->robot_position = RR::AllocateRRArray<double>(2);
            state->robot_position->at(0) = x * 1.0e-3; // Convert to meters
            state->robot_position->at(1) = y * 1.0e-3; // Convert to meters
            state->arm_position = RR::AllocateRRArray<double>(3);
            state->arm_position->at(0) = q1 * (M_PI / 180.0); // Convert to radians
            state->arm_position->at(1) = q2 * (M_PI / 180.0); // Convert to radians
            state->arm_position->at(2) = q3 * (M_PI / 180.0); // Convert to radians
            state->robot_velocity = RR::AllocateRRArray<double>(2);
            state->robot_velocity->at(0) = vel_x * 1.0e-3; // Convert to m/s
            state->robot_velocity->at(1) = vel_y * 1.0e-3; // Convert to m/s
            state->arm_velocity = RR::AllocateRRArray<double>(3);
            state->arm_velocity->at(0) = vel_q1 * (M_PI / 180.0); // Convert to rad/s
            state->arm_velocity->at(1) = vel_q2 * (M_PI / 180.0); // Convert to rad/s
            state->arm_velocity->at(2) = vel_q3 * (M_PI / 180.0); // Convert to rad/s

            if (rrvar_state)
            {
                rrvar_state->SetOutValue(state);
            }

            auto message_json_ret = _get_json("/api/messages");
            for (auto& message : message_json_ret)
            {
                std::string message_str = message.get<std::string>();
                rrvar_new_message(message_str);
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
    // Use RobotRaconteur::NodeSetup to initialize Robot Raconteur
    RR::ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "experimental.reynard_the_robot_cpp_rest", 59201, argc,
                                   argv);

    // Create the Reynard service instance
    auto reynard_obj = RR_MAKE_SHARED<Reynard_impl>("localhost", 29201);

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
