#ifdef ROBOTRACONTEUR_USE_STDAFX
#include "stdafx.h"
#endif

#include "ServiceTestClient2.h"

namespace RobotRaconteurTest
{

	void ServiceTestClient2::RunFullTest(const std::string& url)
	{
		Connect(url);
		
		r->get_o4();
		TestWirePeekPoke();
		AsyncTestWirePeekPoke();

		TestEnums();
		TestPods();
		TestMemories();

		TestGenerators();

		TestNamedArrays();
		TestNamedArrayMemories();

		TestComplex();
		TestComplexMemories();

		TestNoLock();

		TestBools();
		TestBoolMemories();

		TestExceptionParams();

		Disconnect();
	}

	void ServiceTestClient2::Connect(const std::string& url)
	{
		r = rr_cast<testroot3>(RobotRaconteurNode::s()->ConnectService(url));
	}

	void ServiceTestClient2::Disconnect()
	{
		RobotRaconteurNode::s()->DisconnectService(r);
	}

	void ServiceTestClient2::TestWirePeekPoke()
	{

		//Test Peek

		TimeSpec ts;
		int32_t v;
		EXPECT_NO_THROW(v = r->get_peekwire()->PeekInValue(ts));
		EXPECT_EQ(v, 56295674);

		//Test Poke

		EXPECT_NO_THROW(r->get_pokewire()->PokeOutValue(75738265));
		TimeSpec ts2;
		int32_t v2;
		EXPECT_NO_THROW(v2 = r->get_pokewire()->PeekOutValue(ts2));
		EXPECT_EQ(v2, 75738265);

		//Test WireUnicastReceiver

		RR_SHARED_PTR<WireConnection<int32_t> > w;
		ASSERT_NO_THROW(w = r->get_pokewire()->Connect());
		for (size_t i = 0; i < 3; i++)
		{
			EXPECT_NO_THROW(w->SetOutValue(8638356));
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		TimeSpec ts3;
		int32_t v3;
		EXPECT_NO_THROW(v3 = r->get_pokewire()->PeekOutValue(ts2));
		EXPECT_EQ(v3, 8638356);
	}

	void ServiceTestClient2::AsyncTestWirePeekPoke()
	{
		EXPECT_NO_THROW(r->get_peekwire()->AsyncPeekInValue(boost::bind(&ServiceTestClient2::AsyncTestWirePeekPoke1, this, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3))));

		async_peekpoke_evt.WaitOne(5000);

		EXPECT_FALSE(async_peekpoke_err);
	}

	void ServiceTestClient2::AsyncTestWirePeekPoke1(const int32_t& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err)
	{
		EXPECT_FALSE(err) << err->what();
		if (err)
		{
			async_peekpoke_err = err;
			async_peekpoke_evt.Set();
			return;
		}

		EXPECT_EQ(value, 56295674);
		if (value != 56295674)
		{
			async_peekpoke_err = RR_MAKE_SHARED<RobotRaconteurRemoteException>("std::runtime_error","");
			async_peekpoke_evt.Set();
			return;
		}

		EXPECT_NO_THROW(r->get_pokewire()->AsyncPokeOutValue(75738261, boost::bind(&ServiceTestClient2::AsyncTestWirePeekPoke2, this, RR_BOOST_PLACEHOLDERS(_1))));
	}

	void ServiceTestClient2::AsyncTestWirePeekPoke2(RR_SHARED_PTR<RobotRaconteurException> err)
	{
		EXPECT_FALSE(err) << err->what();
		if (err)
		{
			async_peekpoke_err = err;
			async_peekpoke_evt.Set();
			return;
		}

		EXPECT_NO_THROW(r->get_pokewire()->AsyncPeekOutValue(boost::bind(&ServiceTestClient2::AsyncTestWirePeekPoke3, this, RR_BOOST_PLACEHOLDERS(_1), RR_BOOST_PLACEHOLDERS(_2), RR_BOOST_PLACEHOLDERS(_3))));
	}

	void ServiceTestClient2::AsyncTestWirePeekPoke3(const int32_t& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err)
	{
		EXPECT_FALSE(err) << err->what();
		if (err)
		{
			async_peekpoke_err = err;
			async_peekpoke_evt.Set();
			return;
		}

		EXPECT_EQ(value, 75738261);
		if (value != 75738261)
		{
			async_peekpoke_err = RR_MAKE_SHARED<RobotRaconteurRemoteException>("std::runtime_error", "");
			async_peekpoke_evt.Set();
			return;
		}

		async_peekpoke_evt.Set();
	}

	void ServiceTestClient2::TestEnums()
	{
		EXPECT_NO_THROW(EXPECT_EQ(r->get_testenum1_prop(), com::robotraconteur::testing::TestService3::testenum1::anothervalue));
		EXPECT_NO_THROW(r->set_testenum1_prop(com::robotraconteur::testing::TestService3::testenum1::hexval1));

	}

	void ServiceTestClient2::TestPods()
	{	
		SCOPED_TRACE("s0");
		com::robotraconteur::testing::TestService3::testpod1 s0;
		ServiceTest2_fill_testpod1(s0, 563921043);
		ServiceTest2_verify_testpod1(s0, 563921043);
		
		com::robotraconteur::testing::TestService3::testpod1 s1;
		EXPECT_NO_THROW(s1 =  r->get_testpod1_prop());
		SCOPED_TRACE("s1");
		ServiceTest2_verify_testpod1(s1, 563921043);
		com::robotraconteur::testing::TestService3::testpod1 s2;
		ServiceTest2_fill_testpod1(s2, 85932659);
		EXPECT_NO_THROW(r->set_testpod1_prop(s2));
		com::robotraconteur::testing::TestService3::testpod1 s3;
		EXPECT_NO_THROW(s3 = r->testpod1_func2());
		SCOPED_TRACE("s3");
		ServiceTest2_verify_testpod1(s3, 95836295);
		com::robotraconteur::testing::TestService3::testpod1 s4;
		ServiceTest2_fill_testpod1(s4, 29546592);
		EXPECT_NO_THROW(r->testpod1_func1(s4));
		SCOPED_TRACE("s4");
		ServiceTest2_verify_teststruct3(r->get_teststruct3_prop(), 16483675);
		EXPECT_NO_THROW(r->set_teststruct3_prop(ServiceTest2_fill_teststruct3(858362)));
	}

	void ServiceTestClient2::TestMemories()
	{
		test_m1();
		test_m2();
	}

	void ServiceTestClient2::test_m1()
	{
		RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod2> > s
			= AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(32);
		

		for (size_t i = 0; i < s->size(); i++)
		{
			ServiceTest2_fill_testpod2(s->at(i), 59174 + i);
		}

		EXPECT_NO_THROW(EXPECT_EQ(r->get_pod_m1()->Length(), 1024));
		EXPECT_NO_THROW(r->get_pod_m1()->Write(52, s, 3, 17));

		RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod2> > s2
			= AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(32);
		
		EXPECT_NO_THROW(r->get_pod_m1()->Read(53, s2, 2, 16));

		for (size_t i = 2; i < 16; i++)
		{
			SCOPED_TRACE("s2");
			ServiceTest2_verify_testpod2(s2->at(i), 59174 + i +2);
		}
	}

	void ServiceTestClient2::test_m2()
	{
		RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2> > s
			= AllocateEmptyRRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2>();
		uint32_t s_dims[] = { 3, 3 };
		s->Dims = AttachRRArrayCopy<uint32_t>(s_dims, 2);
		s->PodArray = AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(9);
		

		for (size_t i = 0; i < s->PodArray->size(); i++)
		{
			ServiceTest2_fill_testpod2(s->PodArray->at(i), 75721 + i);
		}

		std::vector<uint64_t> z;
		z.push_back(0);
		z.push_back(0);
		std::vector<uint64_t> c;
		c.push_back(3);
		c.push_back(3);
		EXPECT_NO_THROW(r->get_pod_m2()->Write(z, s, z, c));

		RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2> > s2
			= AllocateEmptyRRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod2>();
		s2->Dims = AttachRRArrayCopy<uint32_t>(s_dims, 2);
		s2->PodArray = AllocateEmptyRRPodArray<com::robotraconteur::testing::TestService3::testpod2>(9);
		
		EXPECT_NO_THROW(r->get_pod_m2()->Read(z, s2, z, c));

		for (size_t i = 0; i < 9; i++)
		{
			SCOPED_TRACE("s2");
			ServiceTest2_verify_testpod2(s2->PodArray->at(i), 75721 + i);
		}
	}

	void ServiceTestClient2::TestGenerators()
	{
		RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<RRArray<uint8_t > >, RR_INTRUSIVE_PTR<RRArray<uint8_t > > > > gen;
		ASSERT_NO_THROW(gen = r->gen_func4());
		uint8_t a[] = { 2,3,4 };
		for (size_t i = 0; i < 6; i++)
		{
			EXPECT_NO_THROW(gen->Next(AttachRRArrayCopy(a, 3)));
		}
		
		RR_INTRUSIVE_PTR<RRArray<uint8_t> > b;
		ASSERT_NO_THROW(b = gen->Next(AttachRRArrayCopy(a, 3)));
		EXPECT_NO_THROW(gen->Abort());
		EXPECT_THROW(gen->Next(AttachRRArrayCopy(a, 3)),OperationAbortedException);
		

		std::vector<double> f1;
		EXPECT_NO_THROW(f1 = r->gen_func1()->NextAll());

		RR_SHARED_PTR<Generator<RR_INTRUSIVE_PTR<RRArray<uint8_t > >, RR_INTRUSIVE_PTR<RRArray<uint8_t > > > > gen2;
		ASSERT_NO_THROW(gen2 = r->gen_func4());
		
		ASSERT_NO_THROW(gen2->Next(AttachRRArrayCopy(a, 3)));
		EXPECT_NO_THROW(gen2->Close());
		
		EXPECT_THROW(gen2->Next(AttachRRArrayCopy(a, 3)),StopIterationException);
	}

	void ServiceTestClient2::TestNamedArrays()
	{
		com::robotraconteur::testing::TestService3::transform a1;		
		ServiceTest2_fill_transform(a1, 3956378);
		EXPECT_NO_THROW(r->set_testnamedarray1(a1.s.translation));

		com::robotraconteur::testing::TestService3::transform a1_1;
		a1_1.s.rotation = a1.s.rotation;
		EXPECT_NO_THROW(a1_1.s.translation = r->get_testnamedarray1());
		com::robotraconteur::testing::TestService3::transform a1_2;
		ServiceTest2_fill_transform(a1_2, 74637);
		a1_1.s.rotation = a1_2.s.rotation;
		SCOPED_TRACE("a1_1");
		ServiceTest2_verify_transform(a1_1, 74637);

		com::robotraconteur::testing::TestService3::transform a2;
		ServiceTest2_fill_transform(a2, 827635);
		EXPECT_NO_THROW(r->set_testnamedarray2(a2));

		com::robotraconteur::testing::TestService3::transform a2_1;
		EXPECT_NO_THROW(a2_1 = r->get_testnamedarray2());
		SCOPED_TRACE("a2_1");
		ServiceTest2_verify_transform(a2_1, 1294);

		EXPECT_NO_THROW(r->set_testnamedarray3(ServiceTest2_fill_transform_array(6, 19274)));
		SCOPED_TRACE("get_testnamedarray3");
		EXPECT_NO_THROW(ServiceTest2_verify_transform_array(r->get_testnamedarray3(), 8, 837512));

		EXPECT_NO_THROW(r->set_testnamedarray4(ServiceTest2_fill_transform_multidimarray(5, 2, 6385)));
		SCOPED_TRACE("get_testnamedarray4");
		EXPECT_NO_THROW(ServiceTest2_verify_transform_multidimarray(r->get_testnamedarray4(), 7, 2, 66134));

		EXPECT_NO_THROW(r->set_testnamedarray5(ServiceTest2_fill_transform_multidimarray(3, 2, 7732)));
		SCOPED_TRACE("get_testnamedarray5");
		EXPECT_NO_THROW(ServiceTest2_verify_transform_multidimarray(r->get_testnamedarray5(), 3, 2, 773142));
				
	}

	void ServiceTestClient2::TestNamedArrayMemories()
	{
		test_namedarray_m1();
		test_namedarray_m2();
	}

	void ServiceTestClient2::test_namedarray_m1()
	{
		RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > s
			= AllocateEmptyRRNamedArray<com::robotraconteur::testing::TestService3::transform>(32);
		
		for (size_t i = 0; i < s->size(); i++)
		{
			ServiceTest2_fill_transform((*s)[i], 79174 + i);
		}

		EXPECT_NO_THROW(EXPECT_EQ(r->get_namedarray_m1()->Length(), 512));
		EXPECT_NO_THROW(r->get_namedarray_m1()->Write(23, s, 3, 21));

		RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > s2
			= AllocateEmptyRRNamedArray<com::robotraconteur::testing::TestService3::transform>(32);
		
		EXPECT_NO_THROW(r->get_namedarray_m1()->Read(24, s2, 2, 18));

		for (size_t i = 2; i < 18; i++)
		{
			SCOPED_TRACE("s2");
			ServiceTest2_verify_transform((*s2)[i], 79174 + i + 2);
		}
	}

	void ServiceTestClient2::test_namedarray_m2()
	{
		std::vector<uint32_t> s_dims = boost::assign::list_of(3)(3);
		RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > s
			= AllocateEmptyRRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform>(s_dims);
		
		
		for (size_t i = 0; i < s->NamedArray->size(); i++)
		{
			ServiceTest2_fill_transform((*s->NamedArray)[i], 15721 + i);
		}

		std::vector<uint64_t> z = boost::assign::list_of(0)(0);		
		std::vector<uint64_t> c = boost::assign::list_of(3)(3);		
		EXPECT_NO_THROW(r->get_namedarray_m2()->Write(z, s, z, c));

		RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > s2
			= AllocateEmptyRRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> (s_dims);
		
		EXPECT_NO_THROW(r->get_namedarray_m2()->Read(z, s2, z, c));

		for (size_t i = 0; i < 9; i++)
		{
			SCOPED_TRACE("s2->NamedArray");
			ServiceTest2_verify_transform((*s2->NamedArray)[i], 15721 + i);
		}
	}

	void ServiceTestClient2::TestComplex()
	{
		cdouble c1_1(5.708705e+01, -2.328294e-03 );
		EXPECT_EQ(r->get_c1(), c1_1);

		cdouble c1_2(5.708705e+01, -2.328294e-03 );
		EXPECT_NO_THROW(r->set_c1(c1_2));

		RR_INTRUSIVE_PTR<RRArray<cdouble> > c2_1;
		ASSERT_NO_THROW(c2_1 = r->get_c2());
		double c2_1_1[] = { 1.968551e+07, 2.380643e+18, 3.107374e-16, 7.249542e-16, -4.701135e-19, -6.092764e-17, 2.285854e+14, 2.776180e+05, -1.436152e-12, 3.626609e+11, 3.600952e-02, -3.118123e-16, -1.312210e-10, -1.738940e-07, -1.476586e-12, -2.899781e-20, 4.806642e+03, 4.476869e-05, -2.935084e-16, 3.114019e-20, -3.675955e+01, 3.779796e-21, 2.190594e-11, 4.251420e-06, -9.715221e+11, -3.483924e-01, 7.606428e+05, 5.418088e+15, 4.786378e+16, -1.202581e+08, -1.662061e+02, -2.392954e+03 };
		EXPECT_RRARRAY_EQ(c2_1, AttachRRArrayCopy<cdouble>((cdouble*)c2_1_1, 16));

		double c2_2_1[] = { 4.925965e-03, 5.695254e+13, -4.576890e-14, -6.056342e-07, -4.918571e-08, -1.940684e-10, 1.549104e-02, -1.954145e+04, -2.499019e-16, 4.010614e+09, -1.906811e-08, 3.297924e-10, 2.742399e-02, -4.372839e-01, -3.093171e-10, 4.311755e-01, -2.218220e-14, 5.399758e+10, 3.360304e+17, 1.340681e-18, -4.441140e+11, -1.845055e-09, -3.074586e-10, -1.754926e+01, -2.766799e+04, -2.307577e+10, 2.754875e+14, 1.179639e+15, 6.976204e-10, 1.901856e+08, -3.824351e-02, -1.414167e+08 };
		RR_INTRUSIVE_PTR<RRArray<cdouble> > c2_2 = AttachRRArrayCopy((cdouble*)c2_2_1,16);
		EXPECT_NO_THROW(r->set_c2(c2_2));

		RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> > c3_1;
		ASSERT_NO_THROW(c3_1 = r->get_c3());
		uint32_t c3_1_1[] = { 2,5 };
		double c3_1_2[] = { 5.524802e+18, -2.443857e-05, 3.737932e-02, -4.883553e-03, -1.184347e+12, 4.537366e-08, -4.567913e-01, -1.683542e+15, -1.676517e+00, -8.911085e+12, -2.537376e-17, 1.835687e-10, -9.366069e-22, -5.426323e-12, -7.820969e-10, -1.061541e+12, -3.660854e-12, -4.969930e-03, 1.988428e+07, 1.860782e-16 };
		EXPECT_RRARRAY_EQ(c3_1->Dims, AttachRRArray(c3_1_1, 2,false));
		EXPECT_RRARRAY_EQ(c3_1->Array, AttachRRArray((cdouble*)c3_1_2, 10, false));

		uint32_t c3_2_1[] = { 3,4 };
		double c3_2_2[] = { 4.435180e+04, 5.198060e-18, -1.316737e-13, -4.821771e-03, -4.077550e-19, -1.659105e-09, -6.332363e-11, -1.128999e+16, 4.869912e+16, 2.680490e-04, -8.880119e-04, 3.960452e+11, 4.427784e-09, -2.813742e-18, 7.397516e+18, 1.196394e+13, 3.236906e-14, -4.219297e-17, 1.316282e-06, -2.771084e-18, -1.239118e-09, 2.887453e-08, -1.746515e+08, -2.312264e-11 };
		EXPECT_NO_THROW(r->set_c3(AllocateRRMultiDimArray<cdouble>(AttachRRArrayCopy(c3_2_1,2),AttachRRArrayCopy((cdouble*)c3_2_2,12))));

		RR_INTRUSIVE_PTR<RRList<RRArray<cdouble> > > c5_1;
		ASSERT_NO_THROW(c5_1 = r->get_c5());
		ASSERT_TRUE(c5_1);
		double c5_1_1[] = { 1.104801e+00, 4.871266e-10, -2.392938e-03, 4.210339e-07, 1.474114e-19, -1.147137e-01, -2.026434e+06, 4.450447e-19, 3.702953e-21, 9.722025e+12, 3.464073e-14, 4.628110e+15, 2.345453e-19, 3.730012e-04, 4.116650e+16, 4.380220e+08 };
		EXPECT_RRARRAY_EQ(c5_1->front(), AttachRRArray<cdouble>((cdouble*)c5_1_1, 8, false));
		
		RR_INTRUSIVE_PTR<RRList<RRArray<cdouble> > > c5_2 = AllocateEmptyRRList<RRArray<cdouble> >();
		double c5_2_1[] = { 2.720831e-20, 2.853037e-16, -7.982497e+16, -2.684318e-09, -2.505796e+17, -4.743970e-12, -3.657056e+11, 2.718388e+15, 1.597672e+03, 2.611859e+14, 2.224926e+06, -1.431096e-09, 3.699894e+19, -5.936706e-01, -1.385395e-09, -4.248415e-13 };
		c5_2->push_back(AttachRRArrayCopy((cdouble*)c5_2_1, 8));
		EXPECT_NO_THROW(r->set_c5(c5_2));
		
		cfloat c7_1(-5.527021e-18, -9.848457e+03 );
		EXPECT_NO_THROW(EXPECT_EQ(r->get_c7(), c7_1));

		cfloat c7_2(9.303345e-12, -3.865684e-05 );
		EXPECT_NO_THROW(r->set_c7(c7_2));

		RR_INTRUSIVE_PTR<RRArray<cfloat> > c8_1;
		ASSERT_NO_THROW(c8_1 = r->get_c8());
		float c8_1_1[] = { -3.153395e-09, 3.829492e-02, -2.665239e+12, 1.592927e-03, 3.188444e+06, -3.595015e-11, 2.973887e-18, -2.189921e+17, 1.651567e+10, 1.095838e+05, 3.865249e-02, 4.725510e+10, -2.334376e+03, 3.744977e-05, -1.050821e+02, 1.122660e-22, 3.501520e-18, -2.991601e-17, 6.039622e-17, 4.778095e-07, -4.793136e-05, 3.096513e+19, 2.476004e+18, 1.296297e-03, 2.165336e-13, 4.834427e+06, 4.675370e-01, -2.942290e-12, -2.090883e-19, 6.674942e+07, -4.809047e-10, -4.911772e-13 };
		EXPECT_RRARRAY_EQ(c8_1, AttachRRArrayCopy<cfloat>((cfloat*)c8_1_1, 16));

		float c8_2_1[] = { 1.324498e+06, 1.341746e-04, 4.292993e-04, -3.844509e+15, -3.804802e+10, 3.785305e-12, 2.628285e-19, -1.664089e+15, -4.246472e-10, -3.334943e+03, -3.305796e-01, 1.878648e-03, 1.420880e-05, -3.024657e+14, 2.227031e-21, 2.044653e+17, 9.753609e-20, -6.581817e-03, 3.271063e-03, -1.726081e+06, -1.614502e-06, -2.641638e-19, -2.977317e+07, -1.278224e+03, -1.760207e-05, -4.877944e-07, -2.171524e+02, 1.620645e+01, -4.334168e-02, 1.871011e-09, -3.066163e+06, -3.533662e+07 };
		RR_INTRUSIVE_PTR<RRArray<cfloat> > c8_2 = AttachRRArrayCopy((cfloat*)c8_2_1, 16);
		EXPECT_NO_THROW(r->set_c8(c8_2));

		RR_INTRUSIVE_PTR<RRMultiDimArray<cfloat > > c9_1;
		ASSERT_NO_THROW(c9_1 = r->get_c9());
		uint32_t c9_1_1[] = { 2,4 };
		float c9_1_2[] = { 1.397743e+15, 3.933042e+10, -3.812329e+07, 1.508109e+16, -2.091397e-20, 3.207851e+12, -3.640702e+02, 3.903769e+02, -2.879727e+17, -4.589604e-06, 2.202769e-06, 2.892523e+04, -3.306489e-14, 4.522308e-06, 1.665807e+15, 2.340476e+10 };
		EXPECT_RRARRAY_EQ(c9_1->Dims, AttachRRArray(c9_1_1, 2, false));
		EXPECT_RRARRAY_EQ(c9_1->Array, AttachRRArray((cfloat*)c9_1_2, 8, false));

		uint32_t c9_2_1[] = { 2,2,2 };
		float c9_2_2[] = { 2.138322e-03, 4.036979e-21, 1.345236e+10, -1.348460e-12, -3.615340e+12, -2.911340e-21, 3.220362e+09, 3.459909e-04, 4.276259e-08, -3.199451e+18, 3.468308e+07, -2.928506e-09, -3.154288e+17, -2.352920e-02, 6.976385e-21, 2.435472e+12 };
		EXPECT_NO_THROW(r->set_c9(AllocateRRMultiDimArray<cfloat>(AttachRRArrayCopy(c9_2_1, 3), AttachRRArrayCopy((cfloat*)c9_2_2, 8))));


	}

	void ServiceTestClient2::TestComplexMemories()
	{
		double c_m1_1[] = { 8.952764e-05, 4.348213e-04, -1.051215e+08, 1.458626e-09, -2.575954e+10, 2.118740e+03, -2.555026e-02, 2.192576e-18, -2.035082e+18, 2.951834e-09, -1.760731e+15, 4.620903e-11, -3.098798e+05, -8.883556e-07, 2.472289e+17, 7.059075e-12 };
		RR_INTRUSIVE_PTR<RRArray<cdouble> > c_m1_2 = AttachRRArrayCopy((cdouble*)c_m1_1, 8);
		EXPECT_NO_THROW(r->get_c_m1()->Write(10, c_m1_2, 0, 8));

		RR_INTRUSIVE_PTR<RRArray<cdouble> > c_m1_3 = AllocateRRArray<cdouble>(8);
		EXPECT_NO_THROW(r->get_c_m1()->Read(10, c_m1_3, 0, 8));

		EXPECT_RRARRAY_EQ(c_m1_2, c_m1_3);

		std::vector<uint64_t> z = boost::assign::list_of(0)(0);
		std::vector<uint64_t> c = boost::assign::list_of(3)(3);

		uint32_t c_m2_1[] = { 3,3 };
		double c_m2_2[] = { -4.850043e-03, 3.545429e-07, 2.169430e+12, 1.175943e-09, 2.622300e+08, -4.439823e-11, -1.520489e+17, 8.250078e-14, 3.835439e-07, -1.424709e-02, 3.703099e+08, -1.971111e-08, -2.805354e+01, -2.093850e-17, -4.476148e+19, 9.914350e+11, 2.753067e+08, -1.745041e+14 };
		RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> > c_m2_3 = AllocateRRMultiDimArray<cdouble> (AttachRRArrayCopy(c_m2_1,2), AttachRRArrayCopy((cdouble*)c_m2_2, 9));
		EXPECT_NO_THROW(r->get_c_m2()->Write(z, c_m2_3, z, c));

		RR_INTRUSIVE_PTR<RRMultiDimArray<cdouble> > c_m2_4 = AllocateRRMultiDimArray<cdouble>(AttachRRArrayCopy(c_m2_1, 2), AllocateRRArray<cdouble>(9));
		EXPECT_NO_THROW(r->get_c_m2()->Read(z, c_m2_4, z, c));

		EXPECT_RRARRAY_EQ(c_m2_3->Dims, c_m2_4->Dims);
		EXPECT_RRARRAY_EQ(c_m2_3->Array, c_m2_4->Array);
	}

#define ShouldBeLockedErr(COMMAND) EXPECT_THROW(COMMAND,ObjectLockedException)

	void ServiceTestClient2::TestNoLock()
	{
		RR_SHARED_PTR< com::robotraconteur::testing::TestService3::obj5> o5 = r->get_nolock_test();

		ShouldBeLockedErr(o5->get_p1(););
		EXPECT_NO_THROW(o5->get_p2());
		EXPECT_NO_THROW(o5->set_p2(0));
		EXPECT_NO_THROW(o5->get_p3());
		ShouldBeLockedErr(o5->set_p1(0););

		ShouldBeLockedErr(o5->f1(););
		EXPECT_NO_THROW(o5->f2());

		ShouldBeLockedErr(o5->get_q1()->Connect(-1)->Close(););
		EXPECT_NO_THROW(o5->get_q2()->Connect(-1)->Close());
		ShouldBeLockedErr(o5->get_w1()->Connect()->Close(););
		EXPECT_NO_THROW(o5->get_w2()->Connect()->Close());

		ShouldBeLockedErr(o5->get_m1()->Length(););

		
		RR_INTRUSIVE_PTR<RRArray<int32_t> > b1 = AllocateRRArray<int32_t>(100);

		EXPECT_NO_THROW(o5->get_m2()->Length());
		EXPECT_NO_THROW(o5->get_m2()->Read(0, b1, 0, 10));
		EXPECT_NO_THROW(o5->get_m2()->Write(0, b1, 0, 10));

		EXPECT_NO_THROW(o5->get_m3()->Length());
		EXPECT_NO_THROW(o5->get_m3()->Read(0, b1, 0, 10));
		ShouldBeLockedErr(o5->get_m3()->Write(0, b1, 0, 10););
	}

	void ServiceTestClient2::TestBools()
	{
		EXPECT_NO_THROW(EXPECT_NE(r->get_b1().value, 0));
		EXPECT_NO_THROW(r->set_b1(1));

		rr_bool v2_1_1[] = { 1,0,1,1,0,1,0 };
		RR_INTRUSIVE_PTR<RRArray<rr_bool> > v2_1;
		ASSERT_NO_THROW(v2_1 = r->get_b2());
		EXPECT_RRARRAY_EQ(v2_1, AttachRRArray(v2_1_1, 7,false));
		
		rr_bool v2_2_1[] = { 1,0,0,1,1,1,0,1 };
		EXPECT_NO_THROW(r->set_b2(AttachRRArrayCopy(v2_2_1, 8)));

		rr_bool v3_1_1[] = { 0,1,1,0 };
		uint32_t v3_1_dims[] = { 2,2 };

		RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool > > v3_1;
		ASSERT_NO_THROW(v3_1 = r->get_b3());
		EXPECT_RRARRAY_EQ(v3_1->Dims, AttachRRArray(v3_1_dims, 2, false));
		EXPECT_RRARRAY_EQ(v3_1->Array, AttachRRArray(v3_1_1, 4, false));
		
		rr_bool v3_2_1[] = { 1, 0 };
		uint32_t v3_2_dims[] = { 2,1 };

		EXPECT_NO_THROW(r->set_b3(AllocateRRMultiDimArray(AttachRRArrayCopy(v3_2_dims, 2), AttachRRArrayCopy(v3_2_1, 2))));

		RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool >  > > v4_1;
		ASSERT_NO_THROW(v4_1 = r->get_b4());
		ASSERT_TRUE(v4_1);
		ASSERT_EQ(v4_1->size(), 1);
		ASSERT_EQ(RRArrayToScalar(v4_1->front()), 1);

		RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool >  > > v4_2;
		EXPECT_NO_THROW(v4_2 = AllocateEmptyRRList<RRArray<rr_bool> >());
		v4_2->push_back(ScalarToRRArray<rr_bool>(1));
		EXPECT_NO_THROW(r->set_b4(v4_2));

		RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool >  > > v5_1;
		ASSERT_NO_THROW(v5_1 = r->get_b5());
		rr_bool v5_1_1[] = { 0,1,0,0 };
		ASSERT_TRUE(v5_1);
		ASSERT_EQ(v5_1->size(), 1);
		EXPECT_RRARRAY_EQ(v5_1->front(), AttachRRArray(v5_1_1, 4, false));

		RR_INTRUSIVE_PTR<RRList<RRArray<rr_bool > > > v5_2 = AllocateEmptyRRList<RRArray<rr_bool> >();
		rr_bool v5_2_1[] ={ 1,0 };
		v5_2->push_back(AttachRRArrayCopy(v5_2_1,2));
		EXPECT_NO_THROW(r->set_b5(v5_2));

		RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<rr_bool >  > > v6_1;
		ASSERT_NO_THROW(v6_1 = r->get_b6());
		rr_bool v6_1_1[] = { 0,1,1,0 };
		uint32_t v6_1_dims[] = { 2,2 };
		ASSERT_TRUE(v6_1);
		EXPECT_RRARRAY_EQ(v6_1->front()->Dims, AttachRRArray(v6_1_dims, 2, false));
		EXPECT_RRARRAY_EQ(v6_1->front()->Array, AttachRRArray(v6_1_1, 4, false));

		rr_bool v6_2_1[] = { 1,0 };
		uint32_t v6_2_dims[] = { 2,1 };
		RR_INTRUSIVE_PTR<RRList<RRMultiDimArray<rr_bool >  > > v6_2 = AllocateEmptyRRList<RRMultiDimArray<rr_bool> >();
		v6_2->push_back(AllocateRRMultiDimArray(AttachRRArrayCopy(v6_2_dims, 2), AttachRRArrayCopy(v6_2_1, 2)));
		EXPECT_NO_THROW(r->set_b6(v6_2));
	}

	void ServiceTestClient2::TestBoolMemories()
	{
		RR_SHARED_PTR<ArrayMemory<rr_bool > > c_m5;
		ASSERT_NO_THROW(c_m5 = r->get_c_m5());
		
		rr_bool v1_1[] = { 1,0,0,1,1,0,0,0,1,1 };
		EXPECT_NO_THROW(c_m5->Write(100, AttachRRArrayCopy(v1_1, 10), 1, 8));
		RR_INTRUSIVE_PTR<RRArray<rr_bool> > v2 = AllocateRRArray<rr_bool>(10);
		EXPECT_NO_THROW(c_m5->Read(99, v2, 0, 10));
		for (size_t i = 1; i < 9; i++)
		{
			EXPECT_EQ(v2->at(i), v1_1[i]) << " at index " << i;
		}

		RR_SHARED_PTR<MultiDimArrayMemory<rr_bool > > c_m6;
		EXPECT_NO_THROW(c_m6 = r->get_c_m6());
		uint32_t v3_dims[] = { 2,5 };
		RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> > v3 = AllocateRRMultiDimArray(AttachRRArrayCopy(v3_dims, 2), AttachRRArrayCopy(v1_1, 10));

		std::vector<uint64_t> z = boost::assign::list_of(0)(0);
		std::vector<uint64_t> c = boost::assign::list_of(2)(5);

		EXPECT_NO_THROW(c_m6->Write(z, v3, z, c));
		
		std::vector<uint32_t> v4_dims = boost::assign::list_of(2)(5);

		RR_INTRUSIVE_PTR<RRMultiDimArray<rr_bool> > v4 = AllocateEmptyRRMultiDimArray<rr_bool>(v4_dims);
		EXPECT_NO_THROW(c_m6->Read(z, v4, z, c));
		EXPECT_RRARRAY_EQ(v3->Dims, v4->Dims);
		EXPECT_RRARRAY_EQ(v3->Array, v4->Array);

	}

	void ServiceTestClient2::TestExceptionParams()
	{
		bool exp1_caught = false;
		try
		{
			r->test_exception_params1();
		}
		catch (InvalidOperationException& exp)
		{
			exp1_caught = true;
			EXPECT_EQ(exp.Message, "test error");
			EXPECT_EQ(exp.ErrorSubName, "my_error");
			EXPECT_TRUE(exp.ErrorParam);
			if (exp.ErrorParam)
			{
			
				RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > param_map;
				param_map = RR_DYNAMIC_POINTER_CAST<RRMap<std::string,RRValue> >(exp.ErrorParam);
				ASSERT_TRUE(param_map);
				ASSERT_EQ(param_map->size(), 2);
				EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(rr_cast<RRArray<int32_t> >(param_map->at("param1"))), 10));
				EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(param_map->at("param2"))), "20"));
			}
		}
		EXPECT_TRUE(exp1_caught);

		bool exp2_caught = false;
		try
		{
			r->test_exception_params2();
		}
		catch (com::robotraconteur::testing::TestService3::test_exception4& exp)
		{
			exp2_caught = true;
			EXPECT_EQ(exp.Message, "test error2");
			EXPECT_EQ(exp.ErrorSubName, "my_error2");
			EXPECT_TRUE(exp.ErrorParam);

			if (exp.ErrorParam)
			{
				RR_INTRUSIVE_PTR<RRMap<std::string,RRValue> > param_map;
				param_map = RR_DYNAMIC_POINTER_CAST<RRMap<std::string,RRValue> >(exp.ErrorParam);
				ASSERT_TRUE(param_map);
				ASSERT_EQ(param_map->size(), 2);
				EXPECT_NO_THROW(EXPECT_EQ(RRArrayToScalar(rr_cast<RRArray<int32_t> >(param_map->at("param1"))), 30));
				EXPECT_NO_THROW(EXPECT_EQ(RRArrayToString(rr_cast<RRArray<char> >(param_map->at("param2"))), "40"));
			}
		}
		EXPECT_TRUE(exp2_caught);
	}
}