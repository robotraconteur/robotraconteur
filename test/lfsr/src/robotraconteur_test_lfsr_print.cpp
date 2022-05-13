#include "robotraconteur_test_lfsr_cpp.h"
#include <iostream>

template<typename T>
void print_array(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> > a)
{
    size_t len = a->size();
    for (size_t i=0; i<len; i++)
    {
        std::cout << +a->at(i);
        if (i+1 < len)
        {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void print_array_bool(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::rr_bool> > a)
{
    size_t len = a->size();
    for (size_t i=0; i<len; i++)
    {
        std::cout << ((bool)a->at(i).value ? "true" : "false");
        if (i+1 < len)
        {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

template<typename T>
void print_array_complex(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<T> > a)
{
    size_t len = a->size();
    for (size_t i=0; i<len; i++)
    {
        T v = a->at(i);
        if (v.imag < 0)
        {
            std::cout << v.real << v.imag << "i";
        }
        else
        {
            std::cout << v.real << "+" << v.imag << "i";
        }
        if (i+1 < len)
        {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    using namespace RobotRaconteur;
    using namespace RobotRaconteur::test;

    size_t count = 8;
    uint32_t seed = 35913226;
    std::string key = "Hello World!";

    if (argc > 1)
    {
        key = std::string(argv[1]);
    }

    LFSRSeqGen seqgen(seed, key);

    print_array(seqgen.NextInt8Array(count));
    print_array(seqgen.NextUInt8Array(count));
    print_array(seqgen.NextInt16Array(count));
    print_array(seqgen.NextUInt16Array(count));
    print_array(seqgen.NextInt32Array(count));
    print_array(seqgen.NextUInt32Array(count));
    print_array(seqgen.NextInt64Array(count));
    print_array(seqgen.NextUInt64Array(count));
    print_array(seqgen.NextFloatArray(count));
    print_array(seqgen.NextDoubleArray(count));
    std::cout << seqgen.NextString(count) << std::endl;
    print_array_bool(seqgen.NextBoolArray(count));
    print_array_complex(seqgen.NextCFloatArray(count));
    print_array_complex(seqgen.NextCDoubleArray(count));
    std::cout << std::endl;
    print_array(seqgen.NextInt8ArrayVarLen(count));
    print_array(seqgen.NextUInt8ArrayVarLen(count));
    print_array(seqgen.NextInt16ArrayVarLen(count));
    print_array(seqgen.NextUInt16ArrayVarLen(count));
    print_array(seqgen.NextInt32ArrayVarLen(count));
    print_array(seqgen.NextUInt32ArrayVarLen(count));
    print_array(seqgen.NextInt64ArrayVarLen(count));
    print_array(seqgen.NextUInt64ArrayVarLen(count));
    print_array(seqgen.NextFloatArrayVarLen(count));
    print_array(seqgen.NextDoubleArrayVarLen(count));
    std::cout << seqgen.NextStringVarLen(count) << std::endl;
    print_array_bool(seqgen.NextBoolArrayVarLen(count));
    print_array_complex(seqgen.NextCFloatArrayVarLen(count));
    print_array_complex(seqgen.NextCDoubleArrayVarLen(count));
}