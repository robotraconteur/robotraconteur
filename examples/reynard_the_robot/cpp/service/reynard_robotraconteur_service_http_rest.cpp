// C++ example Robot Raconteur service for Reynard the Robot using the HTTP REST API to communicate with the robot.

// Note that Reynard the Robot uses mm and degrees, while Robot Raconteur services are expected
// to use meters and radians. The service will convert the units as needed.

#include <stdio.h>
#include <iostream>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

#include <boost/json/src.hpp>
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

    boost::json::value _get_json(const std::string& path)
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

        boost::json::value ret = res.body();

        stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);

        return ret;
    }

    boost::json::value _post_json(const std::string& path, const boost::json::value& obj)
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

        boost::json::value ret;
        try
        {
            boost::beast::http::read(stream, buffer, res);
            ret = res.body();
        }
        catch (const boost::system::system_error& e)
        {
            if (e.code() != boost::json::error::incomplete)
            {
                // incomplete_json is expected if the response is empty
                throw;
            }
        }

        stream.socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both);

        return ret;
    }

    RR::RRArrayPtr<double > get_robot_position() override
    {
        auto json_ret = _get_json("/api/state");
        double x = json_ret.at("x").as_double();
        double y = json_ret.at("y").as_double();

        auto ret = RR::AllocateRRArray<double>(2);
        ret->at(0) = x * 1.0e-3; // Convert to meters 
        ret->at(1) = y * 1.0e-3; // Convert to meters
        return ret;
    }
    RR::RRArrayPtr<double > get_color() override
    {
        auto json_ret = _get_json("/api/color");
        double r = json_ret.at("r").as_double();
        double g = json_ret.at("g").as_double();
        double b = json_ret.at("b").as_double();

        auto ret = RR::AllocateRRArray<double>(3);
        ret->at(0) = r;
        ret->at(1) = g;
        ret->at(2) = b;
        return ret;
    }
    void set_color(const RR::RRArrayPtr<double >& value) override
    {
        if (value->size() != 3)
        {
            throw RR::InvalidArgumentException("Color must be a 3 element array");
        }

        double r = value->at(0);
        double g = value->at(1);
        double b = value->at(2);

        boost::json::value obj = boost::json::object{
            {"r", r},
            {"g", g},
            {"b", b}
        };
    }

    void teleport(double x, double y) override
    {
        x = x * 1.0e3; // Convert to mm
        y = y * 1.0e3; // Convert to mm
        boost::json::value obj = boost::json::object{
            {"x", x},
            {"y", y}
        };

        _post_json("/api/teleport", obj);
    }

    void setf_arm_position(double q1, double q2, double q3) override
    {
        q1 = q1 * (180.0 / M_PI); // Convert to degrees
        q2 = q2 * (180.0 / M_PI); // Convert to degrees
        q3 = q3 * (180.0 / M_PI); // Convert to degrees
        boost::json::value obj = boost::json::object{
            {"q1", q1},
            {"q2", q2},
            {"q3", q3}
        };

        _post_json("/api/arm", obj);
    }

    RR::RRArrayPtr<double > getf_arm_position() override
    {
        auto json_ret = _get_json("/api/state");
        double q1 = json_ret.at("q1").as_double();
        double q2 = json_ret.at("q2").as_double();
        double q3 = json_ret.at("q3").as_double();

        auto ret = RR::AllocateRRArray<double>(3);
        ret->at(0) = q1 * (M_PI / 180.0); // Convert to radians
        ret->at(1) = q2 * (M_PI / 180.0); // Convert to radians
        ret->at(2) = q3 * (M_PI / 180.0); // Convert to radians
        return ret;
    }

    void drive_robot(double vel_x, double vel_y, double timeout, RobotRaconteur::rr_bool wait) override
    {
        if (timeout > 0 || wait.value)
        {
            throw RR::OperationFailedException("Timeout not supported");
        }

        vel_x = vel_x * 1.0e3; // Convert to mm/s
        vel_y = vel_y * 1.0e3; // Convert to mm/s
        boost::json::value obj = boost::json::object{
            {"vel_x", vel_x},
            {"vel_y", vel_y}
        };

        _post_json("/api/drive_robot", obj);
    }

    void drive_arm(double q1, double q2, double q3, double timeout, RobotRaconteur::rr_bool wait) override
    {
        if (timeout > 0 || wait.value)
        {
            throw RR::OperationFailedException("Timeout not supported");
        }

        q1 = q1 * (180.0 / M_PI); // Convert to degrees
        q2 = q2 * (180.0 / M_PI); // Convert to degrees
        q3 = q3 * (180.0 / M_PI); // Convert to degrees

        boost::json::value obj = boost::json::object{
            {"q1", q1},
            {"q2", q2},
            {"q3", q3}
        };

        _post_json("/api/drive_arm", obj);
    }

    void say(const std::string& message) override
    {
        boost::json::value obj = boost::json::object{
            {"message", message}
        };

        _post_json("/api/say", obj);
    }


    void _timer_cb(const RR::TimerEvent& ev)
    {   
        try
        {
            auto json_ret = _get_json("/api/state");
            // JSON response only contains the robot position and arm position
            double x = json_ret.at("x").as_double();
            double y = json_ret.at("y").as_double();
            double q1 = json_ret.at("q1").as_double();
            double q2 = json_ret.at("q2").as_double();
            double q3 = json_ret.at("q3").as_double();
            
            experimental::reynard_the_robot::ReynardStatePtr state(new experimental::reynard_the_robot::ReynardState());
            state->robot_position = RR::AllocateRRArray<double>(2);
            state->robot_position->at(0) = x * 1.0e-3; // Convert to meters
            state->robot_position->at(1) = y * 1.0e-3; // Convert to meters
            state->arm_position = RR::AllocateRRArray<double>(3);
            state->arm_position->at(0) = q1 * (M_PI / 180.0); // Convert to radians
            state->arm_position->at(1) = q2 * (M_PI / 180.0); // Convert to radians
            state->arm_position->at(2) = q3 * (M_PI / 180.0); // Convert to radians
            // Velocity is not available in the JSON API

            if (rrvar_state)
            {
                rrvar_state->SetOutValue(state);
            }
        }
        catch (std::exception& exp)
        {
            ROBOTRACONTEUR_LOG_WARNING_COMPONENT(RR::RobotRaconteurNode::weak_sp(), UserService, 0, "Error updating state: " << exp.what());
        }
        
    }

    void _start()
    {
        RR_WEAK_PTR<Reynard_impl> weak_this = shared_from_this();
        state_timer = RR::RobotRaconteurNode::s()->CreateTimer(boost::posix_time::milliseconds(250), [weak_this](const RR::TimerEvent& ev)
        {
            RR_SHARED_PTR<Reynard_impl> strong_this = weak_this.lock();
            if (!strong_this) return;
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
    RR::ServerNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, "experimental.reynard_the_robot_cpp_rest", 59201, argc, argv);

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
    std::cout <<  "Press Ctrl-C to quit" << std::endl;

    // Use drekar_launch_process_cpp package to wait for exit
    drekar_launch_process_cpp::CWaitForExit wait_exit;
    wait_exit.WaitForExit();

    // Stop the service timer
    reynard_obj->_close();

}