// pod_value_types.cpp - Example of using pod value types

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

    // Example using Pods

    // property MyPod j_pod
    vt::MyPod a;
    a.a = 0.928;
    a.b = 8374;
    a.c[0] = 8;
    a.c[1] = 9;
    a.c[2] = 10;
    a.c[3] = 11;
    a.d.resize(1);
    a.d[0] = 17;
    double a_e[6] = {1, 4, 2, 5, 3, 6};
    memcpy(&a.e[0], a_e, sizeof(a_e));
    a.f.s.x = 10.1;
    a.f.s.y = 10.2;
    a.f.s.z = 10.3;
    c->set_j_pod(a);

    vt::MyPod b = c->get_j_pod();
    assert(b.a == 0.791);
    assert(b.b == 1077);
    double b_c[4] = {61, 52, 33, 24};
    assert(memcmp(&b.c[0], b_c, sizeof(b_c)) == 0);
    assert(b.d.size() == 2);
    double b_d[2] = {1, 2};
    assert(memcmp(&b.d[0], b_d, sizeof(b_d)) == 0);
    double b_e[6] = {7, 10, 8, 11, 9, 12};
    assert(memcmp(&b.e[0], b_e, sizeof(b_e)) == 0);
    vt::MyVector3 b_f = b.f;
    assert(b_f.s.x == 20.1);
    assert(b_f.s.y == 20.2);
    assert(b_f.s.z == 20.3);

    // Now an array of Pods
    // property MyPod[] j_pod_array
    RR::RRPodArrayPtr<vt::MyPod> d = RR::AllocateEmptyRRPodArray<vt::MyPod>(2);
    d->at(0).a = 0.928;
    d->at(0).b = 8374;
    double d0_c[] = {8, 9, 10, 11};
    memcpy(&d->at(0).c[0], d0_c, sizeof(d0_c));
    d->at(0).d.resize(1);
    d->at(0).d[0] = 17;
    double d0_e[6] = {1, 4, 2, 5, 3, 6};
    memcpy(&d->at(0).e[0], d0_e, sizeof(d0_e));
    d->at(0).f.s.x = 10.1;
    d->at(0).f.s.y = 10.2;
    d->at(0).f.s.z = 10.3;

    d->at(1).a = 0.67;
    d->at(1).b = 123;
    double d1_c[] = {1, 2, 3, 4};
    memcpy(&d->at(1).c[0], d1_c, sizeof(d1_c));
    d->at(1).d.resize(2);
    double d1_d[] = {5, 6};
    memcpy(&d->at(1).d[0], d1_d, sizeof(d1_d));
    double d1_e[6] = {13, 16, 14, 17, 15, 18};
    memcpy(&d->at(1).e[0], d1_e, sizeof(d1_e));
    d->at(1).f.s.x = 30.1;
    d->at(1).f.s.y = 30.2;
    d->at(1).f.s.z = 30.3;

    c->set_j_pod_array(d);

    RR::RRPodArrayPtr<vt::MyPod> f = c->get_j_pod_array();
    assert(f->size() == 2);
    assert(f->at(0).a == 0.791);
    assert(f->at(0).b == 1077);
    double f0_c[4] = {61, 52, 33, 24};
    assert(memcmp(&f->at(0).c[0], f0_c, sizeof(f0_c)) == 0);
    assert(f->at(0).d.size() == 2);
    double f0_d[2] = {1, 2};
    assert(memcmp(&f->at(0).d[0], f0_d, sizeof(f0_d)) == 0);
    double f0_e[6] = {7, 10, 8, 11, 9, 12};
    assert(memcmp(&f->at(0).e[0], f0_e, sizeof(f0_e)) == 0);
    vt::MyVector3 f0_f = f->at(0).f;
    assert(f0_f.s.x == 20.1);
    assert(f0_f.s.y == 20.2);
    assert(f0_f.s.z == 20.3);

    assert(f->at(1).a == 0.03);
    assert(f->at(1).b == 693);
    double f1_c[4] = {5, 6, 7, 8};
    assert(memcmp(&f->at(1).c[0], f1_c, sizeof(f1_c)) == 0);
    assert(f->at(1).d.size() == 1);
    double f1_d[1] = {3};
    assert(memcmp(&f->at(1).d[0], f1_d, sizeof(f1_d)) == 0);
    double f1_e[6] = {19, 22, 20, 23, 21, 24};
    assert(memcmp(&f->at(1).e[0], f1_e, sizeof(f1_e)) == 0);
    vt::MyVector3 f1_f = f->at(1).f;
    assert(f1_f.s.x == 40.1);
    assert(f1_f.s.y == 40.2);
    assert(f1_f.s.z == 40.3);

    std::cout << "pod_value_types.cpp example complete" << std::endl;
    return 0;
}
