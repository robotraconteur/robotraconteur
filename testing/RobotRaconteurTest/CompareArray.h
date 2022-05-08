#include <RobotRaconteur.h>

#pragma once

// NOLINTBEGIN

using namespace RobotRaconteur;

namespace RobotRaconteurTest
{
template <class T>
void ca(const RR_INTRUSIVE_PTR<RRArray<T> >& v1, const RR_INTRUSIVE_PTR<RRArray<T> >& v2)
{
    RR_NULL_CHECK(v1);
    RR_NULL_CHECK(v2);
    if (v1->size() != v2->size())
        throw std::exception();
    for (size_t i = 0; i < v1->size(); i++)
    {
        if ((*v1)[i] != (*v2)[i])
            throw std::exception();
    }
}

template <>
void ca<double>(const RR_INTRUSIVE_PTR<RRArray<double> >& v1, const RR_INTRUSIVE_PTR<RRArray<double> >& v2);

template <>
void ca<cdouble>(const RR_INTRUSIVE_PTR<RRArray<cdouble> >& v1, const RR_INTRUSIVE_PTR<RRArray<cdouble> >& v2);

template <>
void ca<cfloat>(const RR_INTRUSIVE_PTR<RRArray<cfloat> >& v1, const RR_INTRUSIVE_PTR<RRArray<cfloat> >& v2);

template <class T>
void ca(const std::vector<T>& v1, const std::vector<T>& v2)
{
    if (v1.size() != v2.size())
        throw std::exception();
    for (size_t i = 0; i < v1.size(); i++)
    {
        if ((v1)[i] != (v2)[i])
            throw std::exception();
    }
}

} // namespace RobotRaconteurTest
// NOLINTEND