// struct_value_type.cpp - Simple example of using struct value types

#include <stdio.h>
#include <iostream>
#include <boost/range/algorithm.hpp>
#include <RobotRaconteur.h>
#include "robotraconteur_generated.h"

// Only use the RR alias in cpp files. Do not use it in header files.
namespace RR = RobotRaconteur;
namespace vt = experimental::value_types;

template <typename T>
void assert_array(const RR::RRArrayPtr<T>& a, const std::vector<T>& b)
{
    assert(a->size() == b.size());
    // RRArray supports standard C++ iterator interface.
    // The intrusive_ptr smart pointer
    // is dereferenced to access the underlying RRArray.
    assert(boost::range::equal(*a, b));
}

int main(int argc, char* argv[])
{
    RR::ClientNodeSetup node_setup(ROBOTRACONTEUR_SERVICE_TYPES, argc, argv);

    vt::ValueTypesExamplePtr c = RR::rr_cast<vt::ValueTypesExample>(
        RR::RobotRaconteurNode::s()->ConnectService("rr+tcp://localhost:53223?service=values_example"));

    // Create and populate a MyStructure
    vt::MyStructurePtr s(new vt::MyStructure());
    // field double a
    s->a = 5;
    // field uint32[2] b
    s->b = RR::VectorToRRArray<uint32_t, uint32_t>({10, 20});
    // field string c
    s->c = "String from structure client";
    // field string{list} d
    // Strings are stored in RRArray<char> when stored in containers
    RR::RRListPtr<RR::RRArray<char> > d1 = RR::AllocateEmptyRRList<RR::RRArray<char> >();
    d1->push_back(RR::stringToRRArray("string a"));
    d1->push_back(RR::stringToRRArray("string b"));
    s->d = d1;

    // Set the property using the structure
    // property MyStructure k_struct
    c->set_k_struct(s);

    // Retrieve the structure from the service
    vt::MyStructurePtr u = c->get_k_struct();

    // Structures can be None
    RR::rr_null_check(u);

    // field double a
    assert(u->a == 52);
    // field uint32[2] b
    assert_array(u->b, {110, 120});
    // field string c
    assert(u->c == "String from structure service");
    // field string{list} d
    // Containers can be null
    RR::rr_null_check(u->d);
    assert(u->d->size() == 3);
    auto d2_iterator = u->d->begin();
    assert(RR::RRArrayToString(*d2_iterator++) == "string c");
    assert(RR::RRArrayToString(*d2_iterator++) == "string d");
    assert(RR::RRArrayToString(*d2_iterator++) == "string e");

    std::cout << "struct_value_types.cpp example complete" << std::endl;
    return 0;
}
