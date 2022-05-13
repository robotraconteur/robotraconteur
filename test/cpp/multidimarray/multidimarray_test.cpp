#include "multidim_array_util.h"
#include "array_compare.h"

#include <boost/shared_array.hpp>
#include <RobotRaconteur/IOUtils.h>

#include <gtest/gtest.h>

#include <boost/assign/list_of.hpp>

using namespace RobotRaconteur;
using namespace RobotRaconteur::test;


TEST(MultiDimArray,SanityTest)
{
    /*
    import numpy as np

    sub1 = np.array(range(5*6*7)).reshape((5,6,7),order="F")[2:5,0:4,4:6].flatten(order="F")
    print("boost::assign::list_of" + "".join([f"({x})" for x in sub1]))

    arr2 = np.zeros((5,5),dtype=np.uint32)
    arr2[2:6,1:3] = np.array(range(1,7)).reshape((3,2), order="F")
    arr3 = arr2.flatten(order="F")
    print("boost::assign::list_of" + "".join([f"({x})" for x in arr3]))
    */

    std::vector<uint32_t> sub_array_e = boost::assign::list_of(122)(123)(124)(127)(128)(129)(132)(133)(134) 
        (137)(138)(139)(152)(153)(154)(157)(158)(159)(162)(163)(164)(167)(168)(169);
    std::vector<uint32_t> sub_dims_e = boost::assign::list_of(3)(4)(2);

    RR_INTRUSIVE_PTR<RRArray<uint32_t> > src_array1 = AllocateRRArray<uint32_t>(210);
    for (uint32_t i=0; i<210; i++)
    {
        (*src_array1)[i] = i;
    }
    std::vector<uint32_t> src_dims = boost::assign::list_of(5)(6)(7);
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint32_t> > src_array=AllocateRRMultiDimArray<uint32_t>(VectorToRRArray<uint32_t>(src_dims), src_array1);

    RR_INTRUSIVE_PTR<RRMultiDimArray<uint32_t> > sub_array=GetMultiDimArraySub(src_array, boost::assign::list_of(2)(0)(4), boost::assign::list_of(3)(4)(2));

    ASSERT_EQ(sub_dims_e, RRArrayToVector<uint32_t>(sub_array->Dims));

    ASSERT_EQ(sub_array_e.size(), sub_array->Array->size());

    for (size_t i=0; i<sub_array_e.size(); i++)
    {
        ASSERT_EQ(sub_array_e[i], (*sub_array->Array)[i]);
    }

    std::vector<uint32_t> dest2_array_e = boost::assign::list_of(0)(0)(0)(0)(0)(0)(0)(1)(2)(3)(0)(0)(4)(5)(6)(0)(0)(0)(0)(0)(0)(0)(0)(0)(0);
    RR_INTRUSIVE_PTR<RRArray<uint32_t> > sub2_dest_array1 = AllocateRRArray<uint32_t>(25);
    for (uint32_t i=0; i<25; i++)
    {
        (*sub2_dest_array1)[i] = 0;
    }

    RR_INTRUSIVE_PTR<RRArray<uint32_t> > sub2_src_array1 = AllocateRRArray<uint32_t>(6);
    for (uint32_t i=0; i<6; i++)
    {
        (*sub2_src_array1)[i] = i+1;
    }

    std::vector<uint32_t> sub2_dest_dims = boost::assign::list_of(5)(5);
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint32_t> > sub2_dest_array=AllocateRRMultiDimArray<uint32_t>(VectorToRRArray<uint32_t>(sub2_dest_dims), sub2_dest_array1);

    std::vector<uint32_t> sub2_src_dims = boost::assign::list_of(3)(2);
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint32_t> > sub2_src_array=AllocateRRMultiDimArray<uint32_t>(VectorToRRArray<uint32_t>(sub2_src_dims), sub2_src_array1);

    std::vector<uint32_t> sub2_dest_pos = boost::assign::list_of(2)(1);
    SetMultiDimArraySub(sub2_dest_array, sub2_dest_pos, sub2_src_array);

    for (size_t i=0; i<dest2_array_e.size(); i++)
    {
        ASSERT_EQ(dest2_array_e[i], (*sub2_dest_array->Array)[i]) << " index " << i;
    }
}

TEST(MultiDimArrayTest,TestDouble)
{
    LFSRSeqGen rng((uint32_t)std::time(0), "multidimarray_test_double");
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m1=FillMultiDimArray<double>(rng, boost::assign::list_of(10)(10)(10)(10)(10));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m2=FillMultiDimArray<double>(rng, boost::assign::list_of(2)(8)(8)(2)(2));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m2_1 = GetMultiDimArraySub(m2, boost::assign::list_of(0)(2)(0)(0)(0), boost::assign::list_of(1)(5)(5)(2)(1));
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m3=TestingCopyMultiDimArray(m1);
    SetMultiDimArraySub(m3,boost::assign::list_of(2)(2)(3)(3)(4),m2_1);
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m4=GetMultiDimArraySub(m3, boost::assign::list_of(4)(2)(2)(8)(0), boost::assign::list_of(2)(2)(1)(1)(10));
    uint32_t m5a[]={ 4, 4, 4, 4, 10 };
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m5=AllocateRRMultiDimArray<double>(AttachRRArray(m5a,5,false),AllocateEmptyRRArray<double>(2560));
    SetMultiDimArraySub(m5, boost::assign::list_of(2)(1)(2)(1)(0), m4);

    m1->AssignSubArray(boost::assign::list_of(2)(2)(3)(3)(4),
        m2,
        boost::assign::list_of(0)(2)(0)(0)(0),
        boost::assign::list_of(1)(5)(5)(2)(1));
    ASSERT_RRARRAY_EQ(m1->Array,m3->Array);
    

    uint32_t m6a[]={2,2,1,1,10};
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m6 =AllocateRRMultiDimArray<double>(AttachRRArray(m6a,5,false),AllocateRRArray<double>(40));
    m1->RetrieveSubArray(boost::assign::list_of(4)(2)(2)(8)(0),
        m6,
        boost::assign::list_of(0)(0)(0)(0)(0),
        boost::assign::list_of(2)(2)(1)(1)(10));
    ASSERT_RRARRAY_EQ(m4->Array,m6->Array);
    

    uint32_t m7a[]={ 4, 4, 4, 4, 10 };
    RR_INTRUSIVE_PTR<RRMultiDimArray<double> > m7=AllocateRRMultiDimArray<double>(AttachRRArray(m7a,5,false),AllocateRRArray<double>(2560));
    memset(m7->Array->data(),0,2560*sizeof(double));		
    m1->RetrieveSubArray(boost::assign::list_of(4)(2)(2)(8)(0),
        m7,
        boost::assign::list_of(2)(1)(2)(1)(0),
        boost::assign::list_of(2)(2)(1)(1)(10));
    ASSERT_RRARRAY_EQ(m5->Array,m7->Array);
}

TEST(MultiDimArrayTest,TestByte)
{
    LFSRSeqGen rng((uint32_t)std::time(0), "multidimarray_test_byte");
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m1=FillMultiDimArray<uint8_t>(rng, boost::assign::list_of(1024)(1024));
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m2=FillMultiDimArray<uint8_t>(rng, boost::assign::list_of(450)(450));
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m2_1 = GetMultiDimArraySub(m2, boost::assign::list_of(20)(25), boost::assign::list_of(200)(200));
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m3=TestingCopyMultiDimArray(m1);
    SetMultiDimArraySub(m3,boost::assign::list_of(50)(100),m2_1);
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m4=GetMultiDimArraySub(m3, boost::assign::list_of(65)(800), boost::assign::list_of(200)(200));
    uint32_t m5a[]={ 512, 512 };
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m5=AllocateRRMultiDimArray<uint8_t>(AttachRRArray(m5a,2,false),AllocateEmptyRRArray<uint8_t>(512*512));
    SetMultiDimArraySub(m5, boost::assign::list_of(100)(230), m4);

    m1->AssignSubArray(boost::assign::list_of(50)(100),m2,boost::assign::list_of(20)(25),boost::assign::list_of(200)(200));
    ASSERT_RRARRAY_EQ(m1->Array,m3->Array);
    
    uint32_t m6a[]={200,200};
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m6=AllocateRRMultiDimArray<uint8_t>(AttachRRArray(m6a,2,false),AllocateRRArray<uint8_t>(40000));
    m1->RetrieveSubArray(boost::assign::list_of(65)(800),m6,boost::assign::list_of(0)(0),boost::assign::list_of(200)(200));
    ASSERT_RRARRAY_EQ(m4->Array,m6->Array);
    

    uint32_t m7a[]={ 512,512 };
    RR_INTRUSIVE_PTR<RRMultiDimArray<uint8_t> > m7=AllocateRRMultiDimArray<uint8_t>(AttachRRArray(m7a,2,false),AllocateEmptyRRArray<uint8_t>(512*512));
    m1->RetrieveSubArray(boost::assign::list_of(65)(800),m7,boost::assign::list_of(100)(230),boost::assign::list_of(200)(200));
    ASSERT_RRARRAY_EQ(m5->Array,m7->Array);
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
