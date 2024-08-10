// null_value_types.cpp - Example of types that can be null

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

    // Null values
    // Null values are "nullptr" or empty intrusive_ptr
    // struct, containers, and varvalue are nullable.
    // Numbers, arrays, strings, pods, and namedarrays are not nullable.

    // property MyStructure t_struct_null
    vt::MyStructurePtr s = nullptr;
    c->set_t_struct_null(s);
    assert(c->get_t_struct_null() == nullptr);
}
