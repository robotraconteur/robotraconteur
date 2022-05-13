#include <RobotRaconteur/DataTypes.h>
#include "robotraconteur_test_lfsr_cpp.h"
#include <gtest/gtest.h>


#pragma once

namespace RobotRaconteur
{
namespace test
{
    template<typename T>
    RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<T> > TestingCopyMultiDimArray(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<T> > src)
    {
        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > dims2 = RobotRaconteur::AttachRRArrayCopy(src->Dims->data(), src->Dims->size());
        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> > array2 = RobotRaconteur::AttachRRArrayCopy(src->Array->data(), src->Array->size());

        return RobotRaconteur::AllocateRRMultiDimArray(dims2, array2);
    }

    template<typename T>
    RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<T> > FillMultiDimArray(LFSRSeqGen& seqgen, std::vector<uint32_t> dims)
    {
        uint32_t n_elems = boost::accumulate(dims, 1, std::multiplies<uint32_t>());

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > rr_dims = RobotRaconteur::VectorToRRArray<uint32_t>(dims);
        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> > rr_array = seqgen.NextArray<T>(n_elems);

        return RobotRaconteur::AllocateRRMultiDimArray<T>(rr_dims,rr_array);
    }

    template<typename T>
    bool RRMultiDimArrayNear(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<T> > a1, RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<T> > a2)
    {
        if ((!a1) && (!a2))
            return true;
        if ((!a1) || (!a2))
        {
            EXPECT_TRUE(a1.get()!=NULL);
            EXPECT_TRUE(a2.get()!=NULL);
            return false;
        }
        
        bool dims_eq;
        bool array_eq;
        {
            SCOPED_TRACE("multidimarray dims");
            dims_eq = RRArrayNear(a1->Dims,a2->Dims);
            EXPECT_TRUE(dims_eq);
        }
        {
            SCOPED_TRACE("multidimarray data");
            array_eq = RRArrayNear(a1->Array,a2->Array);
            EXPECT_TRUE(array_eq);
        }
        return dims_eq && array_eq;        
    }

    template <typename T>
    RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<T> > GetMultiDimArraySub(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<T> > source, std::vector<uint32_t> srcpos, std::vector<uint32_t> count)
    {
        std::vector<uint32_t> srcdims = RobotRaconteur::RRArrayToVector<uint32_t>(source->Dims);
        std::vector<uint32_t> pos(srcdims.size());
        std::vector<uint32_t> step(srcdims.size());
        step[0] = 1;
        for (size_t i=1; i<step.size(); i++)
        {
            step[i] = step[i-1]*srcdims[i-1];
        }

        uint32_t n_relems = boost::accumulate(count, 1, std::multiplies<uint32_t>());
        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> > r_array=RobotRaconteur::AllocateRRArray<T>(n_relems);

        for (size_t i=0; i<n_relems; i++)
        {
            uint32_t n_pos = 0;
            for (size_t j=0; j<step.size(); j++)
            {
                n_pos += step[j] * (pos[j] + srcpos[j]);
            }

            if (!(i < r_array->size()))
            {
                EXPECT_LT(i, r_array->size());
                return NULL;
            }

            if (!(n_pos < source->Array->size()))
            {
                EXPECT_LT(n_pos, source->Array->size());
                return NULL;
            }

            (*r_array)[i] = (*source->Array)[n_pos];

            for (size_t j=0; j<step.size(); j++)
            {
                pos[j]++;
                if (!(pos[j] < count[j]))
                {
                    pos[j] = 0;
                }
                else
                {
                    break;
                }
            }
        }

        return RobotRaconteur::AllocateRRMultiDimArray<T>(RobotRaconteur::VectorToRRArray<uint32_t>(count), r_array);
    }

    template <typename T>
    void SetMultiDimArraySub(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<T> > dest, std::vector<uint32_t> destpos, 
        RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<T> > source)
    {
        std::vector<uint32_t> count = RRArrayToVector<uint32_t>(source->Dims);
        std::vector<uint32_t> destdims = RobotRaconteur::RRArrayToVector<uint32_t>(dest->Dims);
        std::vector<uint32_t> pos(destdims.size());
        std::vector<uint32_t> step(destdims.size());
        step[0] = 1;
        for (size_t i=1; i<step.size(); i++)
        {
            step[i] = step[i-1]*destdims[i-1];
        }

        uint32_t n_relems = boost::accumulate(count, 1, std::multiplies<uint32_t>());
        
        for (size_t i=0; i<n_relems; i++)
        {
            uint32_t n_pos = 0;
            for (size_t j=0; j<step.size(); j++)
            {
                n_pos += step[j] * (pos[j] + destpos[j]);
            }

            if (!(i < source->Array->size()))
            {
                EXPECT_LT(i, source->Array->size());
                return;
            }

            if (!(n_pos < dest->Array->size()))
            {
                EXPECT_LT(n_pos, dest->Array->size());
                return;
            }

            (*dest->Array)[n_pos] = (*source->Array)[i];

            for (size_t j=0; j<step.size(); j++)
            {
                pos[j]++;
                if (!(pos[j] < count[j]))
                {
                    pos[j] = 0;
                }
                else
                {
                    break;
                }
            }
        }
    }
}
}

#define EXPECT_RRMULTIDIMARRAY_EQ(a,b) EXPECT_TRUE(RobotRaconteur::test::RRMultiDimArrayNear(a,b))
#define ASSERT_RRMULTIDIMARRAY_EQ(a,b) ASSERT_TRUE(RobotRaconteur::test::RRMultiDimArrayNear(a,b))
