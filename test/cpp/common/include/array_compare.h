#include <RobotRaconteur/DataTypes.h>
#include <gtest/gtest.h>

#pragma once

namespace RobotRaconteur
{
namespace test
{
namespace detail
{

template <typename T>
class rr_array_near_util_impl
{
public:
    static bool near_(const T& a1, const T& a2)
    {
        return a1 == a2;
    }
};

template <>
class rr_array_near_util_impl<double>
{
public:
    static bool near_(const double& a1, const double& a2)
    {
        if (a1 == a2)
            return true;
        if (isnan(a1) && isnan(a2))
            return true;
        return abs(a1-a2) < 1e-14;
    }
};

template <>
class rr_array_near_util_impl<float>
{
public:
    static bool near_(const float& a1, const float& a2)
    {
        if (a1 == a2)
            return true;
        if (isnan(a1) && isnan(a2))
            return true;
        return abs(a1-a2) < 1e-5f;
    }
};

template <>
class rr_array_near_util_impl<RobotRaconteur::cdouble>
{
public:
    static bool near_(const RobotRaconteur::cdouble& a1, const RobotRaconteur::cdouble& a2)
    {
        return rr_array_near_util_impl<double>::near_(a1.real,a1.real) && rr_array_near_util_impl<double>::near_(a1.imag,a1.imag);        
    }
};

template <>
class rr_array_near_util_impl<RobotRaconteur::cfloat>
{
public:
    static bool near_(const RobotRaconteur::cfloat& a1, const RobotRaconteur::cfloat& a2)
    {
        return rr_array_near_util_impl<float>::near_(a1.real,a1.real) && rr_array_near_util_impl<float>::near_(a1.imag,a1.imag);
    }
};

template <typename T>
bool rr_array_near_util_scalar(const T& a1, const T& a2)
{
    return rr_array_near_util_impl<T>::near_(a1, a2);
}

template <typename T>
bool rr_array_near_util_array(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> >& a1, const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> >& a2)
{
    if ((!a1) && (!a2))
        return true;
    if ((!a1) || (!a2))
    {
        EXPECT_TRUE(a1.get()!=NULL);
        EXPECT_TRUE(a2.get()!=NULL);
        return false;
    }
    if (a1->size() != a2->size())
    {
        EXPECT_EQ(a1->size(), a2->size());
        return false;
    }
    for (size_t i= 0; i< a1->size(); i++)
    {
        if(!RobotRaconteur::test::detail::rr_array_near_util_scalar(a1->at(i),a2->at(i)))
        {
            EXPECT_EQ(a1->at(i),a2->at(i)) << " at index " << i;
            return false;
        }
    }
    return true;
}

template <typename T, typename U, typename V>
bool rr_array_near_util_base_array(const U& a1, const V& a2)
{
    if ((!a1) && (!a2))
        return true;

    if ((!a1) || (!a2))
    {
        EXPECT_TRUE(a1.get()!=NULL);
        EXPECT_TRUE(a2.get()!=NULL);
        return false;
    }

    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> > b1 = RR_DYNAMIC_POINTER_CAST<RobotRaconteur::RRArray<T> >(a1);
    RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> > b2 = RR_DYNAMIC_POINTER_CAST<RobotRaconteur::RRArray<T> >(a2);


    return RobotRaconteur::test::detail::rr_array_near_util_array(b1, b2);
}

}

template <typename T>
bool RRArrayNear(const T& arg, const T& b)
{
    return RobotRaconteur::test::detail::rr_array_near_util_array(arg,b);
}

template <typename T>
bool RRBaseArrayNear(const T& arg, const T& b)
{
    switch (b->GetTypeID())
    {
		case DataTypes_double_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<double>(arg,b);
		case DataTypes_single_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<float>(arg,b);
		case DataTypes_int8_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<int8_t>(arg,b);
		case DataTypes_uint8_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<uint8_t>(arg,b);
		case DataTypes_int16_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<int16_t>(arg,b);
		case DataTypes_uint16_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<uint16_t>(arg,b);
		case DataTypes_int32_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<int32_t>(arg,b);
		case DataTypes_uint32_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<uint32_t>(arg,b);
		case DataTypes_int64_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<int64_t>(arg,b);
		case DataTypes_uint64_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<uint64_t>(arg,b);
		case DataTypes_string_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<char>(arg,b);
        case DataTypes_cdouble_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<RobotRaconteur::cdouble>(arg,b);
		case DataTypes_csingle_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<RobotRaconteur::cfloat>(arg,b);
        case DataTypes_bool_t:
            return RobotRaconteur::test::detail::rr_array_near_util_base_array<RobotRaconteur::rr_bool>(arg,b);
        default:
            assert(false);
    }
    return false;
}

template<typename T>
bool RRCompareVec(const std::vector<T>& v1, const std::vector<T>& v2)
{
    EXPECT_EQ(v1.size(),v2.size());
    if (v1.size() != v2.size())
        return false;

    for (size_t i=0; i<v1.size(); i++)
    {
        EXPECT_EQ(v1.at(i),v2.at(i)) << " at index " << i;
        if (v1.at(i) != v2.at(i))
            return false;
    }
    return true;
}

}
}

#define EXPECT_RRARRAY_EQ(a,b) EXPECT_TRUE(RobotRaconteur::test::RRArrayNear(a,b))
#define EXPECT_RRBASEARRAY_EQ(a,b) EXPECT_TRUE(RobotRaconteur::test::RRBaseArrayNear(a,b))
#define ASSERT_RRARRAY_EQ(a,b) ASSERT_TRUE(RobotRaconteur::test::RRArrayNear(a,b))
#define ASSERT_RRBASEARRAY_EQ(a,b) ASSERT_TRUE(RobotRaconteur::test::RRBaseArrayNear(a,b))

#define EXPECT_RRVECTOR_EQ(a,b) EXPECT_TRUE(RobotRaconteur::test::RRCompareVec(a,b))
#define ASSERT_RRVECTOR_EQ(a,b) ASSERT_TRUE(RobotRaconteur::test::RRCompareVec(a,b))