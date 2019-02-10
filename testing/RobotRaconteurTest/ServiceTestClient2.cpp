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
}