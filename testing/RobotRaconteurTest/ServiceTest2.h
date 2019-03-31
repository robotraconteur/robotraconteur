#include "com__robotraconteur__testing__TestService3_stubskel.h"

#pragma once

#include "CompareArray.h"

using namespace RobotRaconteur;

namespace RobotRaconteurTest
{
	class testroot3_impl;

	class RobotRaconteurTestService2Support
	{
	public:
		void RegisterServices(RR_SHARED_PTR<TcpTransport> transport);
		void UnregisterServices();
	protected:

		RR_SHARED_PTR<testroot3_impl> testservice2;
	};

	class testroot3_impl : public virtual com::robotraconteur::testing::TestService3::testroot3_default_impl, 
		public RR_ENABLE_SHARED_FROM_THIS<testroot3_impl>
	{
	public:

		testroot3_impl();
		
		virtual void set_peekwire(RR_SHARED_PTR<Wire<int32_t > > value);

		virtual int32_t get_unknown_modifier();
		virtual void set_unknown_modifier(int32_t value);

		virtual int32_t get_readme();

		virtual void set_writeme(int32_t value);

		virtual RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > get_unreliable1();
		virtual void set_unreliable1(RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > value);

		virtual RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > get_unreliable2();
		virtual void set_unreliable2(RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > value);

		virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<double > > get_readmem();

		virtual RR_SHARED_PTR<com::robotraconteur::testing::TestService3::obj4 > get_o4();

		virtual com::robotraconteur::testing::TestService3::testenum1::testenum1 get_testenum1_prop();
		virtual void set_testenum1_prop(com::robotraconteur::testing::TestService3::testenum1::testenum1 value);

		virtual com::robotraconteur::testing::TestService3::testpod1 get_testpod1_prop();
		virtual void set_testpod1_prop(const com::robotraconteur::testing::TestService3::testpod1& value);

		virtual void testpod1_func1(const com::robotraconteur::testing::TestService3::testpod1& s);

		virtual com::robotraconteur::testing::TestService3::testpod1 testpod1_func2();

		virtual RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3 > get_teststruct3_prop();
		virtual void set_teststruct3_prop(RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3 > value);

		virtual RR_SHARED_PTR<RobotRaconteur::PodArrayMemory<com::robotraconteur::testing::TestService3::testpod2 > > get_pod_m1();
		virtual RR_SHARED_PTR<RobotRaconteur::PodMultiDimArrayMemory<com::robotraconteur::testing::TestService3::testpod2 > > get_pod_m2();

		virtual RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > >, RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > > > > gen_func4();

		virtual RR_SHARED_PTR<RobotRaconteur::Generator<double, void > > gen_func1();

		virtual RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > >, void > > gen_func2(const std::string& name);

		virtual RR_SHARED_PTR<RobotRaconteur::Generator<void, RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > > > > gen_func3(const std::string& name);

		virtual RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2 >, RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2 > > > gen_func5();
				
		virtual RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_p1();
		virtual void set_p1(RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > value);

		virtual RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_p2();
		virtual void set_p2(RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > value);

		virtual RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > get_p3();
		virtual void set_p3(RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > value);

		virtual RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_w1();
		virtual void set_w1(RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > value);

		virtual RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_w2();
		virtual void set_w2(RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > value);

		virtual RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > get_w3();
		virtual void set_w3(RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > value);

		virtual com::robotraconteur::testing::TestService3::vector3 get_testnamedarray1();
		virtual void set_testnamedarray1(const com::robotraconteur::testing::TestService3::vector3& value);

		virtual com::robotraconteur::testing::TestService3::transform get_testnamedarray2();
		virtual void set_testnamedarray2(const com::robotraconteur::testing::TestService3::transform& value);

		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedArray<com::robotraconteur::testing::TestService3::transform> > get_testnamedarray3();
		virtual void set_testnamedarray3(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedArray<com::robotraconteur::testing::TestService3::transform> > value);

		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > get_testnamedarray4();
		virtual void set_testnamedarray4(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > value);

		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > get_testnamedarray5();
		virtual void set_testnamedarray5(RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > value);

		virtual RR_SHARED_PTR<RobotRaconteur::NamedArrayMemory<com::robotraconteur::testing::TestService3::transform > > get_namedarray_m1();

		virtual RR_SHARED_PTR<RobotRaconteur::NamedMultiDimArrayMemory<com::robotraconteur::testing::TestService3::transform > > get_namedarray_m2();

		virtual RobotRaconteur::cdouble get_c1();
		virtual void set_c1(RobotRaconteur::cdouble value);

		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble > > get_c2();
		virtual void set_c2(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble > > value);

		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cdouble > > get_c3();
		virtual void set_c3(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cdouble > > value);

		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cdouble >  > > get_c5();
		virtual void set_c5(RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cdouble >  > > value);

		virtual RobotRaconteur::cfloat get_c7();
		virtual void set_c7(RobotRaconteur::cfloat value);

		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat > > get_c8();
		virtual void set_c8(RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat > > value);

		virtual RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cfloat > > get_c9();
		virtual void set_c9(RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cfloat > > value);

		virtual RR_SHARED_PTR<RobotRaconteur::ArrayMemory<RobotRaconteur::cdouble > > get_c_m1();

		virtual RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<RobotRaconteur::cdouble > > get_c_m2();

	protected:

		void peekwire_timer_handler(const TimerEvent& e);

		RR_SHARED_PTR<Timer> peekwire_timer;

		RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > unreliable1;
		RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > unreliable2;

		RR_SHARED_PTR<RobotRaconteur::PodArrayMemory<com::robotraconteur::testing::TestService3::testpod2 > > pod_m1;
		RR_SHARED_PTR<RobotRaconteur::PodMultiDimArrayMemory<com::robotraconteur::testing::TestService3::testpod2 > > pod_m2;
		
		RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > p1;
		RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > p2;
		RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > p3;

		RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > w1;
		RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > w2;
		RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > w3;

		com::robotraconteur::testing::TestService3::vector3 testnamedarray1;
		com::robotraconteur::testing::TestService3::transform testnamedarray2;
		RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedArray<com::robotraconteur::testing::TestService3::transform> > testnamedarray3;
		RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > testnamedarray4;
		RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > testnamedarray5;

		RR_SHARED_PTR<RobotRaconteur::NamedArrayMemory<com::robotraconteur::testing::TestService3::transform > > namedarray_m1;
		RR_SHARED_PTR<RobotRaconteur::NamedMultiDimArrayMemory<com::robotraconteur::testing::TestService3::transform > > namedarray_m2;

		RR_SHARED_PTR<RobotRaconteur::ArrayMemory<RobotRaconteur::cdouble > > c_m1;
		RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<RobotRaconteur::cdouble > > c_m2;

	};

	class obj4_impl : public virtual com::robotraconteur::testing::TestService3::obj4
	{
	public:
		virtual std::string get_s_ind() { return ""; }
		virtual void set_s_ind(const std::string& value) {}

		virtual int32_t get_i_ind() { return 0; }
		virtual void set_i_ind(int32_t value) {}

		virtual std::string get_data() { return ""; }
		virtual void set_data(const std::string& value) {}

		virtual RR_SHARED_PTR<com::robotraconteur::testing::TestService1::sub3 > get_o3_1(const std::string& ind) { throw std::runtime_error(""); }
				
	};


	void ServiceTest2_fill_testpod1(com::robotraconteur::testing::TestService3::testpod1& v, uint32_t seed);
	void ServiceTest2_verify_testpod1(const com::robotraconteur::testing::TestService3::testpod1& v, uint32_t seed);

	void ServiceTest2_fill_testpod2(com::robotraconteur::testing::TestService3::testpod2& v, uint32_t seed);
	void ServiceTest2_verify_testpod2(const com::robotraconteur::testing::TestService3::testpod2& v, uint32_t seed);

	RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3> ServiceTest2_fill_teststruct3(uint32_t seed);
	void ServiceTest2_verify_teststruct3(RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3> v, uint32_t seed);

	RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> > ServiceTest2_fill_testpod1_array(size_t s, uint32_t seed);
	void ServiceTest2_verify_testpod1_array(RR_INTRUSIVE_PTR<RRPodArray<com::robotraconteur::testing::TestService3::testpod1> > v, size_t s, uint32_t seed);

	RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod1> > ServiceTest2_fill_testpod1_multidimarray(size_t m, size_t n, uint32_t seed);
	void ServiceTest2_verify_testpod1_multidimarray(RR_INTRUSIVE_PTR<RRPodMultiDimArray<com::robotraconteur::testing::TestService3::testpod1> > v, size_t m, size_t n, uint32_t seed);
	void ServiceTest2_verify_testpod1_multidimarray(RR_INTRUSIVE_PTR<RRValue> v, size_t m, size_t n, uint32_t seed);

	void ServiceTest2_fill_transform(com::robotraconteur::testing::TestService3::transform& v, uint32_t seed);
	void ServiceTest2_verify_transform(const com::robotraconteur::testing::TestService3::transform& v, uint32_t seed);
	RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > ServiceTest2_fill_transform_array(size_t s, uint32_t seed);
	void ServiceTest2_verify_transform_array(RR_INTRUSIVE_PTR<RRNamedArray<com::robotraconteur::testing::TestService3::transform> > v, size_t s, uint32_t seed);
	RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > ServiceTest2_fill_transform_multidimarray(size_t m, size_t n, uint32_t seed);
	void ServiceTest2_verify_transform_multidimarray(RR_INTRUSIVE_PTR<RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > v, size_t m, size_t n, uint32_t seed);
	void ServiceTest2_verify_transform_multidimarray(RR_INTRUSIVE_PTR<RRValue> v, size_t m, size_t n, uint32_t seed);
}