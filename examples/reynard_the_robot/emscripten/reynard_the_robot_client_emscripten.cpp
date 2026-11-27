// reynard_the_robot_client_emscripten.cpp - Reynard the Robot client using Emscripten

// This program is compiled to WebAssembly (WASM) using Emscripten and runs inside a web browser.
// The web browser is a single threaded environment, so all Robot Raconteur calls must use the
// "async" variants. Blocking calls such as ConnectService() or reynard->teleport() will fail.

#include <RobotRaconteur.h>
#include <RobotRaconteur/BrowserWebSocketTransport.h>
// Used by ROBOTRACONTEUR_SERVICE_TYPES. Normally included by NodeSetup.h, which is not available in Emscripten.
#include <boost/assign/list_of.hpp>
#include "robotraconteur_generated.h"

#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>

#include <boost/lexical_cast.hpp>
#include <sstream>
#include <string>

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;
namespace reynard_the_robot = experimental::reynard_the_robot;

// Helpers to access the page using embind emscripten::val, which calls the browser
// DOM API from C++. textContent is used instead of innerHTML so text received from
// the service is never interpreted as HTML.

using emscripten::val;

static val get_element(const std::string& id) { return val::global("document").call<val>("getElementById", id); }

static void set_element_text(const std::string& id, const std::string& text)
{
    get_element(id).set("textContent", text);
}

static void append_element_line(const std::string& id, const std::string& text)
{
    val document = val::global("document");
    val e = get_element(id);
    e.call<void>("appendChild", document.call<val>("createTextNode", text));
    e.call<void>("appendChild", document.call<val>("createElement", std::string("br")));
}

static std::string get_input_value(const std::string& id) { return get_element(id)["value"].as<std::string>(); }

static void log_error(const std::string& message) { append_element_line("error_log", "Error: " + message); }

// The connected Reynard client object reference. Use the async_Reynard interface
// for all calls that communicate with the service.
static RR_SHARED_PTR<reynard_the_robot::Reynard> reynard;
static RR_SHARED_PTR<reynard_the_robot::async_Reynard> async_reynard;

// Callback for the new_message event
static void new_message(const std::string& message) { append_element_line("received_messages", message); }

// Handler for async function calls that do not return a value
static void async_call_handler(const std::string& name, const RR_SHARED_PTR<RR::RobotRaconteurException>& err)
{
    if (err)
    {
        log_error(name + " failed: " + err->what());
    }
}

// Handle when the send_message button is clicked
static EM_BOOL send_message_clicked(int event_type, const EmscriptenMouseEvent* mouse_event, void* user_data)
{
    std::string message = get_input_value("message");
    async_reynard->async_say(message, boost::bind(&async_call_handler, "say", RR_BOOST_PLACEHOLDERS(_1)));
    return EM_TRUE;
}

// Handle when the teleport button is clicked
static EM_BOOL teleport_clicked(int event_type, const EmscriptenMouseEvent* mouse_event, void* user_data)
{
    double x = 0.0;
    double y = 0.0;
    try
    {
        // Positions are entered in millimeters, Reynard uses meters
        x = boost::lexical_cast<double>(get_input_value("teleport_x")) * 1e-3;
        y = boost::lexical_cast<double>(get_input_value("teleport_y")) * 1e-3;
    }
    catch (boost::bad_lexical_cast&)
    {
        log_error("Invalid teleport position");
        return EM_TRUE;
    }

    async_reynard->async_teleport(x, y, boost::bind(&async_call_handler, "teleport", RR_BOOST_PLACEHOLDERS(_1)));
    return EM_TRUE;
}

static void schedule_update_position();

// Handler for the wire peek. Displays the position and schedules the next update.
static void peek_state_handler(const reynard_the_robot::ReynardStatePtr& state, const RR::TimeSpec& ts,
                               const RR_SHARED_PTR<RR::RobotRaconteurException>& err)
{
    if (err)
    {
        set_element_text("status", "Error");
        log_error(std::string("Reading state failed: ") + err->what());
    }
    else
    {
        std::stringstream ss;
        ss << "x: " << state->robot_position->at(0) << ", y: " << state->robot_position->at(1);
        set_element_text("reynard_position", ss.str());
    }
    schedule_update_position();
}

// Read the current state using a wire "peek". Can also "connect" to receive streaming updates.
static void update_position(void* user_data) { reynard->get_state()->AsyncPeekInValue(&peek_state_handler); }

// Use a browser timeout to update the position every 100 ms. The next update is scheduled
// after the previous peek completes so requests do not pile up.
static void schedule_update_position() { emscripten_set_timeout(&update_position, 100, NULL); }

// Handler for AsyncConnectService
static void connect_handler(const RR_SHARED_PTR<RR::RRObject>& obj,
                            const RR_SHARED_PTR<RR::RobotRaconteurException>& err)
{
    if (err)
    {
        set_element_text("status", "Error");
        log_error(std::string("Connect failed: ") + err->what());
        return;
    }

    reynard = RR::rr_cast<reynard_the_robot::Reynard>(obj);
    async_reynard = RR::rr_cast<reynard_the_robot::async_Reynard>(obj);

    // Connect a callback function to listen for new messages
    reynard->get_new_message().connect(&new_message);

    emscripten_set_click_callback("#send_message", NULL, EM_FALSE, &send_message_clicked);
    emscripten_set_click_callback("#teleport", NULL, EM_FALSE, &teleport_clicked);

    set_element_text("status", "Connected");

    schedule_update_position();
}

int main(int argc, char* argv[])
{
    RR_SHARED_PTR<RR::RobotRaconteurNode> node = RR::RobotRaconteurNode::sp();

    node->SetLogLevel(RR::RobotRaconteur_LogLevel_Info);

    // The browser WebSocket transport is the only transport available in the browser.
    // It only supports client connections.
    RR_SHARED_PTR<RR::BrowserWebSocketTransport> transport = RR_MAKE_SHARED<RR::BrowserWebSocketTransport>(node);
    node->RegisterTransport(transport);

    // Register the generated service types
    std::vector<RR_SHARED_PTR<RR::ServiceFactory> > service_types = ROBOTRACONTEUR_SERVICE_TYPES;
    for (size_t i = 0; i < service_types.size(); i++)
    {
        node->RegisterServiceType(service_types[i]);
    }

    set_element_text("status", "Connecting...");

    // Connect to Reynard the Robot service using a WebSocket. rr+tcp:// URLs are converted to ws://.
    std::string url = "rr+tcp://localhost:29200?service=reynard";
    node->AsyncConnectService(url, "", RR_INTRUSIVE_PTR<RR::RRMap<std::string, RR::RRValue> >(), NULL,
                              "experimental.reynard_the_robot.Reynard", &connect_handler);

    // Returning from main() does not stop the program. The Emscripten runtime stays alive
    // and the callbacks continue to run from the browser event loop.
    return 0;
}
