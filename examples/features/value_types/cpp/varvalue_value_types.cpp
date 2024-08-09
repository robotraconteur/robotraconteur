// varvalue_value_types.cpp - Example of using varvalue wildcard type

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

    // Any valid Robot Raconteur data type can be stored in a varvalue

    // property varvalue p_varvalue_double_array
    // Example of storing a double array in a varvalue
    RR::RRArrayPtr<double> a = RR::VectorToRRArray<double, double>({1.1, 2.2, 3.3});
    c->set_p_varvalue_double_array(a);
    RR::RRArrayPtr<double> b = RR::rr_cast<RR::RRArray<double> >(c->get_p_varvalue_double_array());
    // varvalue can be null
    RR::rr_null_check(b);
    assert_array(b, {1.4, 2.5, 3.6});

    // property varvalue q_varvalue_string
    // Example of storing a string in a varvalue
    c->set_q_varvalue_string(RR::stringToRRArray("varvalue string from client"));
    RR::RRArrayPtr<char> d = RR::rr_cast<RR::RRArray<char> >(c->get_q_varvalue_string());
    // varvalue can be null
    RR::rr_null_check(d);
    std::string d_str = RR::RRArrayToString(d);
    assert(d_str == "varvalue string from service");

    // property varvalue r_varvalue_struct
    // Example of storing a structure in a varvalue
    // var s = new MyStructure() { a = 5, b = new uint[] { 10, 20 }, c = "String from structure client",
    //                            d = new List<string>() { "string a", "string b" } };
    vt::MyStructurePtr s(new vt::MyStructure());
    s->a = 5;
    s->b = RR::VectorToRRArray<uint32_t, uint32_t>({10, 20});
    s->c = "String from structure client";
    RR::RRListPtr<RR::RRArray<char> > d1 = RR::AllocateEmptyRRList<RR::RRArray<char> >();
    d1->push_back(RR::stringToRRArray("string a"));
    d1->push_back(RR::stringToRRArray("string b"));
    s->d = d1;
    c->set_r_varvalue_struct(s);

    vt::MyStructurePtr t = RR::rr_cast<vt::MyStructure>(c->get_r_varvalue_struct());
    // varvalue can be null
    RR::rr_null_check(t);
    assert(t->a == 52);
    assert_array(t->b, {110, 120});
    assert(t->c == "String from structure service");
    RR::rr_null_check(t->d);
    assert(t->d->size() == 3);
    auto d2_iterator = t->d->begin();
    assert(RR::RRArrayToString(*d2_iterator++) == "string c");
    assert(RR::RRArrayToString(*d2_iterator++) == "string d");
    assert(RR::RRArrayToString(*d2_iterator++) == "string e");

    // property varvalue{string} s_varvalue_map2
    // Example of storing a map of strings in a varvalue
    // varvalue{string} is used extensively in structures
    // to allow for additional fields to be added to the
    // structure without changing the structure definition
    RR::RRMapPtr<std::string, RR::RRValue> s_varvalue_map_2 = RR::AllocateEmptyRRMap<std::string, RR::RRValue>();
    s_varvalue_map_2->insert(std::make_pair("key1", RR::VectorToRRArray<int32_t, int32_t>({2, 3})));
    s_varvalue_map_2->insert(std::make_pair("key2", RR::stringToRRArray("string 2")));
    c->set_s_varvalue_map2(s_varvalue_map_2);

    RR::RRMapPtr<std::string, RR::RRValue> u = c->get_s_varvalue_map2();
    // varvalue can be null
    RR::rr_null_check(u);
    RR::rr_null_check(u->at("key3"));
    RR::rr_null_check(u->at("key4"));
    assert_array(RR::rr_cast<RR::RRArray<int32_t> >(u->at("key3")), {4, 5});
    assert(RR::RRArrayToString(RR::rr_cast<RR::RRArray<char> >(u->at("key4"))) == "string 4");

    std::cout << "varvalue_value_types.cpp example complete" << std::endl;
    return 0;
}
