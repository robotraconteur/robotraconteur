// container_value_types.cpp - Example of using container value types (maps and lists)

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

    // property double{int32} l_double_map
    // int32 keyed map of scalar doubles
    RR::RRMapPtr<int32_t, RR::RRArray<double> > l_double_map1 = RR::AllocateEmptyRRMap<int32_t, RR::RRArray<double> >();
    l_double_map1->insert(std::make_pair(1, RR::ScalarToRRArray<double>(1.1)));
    l_double_map1->insert(std::make_pair(2, RR::ScalarToRRArray<double>(2.2)));
    l_double_map1->insert(std::make_pair(5, RR::ScalarToRRArray<double>(3.3)));
    c->set_l_double_map(l_double_map1);

    RR::RRMapPtr<int32_t, RR::RRArray<double> > b = c->get_l_double_map();
    // Containers can be null
    RR::rr_null_check(b);
    assert(b->size() == 2);
    // Container elements can be null
    RR::rr_null_check(b->at(1));
    RR::rr_null_check(b->at(5));
    assert_array(b->at(1), {5.1});
    assert_array(b->at(5), {6.2});

    // property double[]{string} l_double_array_map
    // string keyed map of double arrays
    RR::RRMapPtr<std::string, RR::RRArray<double> > l_double_array_map1 =
        RR::AllocateEmptyRRMap<std::string, RR::RRArray<double> >();
    l_double_array_map1->insert(std::make_pair("key1", RR::VectorToRRArray<double, double>({1.1, 1.2, 1.3})));
    l_double_array_map1->insert(std::make_pair("key2", RR::VectorToRRArray<double, double>({2.1, 2.2, 2.3})));
    c->set_l_double_array_map(l_double_array_map1);

    RR::RRMapPtr<std::string, RR::RRArray<double> > l_double_array_map2 = c->get_l_double_array_map();
    RR::rr_null_check(l_double_array_map2);
    assert(l_double_array_map2->size() == 2);
    RR::rr_null_check(l_double_array_map2->at("key3"));
    RR::rr_null_check(l_double_array_map2->at("key4"));
    assert_array(l_double_array_map2->at("key3"), {5.1, 5.2, 5.3});
    assert_array(l_double_array_map2->at("key4"), {6.1, 6.2, 6.3});

    // property string{list} m_string_list
    // list of strings
    RR::RRListPtr<RR::RRArray<char> > m_string_list1 = RR::AllocateEmptyRRList<RR::RRArray<char> >();
    m_string_list1->push_back(RR::stringToRRArray("string 1"));
    m_string_list1->push_back(RR::stringToRRArray("string 2"));
    m_string_list1->push_back(RR::stringToRRArray("string 3"));
    c->set_m_string_list(m_string_list1);

    RR::RRListPtr<RR::RRArray<char> > a = c->get_m_string_list();
    // Containers can be null
    RR::rr_null_check(a);
    assert(a->size() == 2);
    // Container elements can be null
    auto a_iterator = a->begin();
    RR::rr_null_check(*a_iterator);
    assert(RR::RRArrayToString(*a_iterator++) == "string 4");
    RR::rr_null_check(*a_iterator);
    assert(RR::RRArrayToString(*a_iterator++) == "string 5");

    // property string{int32} m_string_map_int32
    // int32 keyed map of strings
    RR::RRMapPtr<int32_t, RR::RRArray<char> > m_string_map_int32_1 =
        RR::AllocateEmptyRRMap<int32_t, RR::RRArray<char> >();
    m_string_map_int32_1->insert(std::make_pair(12, RR::stringToRRArray("string 1")));
    m_string_map_int32_1->insert(std::make_pair(100, RR::stringToRRArray("string 2")));
    m_string_map_int32_1->insert(std::make_pair(17, RR::stringToRRArray("string 3")));
    c->set_m_string_map_int32(m_string_map_int32_1);

    RR::RRMapPtr<int32_t, RR::RRArray<char> > e = c->get_m_string_map_int32();
    // Containers can be null
    RR::rr_null_check(e);
    assert(e->size() == 2);
    // Container elements can be null
    RR::rr_null_check(e->at(87));
    RR::rr_null_check(e->at(1));
    assert(RR::RRArrayToString(e->at(87)) == "string 4");
    assert(RR::RRArrayToString(e->at(1)) == "string 5");

    // property string{string} m_string_map_string
    // string keyed map of strings
    RR::RRMapPtr<std::string, RR::RRArray<char> > m_string_map_string_1 =
        RR::AllocateEmptyRRMap<std::string, RR::RRArray<char> >();
    m_string_map_string_1->insert(std::make_pair("key1", RR::stringToRRArray("string 1")));
    m_string_map_string_1->insert(std::make_pair("key2", RR::stringToRRArray("string 2")));
    c->set_m_string_map_string(m_string_map_string_1);

    RR::RRMapPtr<std::string, RR::RRArray<char> > f = c->get_m_string_map_string();
    // Containers can be null
    RR::rr_null_check(f);
    assert(f->size() == 3);
    // Container elements can be null
    RR::rr_null_check(f->at("key3"));
    RR::rr_null_check(f->at("key4"));
    RR::rr_null_check(f->at("key5"));
    assert(RR::RRArrayToString(f->at("key3")) == "string 3");
    assert(RR::RRArrayToString(f->at("key4")) == "string 4");
    assert(RR::RRArrayToString(f->at("key5")) == "string 5");

    // property MyVector3{int32} n_vector_map
    // int32 keyed map of MyVector3
    vt::MyVector3 g; // = new MyVector3() { x = 1.0, y = 2.0, z = 3.0 };
    g.s.x = 1.0;
    g.s.y = 2.0;
    g.s.z = 3.0;
    RR::RRMapPtr<int32_t, RR::RRNamedArray<vt::MyVector3> > n_vector_map_1 =
        RR::AllocateEmptyRRMap<int32_t, RR::RRNamedArray<vt::MyVector3> >();
    n_vector_map_1->insert(std::make_pair(1, RR::ScalarToRRNamedArray(g)));
    c->set_n_vector_map(n_vector_map_1);

    RR::RRMapPtr<int32_t, RR::RRNamedArray<vt::MyVector3> > g1 = c->get_n_vector_map();
    // Containers can be null
    RR::rr_null_check(g1);
    assert(g1->size() == 1);
    // Container elements can be null
    RR::rr_null_check(g1->at(1));
    vt::MyVector3 g2 = RR::RRNamedArrayToScalar(g1->at(1));
    assert(g2.s.x == 4.0);
    assert(g2.s.y == 5.0);
    assert(g2.s.z == 6.0);

    // property MyStructure{list} o_struct_list
    // list of MyStructure
    vt::MyStructurePtr h1(new vt::MyStructure());
    h1->a = 5;
    h1->b = RR::VectorToRRArray<uint32_t, uint32_t>({10, 20});
    h1->c = "String from structure client";
    RR::RRListPtr<RR::RRArray<char> > h1_d = RR::AllocateEmptyRRList<RR::RRArray<char> >();
    h1_d->push_back(RR::stringToRRArray("string a"));
    h1_d->push_back(RR::stringToRRArray("string b"));
    h1->d = h1_d;
    RR::RRListPtr<vt::MyStructure> h2 = RR::AllocateEmptyRRList<vt::MyStructure>();
    h2->push_back(h1);
    c->set_o_struct_list(h2);

    RR::RRListPtr<vt::MyStructure> i = c->get_o_struct_list();
    // Containers can be null
    RR::rr_null_check(i);
    assert(i->size() == 1);
    // Container elements can be null
    RR::rr_null_check(*(i->begin()));
    vt::MyStructurePtr i0 = *(i->begin());
    assert(i0->a == 52);
    assert_array(i0->b, {110, 120});
    assert(i0->c == "String from structure service");
    // Containers can be null
    RR::rr_null_check(i0->d);
    assert(i0->d->size() == 3);
    auto i0_d_iterator = i0->d->begin();
    assert(RR::RRArrayToString(*i0_d_iterator++) == "string c");
    assert(RR::RRArrayToString(*i0_d_iterator++) == "string d");
    assert(RR::RRArrayToString(*i0_d_iterator++) == "string e");

    std::cout << "container_value_types.cpp example complete" << std::endl;
    return 0;
}
