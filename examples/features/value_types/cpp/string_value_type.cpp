// string_value_type.cpp - Example of using string value types

#include <stdio.h>
#include <iostream>
#include <boost/range/algorithm.hpp>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;
namespace vt = experimental::value_types;

int main(int argc, char* argv[])
{
    RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

    vt::ValueTypesExamplePtr c = RR::rr_cast<vt::ValueTypesExample>(
        RR::RobotRaconteurNode::s()->ConnectService("rr+tcp://localhost:53223?service=values_example"));

    // Strings
    // property string f_string
    std::string f_string_read = c->get_f_string();
    std::cout << f_string_read << std::endl;
    // assert is used for the rest of the example property read operations
    // to demonstrate the expected values.
    assert(c->get_f_string() == "An example string read from the service");
    c->set_f_string("An example string written to the service");

    std::cout << "string_value_type.cpp example complete" << std::endl;
    return 0;
}
