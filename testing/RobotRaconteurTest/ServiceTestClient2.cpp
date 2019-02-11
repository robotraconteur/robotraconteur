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
		TestCStructs();
		TestMemories();

		TestGenerators();

		TestAStructs();
		TestAStructMemories();

		TestComplex();
		TestComplexMemories();

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
		int32_t v = r->get_peekwire()->PeekInValue(ts);
		if (v != 56295674) throw std::runtime_error("");

		//Test Poke

		r->get_pokewire()->PokeOutValue(75738265);
		TimeSpec ts2;
		int32_t v2 = r->get_pokewire()->PeekOutValue(ts2);
		if (v2 != 75738265) throw std::runtime_error("");

		//Test WireUnicastReceiver

		RR_SHARED_PTR<WireConnection<int32_t> > w = r->get_pokewire()->Connect();
		for (size_t i = 0; i < 3; i++)
		{
			w->SetOutValue(8638356);
		}
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
		TimeSpec ts3;
		int32_t v3 = r->get_pokewire()->PeekOutValue(ts2);
		if (v3 != 8638356) throw std::runtime_error("");		
	}

	void ServiceTestClient2::AsyncTestWirePeekPoke()
	{
		r->get_peekwire()->AsyncPeekInValue(boost::bind(&ServiceTestClient2::AsyncTestWirePeekPoke1, this, _1, _2, _3));

		async_peekpoke_evt.WaitOne(5000);

		if (async_peekpoke_err)
		{
			throw std::runtime_error("");
		}
	}

	void ServiceTestClient2::AsyncTestWirePeekPoke1(const int32_t& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err)
	{
		if (err)
		{
			async_peekpoke_err = err;
			async_peekpoke_evt.Set();
			return;
		}

		if (value != 56295674)
		{
			async_peekpoke_err = RR_MAKE_SHARED<RobotRaconteurRemoteException>("std::runtime_error","");
			async_peekpoke_evt.Set();
			return;
		}

		r->get_pokewire()->AsyncPokeOutValue(75738261, boost::bind(&ServiceTestClient2::AsyncTestWirePeekPoke2, this, _1));
	}

	void ServiceTestClient2::AsyncTestWirePeekPoke2(RR_SHARED_PTR<RobotRaconteurException> err)
	{
		if (err)
		{
			async_peekpoke_err = err;
			async_peekpoke_evt.Set();
			return;
		}

		r->get_pokewire()->AsyncPeekOutValue(boost::bind(&ServiceTestClient2::AsyncTestWirePeekPoke3, this, _1, _2, _3));
	}

	void ServiceTestClient2::AsyncTestWirePeekPoke3(const int32_t& value, const TimeSpec& ts, RR_SHARED_PTR<RobotRaconteurException> err)
	{
		if (err)
		{
			async_peekpoke_err = err;
			async_peekpoke_evt.Set();
			return;
		}

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
		if (r->get_testenum1_prop() != com::robotraconteur::testing::TestService3::testenum1::anothervalue)
			throw std::runtime_error("");
		r->set_testenum1_prop(com::robotraconteur::testing::TestService3::testenum1::hexval1);

	}

	void ServiceTestClient2::TestCStructs()
	{		
		com::robotraconteur::testing::TestService3::testcstruct1 s0;
		ServiceTest2_fill_testcstruct1(s0, 563921043);
		ServiceTest2_verify_testcstruct1(s0, 563921043);
		

		com::robotraconteur::testing::TestService3::testcstruct1 s1 =  r->get_testcstruct1_prop();
		ServiceTest2_verify_testcstruct1(s1, 563921043);
		com::robotraconteur::testing::TestService3::testcstruct1 s2;
		ServiceTest2_fill_testcstruct1(s2, 85932659);
		r->set_testcstruct1_prop(s2);
		com::robotraconteur::testing::TestService3::testcstruct1 s3 = r->testcstruct1_func2();
		ServiceTest2_verify_testcstruct1(s3, 95836295);
		com::robotraconteur::testing::TestService3::testcstruct1 s4;
		ServiceTest2_fill_testcstruct1(s4, 29546592);
		r->testcstruct1_func1(s4);
		ServiceTest2_verify_teststruct3(r->get_teststruct3_prop(), 16483675);
		r->set_teststruct3_prop(ServiceTest2_fill_teststruct3(858362));
	}

	void ServiceTestClient2::TestMemories()
	{
		test_m1();
		test_m2();
	}

	void ServiceTestClient2::test_m1()
	{
		RR_SHARED_PTR<RRCStructureArray<com::robotraconteur::testing::TestService3::testcstruct2> > s
			= RR_MAKE_SHARED<RRCStructureArray<com::robotraconteur::testing::TestService3::testcstruct2> >();
		s->cstruct_array.resize(32);

		for (size_t i = 0; i < s->cstruct_array.size(); i++)
		{
			ServiceTest2_fill_testcstruct2(s->cstruct_array.at(i), 59174 + i);
		}

		if (r->get_cstruct_m1()->Length() != 1024) throw std::runtime_error("");
		r->get_cstruct_m1()->Write(52, s, 3, 17);

		RR_SHARED_PTR<RRCStructureArray<com::robotraconteur::testing::TestService3::testcstruct2> > s2
			= RR_MAKE_SHARED<RRCStructureArray<com::robotraconteur::testing::TestService3::testcstruct2> >();
		s2->cstruct_array.resize(32);
		r->get_cstruct_m1()->Read(53, s2, 2, 16);

		for (size_t i = 2; i < 16; i++)
		{
			ServiceTest2_verify_testcstruct2(s2->cstruct_array.at(i), 59174 + i +2);
		}
	}

	void ServiceTestClient2::test_m2()
	{
		RR_SHARED_PTR<RRCStructureMultiDimArray<com::robotraconteur::testing::TestService3::testcstruct2> > s
			= RR_MAKE_SHARED<RRCStructureMultiDimArray<com::robotraconteur::testing::TestService3::testcstruct2> >();
		uint32_t s_dims[] = { 3, 3 };
		s->Dims = AttachRRArrayCopy<uint32_t>(s_dims, 2);
		s->CStructArray = RR_MAKE_SHARED<RRCStructureArray<com::robotraconteur::testing::TestService3::testcstruct2> >();
		s->CStructArray->cstruct_array.resize(9);

		for (size_t i = 0; i < s->CStructArray->cstruct_array.size(); i++)
		{
			ServiceTest2_fill_testcstruct2(s->CStructArray->cstruct_array.at(i), 75721 + i);
		}

		std::vector<uint64_t> z;
		z.push_back(0);
		z.push_back(0);
		std::vector<uint64_t> c;
		c.push_back(3);
		c.push_back(3);
		r->get_cstruct_m2()->Write(z, s, z, c);

		RR_SHARED_PTR<RRCStructureMultiDimArray<com::robotraconteur::testing::TestService3::testcstruct2> > s2
			= RR_MAKE_SHARED<RRCStructureMultiDimArray<com::robotraconteur::testing::TestService3::testcstruct2> >();
		s2->Dims = AttachRRArrayCopy<uint32_t>(s_dims, 2);
		s2->CStructArray = RR_MAKE_SHARED<RRCStructureArray<com::robotraconteur::testing::TestService3::testcstruct2> >();
		s2->CStructArray->cstruct_array.resize(9);
		r->get_cstruct_m2()->Read(z, s2, z, c);

		for (size_t i = 0; i < 9; i++)
		{
			ServiceTest2_verify_testcstruct2(s2->CStructArray->cstruct_array.at(i), 75721 + i);
		}
	}

	void ServiceTestClient2::TestGenerators()
	{
		RR_SHARED_PTR<Generator<RR_SHARED_PTR<RRArray<uint8_t > >, RR_SHARED_PTR<RRArray<uint8_t > > > > gen = r->gen_func4();
		uint8_t a[] = { 2,3,4 };
		for (size_t i = 0; i < 6; i++)
		{
			gen->Next(AttachRRArrayCopy(a, 3));
		}
		
		RR_SHARED_PTR<RRArray<uint8_t> > b = gen->Next(AttachRRArrayCopy(a, 3));
		gen->Abort();
		try
		{
			gen->Next(AttachRRArrayCopy(a, 3));
		}
		catch (OperationAbortedException&)
		{
			std::cout << "Caught aborted" << std::endl;
		}

		std::vector<double> f1 = r->gen_func1()->NextAll();

		RR_SHARED_PTR<Generator<RR_SHARED_PTR<RRArray<uint8_t > >, RR_SHARED_PTR<RRArray<uint8_t > > > > gen2 = r->gen_func4();
		
		gen2->Next(AttachRRArrayCopy(a, 3));
		gen2->Close();
		try
		{
			gen2->Next(AttachRRArrayCopy(a, 3));
		}
		catch (StopIterationException&)
		{
			std::cout << "Caught stop" << std::endl;
		}

		cout << "" << endl;
	}

	void ServiceTestClient2::TestAStructs()
	{
		com::robotraconteur::testing::TestService3::transform a1;		
		ServiceTest2_fill_transform(a1, 3956378);
		r->set_testastruct1(a1.s.translation);

		com::robotraconteur::testing::TestService3::transform a1_1;
		a1_1.s.rotation = a1.s.rotation;
		a1_1.s.translation = r->get_testastruct1();
		com::robotraconteur::testing::TestService3::transform a1_2;
		ServiceTest2_fill_transform(a1_2, 74637);
		a1_1.s.rotation = a1_2.s.rotation;
		ServiceTest2_verify_transform(a1_1, 74637);

		com::robotraconteur::testing::TestService3::transform a2;
		ServiceTest2_fill_transform(a2, 827635);
		r->set_testastruct2(a2);

		com::robotraconteur::testing::TestService3::transform a2_1
			= r->get_testastruct2();
		ServiceTest2_verify_transform(a2_1, 1294);

		r->set_testastruct3(ServiceTest2_fill_transform_array(6, 19274));
		ServiceTest2_verify_transform_array(r->get_testastruct3(), 8, 837512);

		r->set_testastruct4(ServiceTest2_fill_transform_multidimarray(5, 2, 6385));
		ServiceTest2_verify_transform_multidimarray(r->get_testastruct4(), 7, 2, 66134);

		r->set_testastruct5(ServiceTest2_fill_transform_multidimarray(3, 2, 7732));
		ServiceTest2_verify_transform_multidimarray(r->get_testastruct5(), 3, 2, 773142);
				
	}

	void ServiceTestClient2::TestAStructMemories()
	{
		test_astruct_m1();
		test_astruct_m2();
	}

	void ServiceTestClient2::test_astruct_m1()
	{
		RR_SHARED_PTR<RRAStructureArray<com::robotraconteur::testing::TestService3::transform> > s
			= AllocateEmptyRRAStructureArray<com::robotraconteur::testing::TestService3::transform>(32);
		
		for (size_t i = 0; i < s->Length(); i++)
		{
			ServiceTest2_fill_transform((*s)[i], 79174 + i);
		}

		if (r->get_astruct_m1()->Length() != 512) throw std::runtime_error("");
		r->get_astruct_m1()->Write(23, s, 3, 21);

		RR_SHARED_PTR<RRAStructureArray<com::robotraconteur::testing::TestService3::transform> > s2
			= AllocateEmptyRRAStructureArray<com::robotraconteur::testing::TestService3::transform>(32);
		
		r->get_astruct_m1()->Read(24, s2, 2, 18);

		for (size_t i = 2; i < 18; i++)
		{
			ServiceTest2_verify_transform((*s2)[i], 79174 + i + 2);
		}
	}

	void ServiceTestClient2::test_astruct_m2()
	{
		std::vector<uint32_t> s_dims = boost::assign::list_of(3)(3);
		RR_SHARED_PTR<RRAStructureMultiDimArray<com::robotraconteur::testing::TestService3::transform> > s
			= AllocateEmptyRRAStructureMultiDimArray<com::robotraconteur::testing::TestService3::transform>(s_dims);
		
		
		for (size_t i = 0; i < s->AStructArray->Length(); i++)
		{
			ServiceTest2_fill_transform((*s->AStructArray)[i], 15721 + i);
		}

		std::vector<uint64_t> z = boost::assign::list_of(0)(0);		
		std::vector<uint64_t> c = boost::assign::list_of(3)(3);		
		r->get_astruct_m2()->Write(z, s, z, c);

		RR_SHARED_PTR<RRAStructureMultiDimArray<com::robotraconteur::testing::TestService3::transform> > s2
			= AllocateEmptyRRAStructureMultiDimArray<com::robotraconteur::testing::TestService3::transform> (s_dims);
		
		r->get_astruct_m2()->Read(z, s2, z, c);

		for (size_t i = 0; i < 9; i++)
		{
			ServiceTest2_verify_transform((*s2->AStructArray)[i], 15721 + i);
		}
	}

	void ServiceTestClient2::TestComplex()
	{
		cdouble c1_1 = { 5.708705e+01, -2.328294e-03 };
		if (r->get_c1() != c1_1) throw std::runtime_error("");

		cdouble c1_2 = { 5.708705e+01, -2.328294e-03 };
		r->set_c1(c1_2);

		RR_SHARED_PTR<RRArray<cdouble> > c2_1 = r->get_c2();
		double c2_1_1[] = { 1.968551e+07, 2.380643e+18, 3.107374e-16, 7.249542e-16, -4.701135e-19, -6.092764e-17, 2.285854e+14, 2.776180e+05, -1.436152e-12, 3.626609e+11, 3.600952e-02, -3.118123e-16, -1.312210e-10, -1.738940e-07, -1.476586e-12, -2.899781e-20, 4.806642e+03, 4.476869e-05, -2.935084e-16, 3.114019e-20, -3.675955e+01, 3.779796e-21, 2.190594e-11, 4.251420e-06, -9.715221e+11, -3.483924e-01, 7.606428e+05, 5.418088e+15, 4.786378e+16, -1.202581e+08, -1.662061e+02, -2.392954e+03 };
		ca(c2_1, AttachRRArrayCopy<cdouble>((cdouble*)c2_1_1, 16));

		double c2_2_1[] = { 4.925965e-03, 5.695254e+13, -4.576890e-14, -6.056342e-07, -4.918571e-08, -1.940684e-10, 1.549104e-02, -1.954145e+04, -2.499019e-16, 4.010614e+09, -1.906811e-08, 3.297924e-10, 2.742399e-02, -4.372839e-01, -3.093171e-10, 4.311755e-01, -2.218220e-14, 5.399758e+10, 3.360304e+17, 1.340681e-18, -4.441140e+11, -1.845055e-09, -3.074586e-10, -1.754926e+01, -2.766799e+04, -2.307577e+10, 2.754875e+14, 1.179639e+15, 6.976204e-10, 1.901856e+08, -3.824351e-02, -1.414167e+08 };
		RR_SHARED_PTR<RRArray<cdouble> > c2_2 = AttachRRArrayCopy((cdouble*)c2_2_1,16);
		r->set_c2(c2_2);

		RR_SHARED_PTR<RRMultiDimArray<cdouble> > c3_1 = r->get_c3();
		uint32_t c3_1_1[] = { 2,5 };
		double c3_1_2[] = { 5.524802e+18, -2.443857e-05, 3.737932e-02, -4.883553e-03, -1.184347e+12, 4.537366e-08, -4.567913e-01, -1.683542e+15, -1.676517e+00, -8.911085e+12, -2.537376e-17, 1.835687e-10, -9.366069e-22, -5.426323e-12, -7.820969e-10, -1.061541e+12, -3.660854e-12, -4.969930e-03, 1.988428e+07, 1.860782e-16 };
		ca(c3_1->Dims, AttachRRArray(c3_1_1, 2,false));
		ca(c3_1->Array, AttachRRArray((cdouble*)c3_1_2, 10, false));

		uint32_t c3_2_1[] = { 3,4 };
		double c3_2_2[] = { 4.435180e+04, 5.198060e-18, -1.316737e-13, -4.821771e-03, -4.077550e-19, -1.659105e-09, -6.332363e-11, -1.128999e+16, 4.869912e+16, 2.680490e-04, -8.880119e-04, 3.960452e+11, 4.427784e-09, -2.813742e-18, 7.397516e+18, 1.196394e+13, 3.236906e-14, -4.219297e-17, 1.316282e-06, -2.771084e-18, -1.239118e-09, 2.887453e-08, -1.746515e+08, -2.312264e-11 };
		r->set_c3(RR_MAKE_SHARED<RRMultiDimArray<cdouble> >(AttachRRArrayCopy(c3_2_1,2),AttachRRArrayCopy((cdouble*)c3_2_2,12)));

		RR_SHARED_PTR<RRList<RRArray<cdouble> > > c5_1 = r->get_c5();
		if (!c5_1) throw std::runtime_error("");
		double c5_1_1[] = { 1.104801e+00, 4.871266e-10, -2.392938e-03, 4.210339e-07, 1.474114e-19, -1.147137e-01, -2.026434e+06, 4.450447e-19, 3.702953e-21, 9.722025e+12, 3.464073e-14, 4.628110e+15, 2.345453e-19, 3.730012e-04, 4.116650e+16, 4.380220e+08 };
		ca(c5_1->list.at(0), AttachRRArray<cdouble>((cdouble*)c5_1_1, 8, false));
		
		RR_SHARED_PTR<RRList<RRArray<cdouble> > > c5_2 = RR_MAKE_SHARED<RRList<RRArray<cdouble> > >();
		double c5_2_1[] = { 2.720831e-20, 2.853037e-16, -7.982497e+16, -2.684318e-09, -2.505796e+17, -4.743970e-12, -3.657056e+11, 2.718388e+15, 1.597672e+03, 2.611859e+14, 2.224926e+06, -1.431096e-09, 3.699894e+19, -5.936706e-01, -1.385395e-09, -4.248415e-13 };
		c5_2->list.push_back(AttachRRArrayCopy((cdouble*)c5_2_1, 8));
		r->set_c5(c5_2);
		
		cfloat c7_1 = { -5.527021e-18, -9.848457e+03 };
		if (r->get_c7() != c7_1) throw std::runtime_error("");

		cfloat c7_2 = { 9.303345e-12, -3.865684e-05 };
		r->set_c7(c7_2);

		RR_SHARED_PTR<RRArray<cfloat> > c8_1 = r->get_c8();
		float c8_1_1[] = { -3.153395e-09, 3.829492e-02, -2.665239e+12, 1.592927e-03, 3.188444e+06, -3.595015e-11, 2.973887e-18, -2.189921e+17, 1.651567e+10, 1.095838e+05, 3.865249e-02, 4.725510e+10, -2.334376e+03, 3.744977e-05, -1.050821e+02, 1.122660e-22, 3.501520e-18, -2.991601e-17, 6.039622e-17, 4.778095e-07, -4.793136e-05, 3.096513e+19, 2.476004e+18, 1.296297e-03, 2.165336e-13, 4.834427e+06, 4.675370e-01, -2.942290e-12, -2.090883e-19, 6.674942e+07, -4.809047e-10, -4.911772e-13 };
		ca(c8_1, AttachRRArrayCopy<cfloat>((cfloat*)c8_1_1, 16));

		float c8_2_1[] = { 1.324498e+06, 1.341746e-04, 4.292993e-04, -3.844509e+15, -3.804802e+10, 3.785305e-12, 2.628285e-19, -1.664089e+15, -4.246472e-10, -3.334943e+03, -3.305796e-01, 1.878648e-03, 1.420880e-05, -3.024657e+14, 2.227031e-21, 2.044653e+17, 9.753609e-20, -6.581817e-03, 3.271063e-03, -1.726081e+06, -1.614502e-06, -2.641638e-19, -2.977317e+07, -1.278224e+03, -1.760207e-05, -4.877944e-07, -2.171524e+02, 1.620645e+01, -4.334168e-02, 1.871011e-09, -3.066163e+06, -3.533662e+07 };
		RR_SHARED_PTR<RRArray<cfloat> > c8_2 = AttachRRArrayCopy((cfloat*)c8_2_1, 16);
		r->set_c8(c8_2);

		RR_SHARED_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cfloat > > c9_1 = r->get_c9();
		uint32_t c9_1_1[] = { 2,4 };
		float c9_1_2[] = { 1.397743e+15, 3.933042e+10, -3.812329e+07, 1.508109e+16, -2.091397e-20, 3.207851e+12, -3.640702e+02, 3.903769e+02, -2.879727e+17, -4.589604e-06, 2.202769e-06, 2.892523e+04, -3.306489e-14, 4.522308e-06, 1.665807e+15, 2.340476e+10 };
		ca(c9_1->Dims, AttachRRArray(c9_1_1, 2, false));
		ca(c9_1->Array, AttachRRArray((cfloat*)c9_1_2, 8, false));

		uint32_t c9_2_1[] = { 2,2,2 };
		float c9_2_2[] = { 2.138322e-03, 4.036979e-21, 1.345236e+10, -1.348460e-12, -3.615340e+12, -2.911340e-21, 3.220362e+09, 3.459909e-04, 4.276259e-08, -3.199451e+18, 3.468308e+07, -2.928506e-09, -3.154288e+17, -2.352920e-02, 6.976385e-21, 2.435472e+12 };
		r->set_c9(RR_MAKE_SHARED<RRMultiDimArray<cfloat> >(AttachRRArrayCopy(c9_2_1, 3), AttachRRArrayCopy((cfloat*)c9_2_2, 8)));


	}

	void ServiceTestClient2::TestComplexMemories()
	{
		double c_m1_1[] = { 8.952764e-05, 4.348213e-04, -1.051215e+08, 1.458626e-09, -2.575954e+10, 2.118740e+03, -2.555026e-02, 2.192576e-18, -2.035082e+18, 2.951834e-09, -1.760731e+15, 4.620903e-11, -3.098798e+05, -8.883556e-07, 2.472289e+17, 7.059075e-12 };
		RR_SHARED_PTR<RRArray<cdouble> > c_m1_2 = AttachRRArrayCopy((cdouble*)c_m1_1, 8);
		r->get_c_m1()->Write(10, c_m1_2, 0, 8);

		RR_SHARED_PTR<RRArray<cdouble> > c_m1_3 = AllocateRRArray<cdouble>(8);
		r->get_c_m1()->Read(10, c_m1_3, 0, 8);

		ca(c_m1_2, c_m1_3);

		std::vector<uint64_t> z = boost::assign::list_of(0)(0);
		std::vector<uint64_t> c = boost::assign::list_of(3)(3);

		uint32_t c_m2_1[] = { 3,3 };
		double c_m2_2[] = { -4.850043e-03, 3.545429e-07, 2.169430e+12, 1.175943e-09, 2.622300e+08, -4.439823e-11, -1.520489e+17, 8.250078e-14, 3.835439e-07, -1.424709e-02, 3.703099e+08, -1.971111e-08, -2.805354e+01, -2.093850e-17, -4.476148e+19, 9.914350e+11, 2.753067e+08, -1.745041e+14 };
		RR_SHARED_PTR<RRMultiDimArray<cdouble> > c_m2_3 = RR_MAKE_SHARED<RRMultiDimArray<cdouble> > (AttachRRArrayCopy(c_m2_1,2), AttachRRArrayCopy((cdouble*)c_m2_2, 9));
		r->get_c_m2()->Write(z, c_m2_3, z, c);

		RR_SHARED_PTR<RRMultiDimArray<cdouble> > c_m2_4 = RR_MAKE_SHARED<RRMultiDimArray<cdouble> >(AttachRRArrayCopy(c_m2_1, 2), AllocateRRArray<cdouble>(9));
		r->get_c_m2()->Read(z, c_m2_4, z, c);

		ca(c_m2_3->Dims, c_m2_4->Dims);
		ca(c_m2_3->Array, c_m2_4->Array);
	}
}