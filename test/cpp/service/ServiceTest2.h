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
		void RegisterServices(RR_SHARED_PTR<TcpTransport> transport, RR_SHARED_PTR<RobotRaconteurNode> node=RR_SHARED_PTR<RobotRaconteurNode>());
		void UnregisterServices();
	protected:

		RR_SHARED_PTR<testroot3_impl> testservice2;
	};

	class testroot3_impl : public virtual com::robotraconteur::testing::TestService3::testroot3_default_impl, 
		public RR_ENABLE_SHARED_FROM_THIS<testroot3_impl>
	{
	public:

		testroot3_impl();
		
		RR_OVIRTUAL void set_peekwire(const RR_SHARED_PTR<Wire<int32_t > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL int32_t get_unknown_modifier() RR_OVERRIDE ;
		RR_OVIRTUAL void set_unknown_modifier(int32_t value) RR_OVERRIDE ;

		RR_OVIRTUAL int32_t get_readme() RR_OVERRIDE ;

		RR_OVIRTUAL void set_writeme(int32_t value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > > get_unreliable1() RR_OVERRIDE ;
		RR_OVIRTUAL void set_unreliable1(const RR_SHARED_PTR<RobotRaconteur::Pipe<int32_t > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ArrayMemory<double > > get_readmem() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<com::robotraconteur::testing::TestService3::obj4 > get_o4() RR_OVERRIDE ;

		RR_OVIRTUAL com::robotraconteur::testing::TestService3::testenum1::testenum1 get_testenum1_prop() RR_OVERRIDE ;
		RR_OVIRTUAL void set_testenum1_prop(com::robotraconteur::testing::TestService3::testenum1::testenum1 value) RR_OVERRIDE ;

		RR_OVIRTUAL com::robotraconteur::testing::TestService3::testpod1 get_testpod1_prop() RR_OVERRIDE ;
		RR_OVIRTUAL void set_testpod1_prop(const com::robotraconteur::testing::TestService3::testpod1& value) RR_OVERRIDE ;

		RR_OVIRTUAL void testpod1_func1(const com::robotraconteur::testing::TestService3::testpod1& s) RR_OVERRIDE ;

		RR_OVIRTUAL com::robotraconteur::testing::TestService3::testpod1 testpod1_func2() RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3 > get_teststruct3_prop() RR_OVERRIDE ;
		RR_OVIRTUAL void set_teststruct3_prop(const RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService3::teststruct3 >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::PodArrayMemory<com::robotraconteur::testing::TestService3::testpod2 > > get_pod_m1() RR_OVERRIDE ;
		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::PodMultiDimArrayMemory<com::robotraconteur::testing::TestService3::testpod2 > > get_pod_m2() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > >, RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > > > > gen_func4() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Generator<double, void > > gen_func1() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > >, void > > gen_func2(const std::string& name) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Generator<void, RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<uint8_t > > > > gen_func3(const std::string& name) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Generator<RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2 >, RR_INTRUSIVE_PTR<com::robotraconteur::testing::TestService1::teststruct2 > > > gen_func5() RR_OVERRIDE ;
				
		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_p1() RR_OVERRIDE ;
		RR_OVIRTUAL void set_p1(const RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_p2() RR_OVERRIDE ;
		RR_OVIRTUAL void set_p2(const RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > get_p3() RR_OVERRIDE ;
		RR_OVIRTUAL void set_p3(const RR_SHARED_PTR<RobotRaconteur::Pipe<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_w1() RR_OVERRIDE ;
		RR_OVIRTUAL void set_w1(const RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > > get_w2() RR_OVERRIDE ;
		RR_OVIRTUAL void set_w2(const RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<int32_t > > > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > > get_w3() RR_OVERRIDE ;
		RR_OVIRTUAL void set_w3(const RR_SHARED_PTR<RobotRaconteur::Wire<RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<int32_t > > > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL com::robotraconteur::testing::TestService3::vector3 get_testnamedarray1() RR_OVERRIDE ;
		RR_OVIRTUAL void set_testnamedarray1(const com::robotraconteur::testing::TestService3::vector3& value) RR_OVERRIDE ;

		RR_OVIRTUAL com::robotraconteur::testing::TestService3::transform get_testnamedarray2() RR_OVERRIDE ;
		RR_OVIRTUAL void set_testnamedarray2(const com::robotraconteur::testing::TestService3::transform& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedArray<com::robotraconteur::testing::TestService3::transform> > get_testnamedarray3() RR_OVERRIDE ;
		RR_OVIRTUAL void set_testnamedarray3(const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedArray<com::robotraconteur::testing::TestService3::transform> >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > get_testnamedarray4() RR_OVERRIDE ;
		RR_OVIRTUAL void set_testnamedarray4(const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> > get_testnamedarray5() RR_OVERRIDE ;
		RR_OVIRTUAL void set_testnamedarray5(const RR_INTRUSIVE_PTR<RobotRaconteur::RRNamedMultiDimArray<com::robotraconteur::testing::TestService3::transform> >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::NamedArrayMemory<com::robotraconteur::testing::TestService3::transform > > get_namedarray_m1() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::NamedMultiDimArrayMemory<com::robotraconteur::testing::TestService3::transform > > get_namedarray_m2() RR_OVERRIDE ;

		RR_OVIRTUAL RobotRaconteur::cdouble get_c1() RR_OVERRIDE ;
		RR_OVIRTUAL void set_c1(RobotRaconteur::cdouble value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble > > get_c2() RR_OVERRIDE ;
		RR_OVIRTUAL void set_c2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cdouble > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cdouble > > get_c3() RR_OVERRIDE ;
		RR_OVIRTUAL void set_c3(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cdouble > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cdouble >  > > get_c5() RR_OVERRIDE ;
		RR_OVIRTUAL void set_c5(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::cdouble >  > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RobotRaconteur::cfloat get_c7() RR_OVERRIDE ;
		RR_OVIRTUAL void set_c7(RobotRaconteur::cfloat value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat > > get_c8() RR_OVERRIDE ;
		RR_OVIRTUAL void set_c8(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::cfloat > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cfloat > > get_c9() RR_OVERRIDE ;
		RR_OVIRTUAL void set_c9(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::cfloat > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ArrayMemory<RobotRaconteur::cdouble > > get_c_m1() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<RobotRaconteur::cdouble > > get_c_m2() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<com::robotraconteur::testing::TestService3::obj5 > get_nolock_test() RR_OVERRIDE ;

		RR_OVIRTUAL RobotRaconteur::rr_bool get_b1() RR_OVERRIDE ;
		RR_OVIRTUAL void set_b1(RobotRaconteur::rr_bool value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::rr_bool > > get_b2() RR_OVERRIDE ;
		RR_OVIRTUAL void set_b2(const RR_INTRUSIVE_PTR<RobotRaconteur::RRArray<RobotRaconteur::rr_bool > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::rr_bool > > get_b3() RR_OVERRIDE ;
		RR_OVIRTUAL void set_b3(const RR_INTRUSIVE_PTR<RobotRaconteur::RRMultiDimArray<RobotRaconteur::rr_bool > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::rr_bool >  > > get_b4() RR_OVERRIDE ;
		RR_OVIRTUAL void set_b4(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::rr_bool >  > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::rr_bool >  > > get_b5() RR_OVERRIDE ;
		RR_OVIRTUAL void set_b5(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRArray<RobotRaconteur::rr_bool >  > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<RobotRaconteur::rr_bool >  > > get_b6() RR_OVERRIDE ;
		RR_OVIRTUAL void set_b6(const RR_INTRUSIVE_PTR<RobotRaconteur::RRList<RobotRaconteur::RRMultiDimArray<RobotRaconteur::rr_bool >  > >& value) RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ArrayMemory<RobotRaconteur::rr_bool > > get_c_m5() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<RobotRaconteur::rr_bool > > get_c_m6() RR_OVERRIDE ;

		RR_OVIRTUAL void test_exception_params1() RR_OVERRIDE ;

		RR_OVIRTUAL void test_exception_params2() RR_OVERRIDE ;

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

		RR_SHARED_PTR<RobotRaconteur::ArrayMemory<RobotRaconteur::rr_bool > > c_m5;
		RR_SHARED_PTR<RobotRaconteur::MultiDimArrayMemory<RobotRaconteur::rr_bool > > c_m6;

		RR_SHARED_PTR<com::robotraconteur::testing::TestService3::obj5> obj5_;

	};

	class obj4_impl : public virtual com::robotraconteur::testing::TestService3::obj4
	{
	public:
		RR_OVIRTUAL std::string get_s_ind() RR_OVERRIDE  { return ""; }
		RR_OVIRTUAL void set_s_ind(const std::string& value)  RR_OVERRIDE {}

		RR_OVIRTUAL int32_t get_i_ind()  RR_OVERRIDE { return 0; }
		RR_OVIRTUAL void set_i_ind(int32_t value) RR_OVERRIDE  {}

		RR_OVIRTUAL std::string get_data()  RR_OVERRIDE { return ""; }
		RR_OVIRTUAL void set_data(const std::string& value)  RR_OVERRIDE {}

		RR_OVIRTUAL RR_SHARED_PTR<com::robotraconteur::testing::TestService1::sub3 > get_o3_1(const std::string& ind)  RR_OVERRIDE { throw std::runtime_error(""); }
				
	};

	class obj5_impl : public virtual com::robotraconteur::testing::TestService3::obj5_default_impl
	{
	public:

		obj5_impl();

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ArrayMemory<int32_t > > get_m1() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ArrayMemory<int32_t > > get_m2() RR_OVERRIDE ;

		RR_OVIRTUAL RR_SHARED_PTR<RobotRaconteur::ArrayMemory<int32_t > > get_m3() RR_OVERRIDE ;

		RR_OVIRTUAL int32_t f1() RR_OVERRIDE ;

		RR_OVIRTUAL int32_t f2() RR_OVERRIDE ;
	
	protected:

		RR_SHARED_PTR<RobotRaconteur::ArrayMemory<int32_t > > _m1;

		RR_SHARED_PTR<RobotRaconteur::ArrayMemory<int32_t > > _m2;

		RR_SHARED_PTR<RobotRaconteur::ArrayMemory<int32_t > > _m3;

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