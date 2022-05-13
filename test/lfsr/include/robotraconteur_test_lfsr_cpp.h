#pragma once

#include "robotraconteur_test_lfsr.h"

#ifdef __cplusplus
#include <RobotRaconteur/DataTypes.h>

namespace RobotRaconteur
{
namespace test
{
    class LFSRSeqGen
    {
    protected:
        uint32_t lfsr;
    
    public:
        LFSRSeqGen(uint32_t seed)
        {
            lfsr = seed;
        }

        LFSRSeqGen(uint32_t seed, const std::string& key)
        {
            lfsr = robotraconteur_test_lfsr_initkey(seed, key.c_str(), key.size());
        }

        int8_t NextInt8()
        {
            int8_t ret;
            lfsr = robotraconteur_test_lfsr_next_int8(lfsr, &ret);
            return ret;
        }

        uint8_t NextUInt8()
        {
            uint8_t ret;
            lfsr = robotraconteur_test_lfsr_next_uint8(lfsr, &ret);
            return ret;
        }

        int16_t NextInt16()
        {
            int16_t ret;
            lfsr = robotraconteur_test_lfsr_next_int16(lfsr, &ret);
            return ret;
        }

        uint16_t NextUInt16()
        {
            uint16_t ret;
            lfsr = robotraconteur_test_lfsr_next_uint16(lfsr, &ret);
            return ret;
        }

        int32_t NextInt32()
        {
            int32_t ret;
            lfsr = robotraconteur_test_lfsr_next_int32(lfsr, &ret);
            return ret;
        }

        uint32_t NextUInt32()
        {
            uint32_t ret;
            lfsr = robotraconteur_test_lfsr_next_uint32(lfsr, &ret);
            return ret;
        }

        int64_t NextInt64()
        {
            int64_t ret;
            lfsr = robotraconteur_test_lfsr_next_int64(lfsr, &ret);
            return ret;
        }

        uint64_t NextUInt64()
        {
            uint64_t ret;
            lfsr = robotraconteur_test_lfsr_next_uint64(lfsr, &ret);
            return ret;
        }

        float NextFloat()
        {
            float ret;
            lfsr = robotraconteur_test_lfsr_next_float(lfsr, &ret);
            return ret;
        }

        double NextDouble()
        {
            double ret;
            lfsr = robotraconteur_test_lfsr_next_double(lfsr, &ret);
            return ret;
        }

        RobotRaconteur::rr_bool NextBool()
        {
            RobotRaconteur::rr_bool ret;
            lfsr = robotraconteur_test_lfsr_next_bool(lfsr, &ret.value);
            return ret;
        }

        RobotRaconteur::cfloat NextCFloat()
        {
            RobotRaconteur::cfloat ret;
            lfsr = robotraconteur_test_lfsr_next_cfloat(lfsr, &ret.real, &ret.imag);
            return ret;
        }

        RobotRaconteur::cdouble NextCDouble()
        {
            RobotRaconteur::cdouble ret;
            lfsr = robotraconteur_test_lfsr_next_cdouble(lfsr, &ret.real, &ret.imag);
            return ret;
        }

        char NextChar()
        {
            char ret;
            lfsr = robotraconteur_test_lfsr_next_char(lfsr, &ret);
            return ret;
        }

        size_t NextLen(size_t max_len)
        {
            size_t len;
            lfsr = robotraconteur_test_lfsr_next_len(lfsr, max_len, &len);
            return len;
        }

        uint32_t NextDist(uint32_t min_, uint32_t max_)
        {
            uint32_t val;
            lfsr = robotraconteur_test_lfsr_next_dist(lfsr, min_, max_, &val);
            return val;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > NextInt8Array(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > ret = RobotRaconteur::AllocateRRArray<int8_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextInt8();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > NextUInt8Array(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > ret = RobotRaconteur::AllocateRRArray<uint8_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextUInt8();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > NextInt16Array(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > ret = RobotRaconteur::AllocateRRArray<int16_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextInt16();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > NextUInt16Array(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > ret = RobotRaconteur::AllocateRRArray<uint16_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextUInt16();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > NextInt32Array(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > ret = RobotRaconteur::AllocateRRArray<int32_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextInt32();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > NextUInt32Array(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > ret = RobotRaconteur::AllocateRRArray<uint32_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextUInt32();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > NextInt64Array(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > ret = RobotRaconteur::AllocateRRArray<int64_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextInt64();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > NextUInt64Array(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > ret = RobotRaconteur::AllocateRRArray<uint64_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextUInt64();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > NextFloatArray(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > ret = RobotRaconteur::AllocateRRArray<float>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextFloat();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > NextDoubleArray(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > ret = RobotRaconteur::AllocateRRArray<double>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextDouble();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::rr_bool> > NextBoolArray(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::rr_bool> > ret = RobotRaconteur::AllocateRRArray<RobotRaconteur::rr_bool>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextBool();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat> > NextCFloatArray(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat> > ret = RobotRaconteur::AllocateRRArray<RobotRaconteur::cfloat>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextCFloat();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble> > NextCDoubleArray(size_t len)
        {
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble> > ret = RobotRaconteur::AllocateRRArray<RobotRaconteur::cdouble>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextCDouble();
            }
            return ret;
        }

        std::string NextString(size_t len)
        {
            std::string ret;
            ret.resize(len,' ');
            for (size_t i = 0; i<len; i++)
            {
                ret[i] = NextChar();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > NextInt8ArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int8_t> > ret = RobotRaconteur::AllocateRRArray<int8_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextInt8();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > NextUInt8ArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t> > ret = RobotRaconteur::AllocateRRArray<uint8_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextUInt8();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > NextInt16ArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int16_t> > ret = RobotRaconteur::AllocateRRArray<int16_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextInt16();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > NextUInt16ArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint16_t> > ret = RobotRaconteur::AllocateRRArray<uint16_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextUInt16();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > NextInt32ArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t> > ret = RobotRaconteur::AllocateRRArray<int32_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextInt32();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > NextUInt32ArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint32_t> > ret = RobotRaconteur::AllocateRRArray<uint32_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextUInt32();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > NextInt64ArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int64_t> > ret = RobotRaconteur::AllocateRRArray<int64_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextInt64();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > NextUInt64ArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint64_t> > ret = RobotRaconteur::AllocateRRArray<uint64_t>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextUInt64();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > NextFloatArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<float> > ret = RobotRaconteur::AllocateRRArray<float>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextFloat();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > NextDoubleArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<double> > ret = RobotRaconteur::AllocateRRArray<double>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextDouble();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::rr_bool> > NextBoolArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::rr_bool> > ret = RobotRaconteur::AllocateRRArray<RobotRaconteur::rr_bool>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextBool();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat> > NextCFloatArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat> > ret = RobotRaconteur::AllocateRRArray<RobotRaconteur::cfloat>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextCFloat();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble> > NextCDoubleArrayVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble> > ret = RobotRaconteur::AllocateRRArray<RobotRaconteur::cdouble>(len);
            for (size_t i = 0; i<len; i++)
            {
                (*ret)[i] = NextCDouble();
            }
            return ret;
        }

        std::string NextStringVarLen(size_t max_len)
        {
            size_t len = NextLen(max_len);
            std::string ret;
            ret.resize(len,' ');
            for (size_t i = 0; i<len; i++)
            {
                ret[i] = NextChar();
            }
            return ret;
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRBaseArray> NextArrayByType(RobotRaconteur::DataTypes dtype, size_t len)
        {
            switch (dtype)
            {
            case DataTypes_double_t:
                return NextDoubleArray(len);
            case DataTypes_single_t:
                return NextFloatArray(len);
            case DataTypes_int8_t:
                return NextInt8Array(len);
            case DataTypes_uint8_t:
                return NextUInt8Array(len);
            case DataTypes_int16_t:
                return NextInt16Array(len);
            case DataTypes_uint16_t:
                return NextUInt16Array(len);
            case DataTypes_int32_t:
                return NextInt32Array(len);
            case DataTypes_uint32_t:
                return NextUInt32Array(len);
            case DataTypes_int64_t:
                return NextInt64Array(len);
            case DataTypes_uint64_t:
                return NextUInt64Array(len);
            case DataTypes_string_t:
                return RobotRaconteur::stringToRRArray(NextString(len));
            case DataTypes_bool_t:
                return NextBoolArray(len);
            case DataTypes_csingle_t:
                return NextCFloatArray(len);
            case DataTypes_cdouble_t:
                return NextCDoubleArray(len);
            default:
                BOOST_ASSERT(false);
                return NULL;
        }
        }

        RR_INTRUSIVE_PTR<RobotRaconteur::RRBaseArray> NextArrayByTypeVarLen(RobotRaconteur::DataTypes dtype, size_t max_len)
        {
            switch (dtype)
            {
            case DataTypes_double_t:
                return NextDoubleArrayVarLen(max_len);
            case DataTypes_single_t:
                return NextFloatArrayVarLen(max_len);
            case DataTypes_int8_t:
                return NextInt8ArrayVarLen(max_len);
            case DataTypes_uint8_t:
                return NextUInt8ArrayVarLen(max_len);
            case DataTypes_int16_t:
                return NextInt16ArrayVarLen(max_len);
            case DataTypes_uint16_t:
                return NextUInt16ArrayVarLen(max_len);
            case DataTypes_int32_t:
                return NextInt32ArrayVarLen(max_len);
            case DataTypes_uint32_t:
                return NextUInt32ArrayVarLen(max_len);
            case DataTypes_int64_t:
                return NextInt64ArrayVarLen(max_len);
            case DataTypes_uint64_t:
                return NextUInt64ArrayVarLen(max_len);
            case DataTypes_string_t:
                return RobotRaconteur::stringToRRArray(NextStringVarLen(max_len));
            case DataTypes_bool_t:
                return NextBoolArrayVarLen(max_len);
            case DataTypes_csingle_t:
                return NextCFloatArrayVarLen(max_len);
            case DataTypes_cdouble_t:
                return NextCDoubleArrayVarLen(max_len);
            default:
                BOOST_ASSERT(false);
                return NULL;
        }
        }

        template<typename T>
        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> > NextArray(size_t len)
        {
            return RR_STATIC_POINTER_CAST<RobotRaconteur::RRArray<T> >(NextArrayByType(RobotRaconteur::RRPrimUtil<T>::GetTypeID(), len));
        }

        template<typename T>
        RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> > NextArrayVarLen(size_t max_len)
        {
            return RR_STATIC_POINTER_CAST<RobotRaconteur::RRArray<T> >(NextArrayByTypeVarLen(RobotRaconteur::RRPrimUtil<T>::GetTypeID(), max_len));
        }
    };
}
}

#endif