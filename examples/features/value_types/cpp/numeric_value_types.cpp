// numeric_value_types.cpp - Example of using numeric value types

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

    // Scalar numbers
    // property double a_double
    double a_double_read = c->get_a_double();
    std::cout << a_double_read << std::endl;
    // assert used for the rest of the example property read operations
    // to demonstrate the expected values.
    assert(c->get_a_double() == 5.78);
    c->set_a_double(49.3);
    // property int32 b_int
    assert(c->get_b_int() == 4557);
    c->set_b_int(359);
    // property uint8 c_byte
    assert(c->get_c_byte() == 0x1A);
    c->set_c_byte(31);
    // property cdouble d_cdouble
    assert(c->get_d_cdouble() == RR::cdouble(23.7, 5.3));
    c->set_d_cdouble(RR::cdouble(1.2, 3.4));
    // property bool e_bool
    // RR::rr_bool is used for Robot Raconteur bool values
    assert(c->get_e_bool().value != 0);
    c->set_e_bool(RR::rr_bool(0));
    // property int32 meaning_of_life [readonly]
    assert(c->get_meaning_of_life() == 42);

    // Numeric Arrays
    // property double[] a_double_array
    assert_array(c->get_a_double_array(), {0.016, 0.226});

    // Examples of different ways to initialize RRArray. Also see Eigen converters in robotraconteur-companion

    // Use AttachRRArrayCopy()
    double a_double_array_1a[] = {0.582, 0.288, 0.09, 0.213, 0.98};
    RR::RRArrayPtr<double> a_double_array_1 = RR::AttachRRArrayCopy(a_double_array_1a, 5);
    c->set_a_double_array(a_double_array_1);

    // Use AllocateRRArray to allocate an uninitialized array
    RR::RRArrayPtr<double> a_double_array_2 = RR::AllocateRRArray<double>(5);
    a_double_array_2->at(0) = 0.582;
    a_double_array_2->at(1) = 0.288;
    a_double_array_2->at(2) = 0.09;
    a_double_array_2->at(3) = 0.213;
    a_double_array_2->at(4) = 0.98;
    c->set_a_double_array(a_double_array_2);

    RR::RRArrayPtr<double> a_double_array_2b = c->get_a_double_array();
    assert(a_double_array_2b->size() == 2);
    assert(a_double_array_2b->at(0) == 0.016);
    assert((*a_double_array_2b)[1] == 0.226);

    // Use VectorToRRArray to convert a std::vector to RRArray
    std::vector<double> a_double_array_3a = {0.582, 0.288, 0.09, 0.213, 0.98};
    RR::RRArrayPtr<double> a_double_array_3 = RR::VectorToRRArray<double, double>(a_double_array_3a);
    c->set_a_double_array(a_double_array_3);

    // property double[3] a_double_array_fixed
    assert_array(c->get_a_double_array_fixed(), {0.13, 0.27, 0.15});
    c->set_a_double_array_fixed(RR::VectorToRRArray<double, double>({0.21, 0.12, 0.39}));
    // % property double[6-] a_double_array_maxlen
    assert_array(c->get_a_double_array_maxlen(), {0.7, 0.16, 0.16, 0.05, 0.61, 0.9});
    c->set_a_double_array_maxlen(RR::VectorToRRArray<double, double>({0.035, 0.4}));
    // property double[3,2] a_double_marray_fixed
    RR::RRMultiDimArrayPtr<double> a_double_marray_fixed_read = c->get_a_double_marray_fixed();
    assert_array(a_double_marray_fixed_read->Dims, {3, 2});
    assert_array(a_double_marray_fixed_read->Array, {0.29, 0.41, 0.4, 0.66, 0.6, 0.2});
    RR::RRMultiDimArrayPtr<double> a_double_marray_fixed_1 =
        RR::AllocateRRMultiDimArray<double>(RR::VectorToRRArray<uint32_t, uint32_t>({3, 2}),
                                            RR::VectorToRRArray<double, double>({0.3, 0.4, 0.5, 0.6, 0.6, 0.2}));
    c->set_a_double_marray_fixed(a_double_marray_fixed_1);
    // property double[*] a_double_marray
    RR::RRMultiDimArrayPtr<double> a_double_marray_read = c->get_a_double_marray();
    assert_array(a_double_marray_read->Dims, {2, 2});
    assert_array(a_double_marray_read->Array, {0.72, 0.05, 0.4, 0.07});
    RR::RRMultiDimArrayPtr<double> a_double_marray_1 = RR::AllocateRRMultiDimArray<double>(
        RR::VectorToRRArray<uint32_t, uint32_t>({2, 1}), RR::VectorToRRArray<double, double>({0.3, 0.01}));
    // property uint8[] c_byte_array
    assert_array(c->get_c_byte_array(), {0x1A, 0x2B});
    c->set_c_byte_array(RR::VectorToRRArray<uint8_t, uint8_t>({0x3C, 0x4D, 0x5E, 0x6F, 0x70}));

    std::cout << "numeric_value_types.cpp example complete" << std::endl;
    return 0;
}
