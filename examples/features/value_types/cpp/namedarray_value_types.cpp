// namedarray_value_types.cpp - Example of using namedarray value types

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

    // property MyVector3 g_vector
    vt::MyVector3 c1;
    c1.s.x = 1.0;
    c1.s.y = 2.0;
    c1.s.z = 3.0;
    c->set_g_vector(c1);
    vt::MyVector3 b = c->get_g_vector();
    assert(b.s.x == 4.0);
    assert(b.s.y == 5.0);
    assert(b.s.z == 6.0);

    // Now an array of NamedArrays
    // property MyVector3[] g_vector_array
    RR::RRNamedArrayPtr<vt::MyVector3> f = RR::AllocateEmptyRRNamedArray<vt::MyVector3>(2);
    f->at(0).s.x = 1.0;
    f->at(0).s.y = 2.0;
    f->at(0).s.z = 3.0;
    f->at(1).s.x = 4.0;
    f->at(1).s.y = 5.0;
    f->at(1).s.z = 6.0;

    c->set_g_vector_array(f);

    RR::RRNamedArrayPtr<vt::MyVector3> g = c->get_g_vector_array();
    assert(g->size() == 2);
    assert(g->at(0).s.x == 7.0);
    assert(g->at(0).s.y == 8.0);
    assert(g->at(0).s.z == 9.0);
    assert(g->at(1).s.x == 10.0);
    assert(g->at(1).s.y == 11.0);
    assert(g->at(1).s.z == 12.0);

    // Use plain arrays and convert to and from NamedArrays
    vt::MyVector3 d;
    // d.a has type double[3]
    d.a[0] = 1.0;
    d.a[1] = 2.0;
    d.a[2] = 3.0;
    c->set_g_vector(d);
    vt::MyVector3 e = c->get_g_vector();
    assert(e.a[0] == 4.0);
    assert(e.a[1] == 5.0);
    assert(e.a[2] == 6.0);

    // property MyQuaternion h_quaternion
    vt::MyQuaternion j;
    j.s.w = 1.0;
    j.s.x = 0.0;
    j.s.y = 0.0;
    j.s.z = 0.0;

    c->set_h_quaternion(j);

    vt::MyQuaternion k = c->get_h_quaternion();
    assert(k.s.w == 0.707);
    assert(k.s.x == 0.0);
    assert(k.s.y == 0.707);
    assert(k.s.z == 0.0);

    // MyPose is a composite NamedArray with fields of other NamedArrays
    // property MyPose l_pose
    vt::MyPose l;
    l.s.orientation.s.w = 1.0;
    l.s.orientation.s.x = 0.0;
    l.s.orientation.s.y = 0.0;
    l.s.orientation.s.z = 0.0;
    l.s.position.s.x = 1.0;
    l.s.position.s.y = 2.0;
    l.s.position.s.z = 3.0;

    c->set_i_pose(l);

    vt::MyPose m = c->get_i_pose();
    assert(m.s.position.s.x == 4.0);
    assert(m.s.position.s.y == 5.0);
    assert(m.s.position.s.z == 6.0);
    assert(m.s.orientation.s.w == 0.707);
    assert(m.s.orientation.s.x == 0.0);
    assert(m.s.orientation.s.y == 0.707);
    assert(m.s.orientation.s.z == 0.0);

    // Now use plain arrays and convert to and from NamedArrays
    double n1[] = {1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 3.0};
    vt::MyPose n;
    memcpy(&n.a[0], n1, sizeof(vt::MyPose));
    c->set_i_pose(n);

    vt::MyPose o = c->get_i_pose();
    double o1[]{0.707, 0.0, 0.707, 0.0, 4.0, 5.0, 6.0};
    assert(memcmp(&o.a[0], o1, sizeof(vt::MyPose)) == 0);

    std::cout << "namedarray_value_types.cpp example complete" << std::endl;
    return 0;
}
