package com.robotraconteur.javatest;

import com.robotraconteur.*;
import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;
import com.robotraconteur.testing.TestService3.*;

import java.util.*;

public class testroot3_impl implements testroot3
{
	
	Wire<int[]> _peekwire;
	Wire<int[]> _pokewire;
	WireBroadcaster<int[]> _peekwire_b;
	WireUnicastReceiver<int[]> _pokewire_r;
	
	com.robotraconteur.Timer _peekwire_t;
	
	public Wire<int[]> get_peekwire()
	{
		return _peekwire;
	}
    public void set_peekwire(Wire<int[]> value)
	{
		_peekwire=value;
		_peekwire_b=new WireBroadcaster<int[]>(_peekwire);
		int[] v=new int[] {56295674};
		_peekwire_b.setOutValue(v);
		_peekwire_t=RobotRaconteurNode.s().createTimer(100, new peektimer_handler());
		
	}
    public Wire<int[]> get_pokewire()
	{
		return _pokewire;
	}
    public void set_pokewire(Wire<int[]> value)
	{
		_pokewire=value;
		_pokewire_r=new WireUnicastReceiver<int[]>(_pokewire);
		_pokewire_r.addInValueListener(new pokewire_changed());
	}
	
	class peektimer_handler implements Action1<TimerEvent>
	{
		@Override
		public void action(TimerEvent evt)
		{
			int[] v=new int[] {56295674};
			_peekwire_b.setOutValue(v);
		}
		
	}

	class pokewire_changed implements Action3<int[], TimeSpec, Long>
	{
		public void action(int[] value, TimeSpec ts, Long ep)
		{
			System.out.println("RobotRaconteurTestService2.pokewire changed: " + Integer.toString(value[0]));
		}

	}
	
	public int get_unknown_modifier()
	{
		return 0;
	}
	
	public void set_unknown_modifier(int v)
	{
		
	}
	
	public ArrayMemory<double[]> get_readmem()
	{
		return null;
	}
	
	Pipe<int[]> unreliable1;
	
	public Pipe<int[]> get_unreliable1()
	{
		return unreliable1;
	}
	
	public void set_unreliable1(Pipe<int[]> p)
	{
		unreliable1=p;
	}
	
	public Pipe<int[]> get_unreliable2()
	{
		return unreliable1;
	}
	
	public void set_unreliable2(Pipe<int[]> p)
	{
		unreliable1=p;
	}
	
	public int get_readme()
	{
		return 0;
	}
	
	public void set_readme(int v)
	{
		
	}
	
	public int get_writeme()
	{
		return 0;
	}
	
	public void set_writeme(int v)
	{
		
	}
	
	public testenum1 get_testenum1_prop()
	{
		return testenum1.anothervalue;
	}
    public void set_testenum1_prop(testenum1 value)
	{
		if (value != testenum1.hexval1)
			throw new RuntimeException("");
	}
    public obj4 get_o4()
	{
		return new obj4_impl();
	}
	@Override
	public testcstruct1 get_testcstruct1_prop() {
		testcstruct1 o = new testcstruct1();
        ServiceTest2_cstruct.fill_testcstruct1(o, 563921043);
        return o;
	}
	@Override
	public void set_testcstruct1_prop(testcstruct1 value) {
		ServiceTest2_cstruct.verify_testcstruct1(value, 85932659);
		
	}
	@Override
	public teststruct3 get_teststruct3_prop() {
		return ServiceTest2_cstruct.fill_teststruct3(16483675);
	}
	@Override
	public void set_teststruct3_prop(teststruct3 value) {
		ServiceTest2_cstruct.verify_teststruct3(value, 858362);		
	}
	@Override
	public void testcstruct1_func1(testcstruct1 s) {
		ServiceTest2_cstruct.verify_testcstruct1(s, 29546592);
		
	}
	@Override
	public testcstruct1 testcstruct1_func2() {
		testcstruct1 o = new testcstruct1();
        ServiceTest2_cstruct.fill_testcstruct1(o, 95836295);
        return o;		
	}
	@Override
	public Generator2<double[]> gen_func1() {
		ArrayList<double[]> v=new ArrayList<double[]>();
		for (double i=0; i<16; i++)
		{
			v.add(new double[] {i});
		}
		return new IteratorGenerator<double[]>(v.iterator());            
	}
	@Override
	public Generator2<UnsignedBytes> gen_func2(String name) {
		ArrayList<UnsignedBytes> v=new ArrayList<UnsignedBytes>();
		for (byte i=0; i<16; i++)
		{
			v.add(new UnsignedBytes(new byte[] {i}));
		}
		
		return new IteratorGenerator<UnsignedBytes>(v.iterator());		
	}
	@Override
	public Generator3<UnsignedBytes> gen_func3(String name) {
		// TODO Auto-generated method stub
		return null;
	}
	
	class func4_gen extends SyncGenerator1<UnsignedBytes, UnsignedBytes>
	{
		
		boolean aborted=false;
		byte j=0;

		@Override
		public void close() {
			j=100;			
		}

		@Override
		public void abort() {
			aborted=true;			
		}

		@Override
		public UnsignedBytes next(UnsignedBytes param) {
			if (aborted)
			{
				throw new OperationAbortedException("");
			}
			
			if (j>=8)
			{
				throw new StopIterationException("");
			}
			byte[] a=new byte[param.value.length];
			for (int i=0; i<param.value.length; i++)
			{
				a[i] = (byte)(param.value[i] + j);
			}
			j++;
			return new UnsignedBytes(a);
		}		
	}
	
	
	@Override
	public Generator1<UnsignedBytes, UnsignedBytes> gen_func4() {
		
		return new func4_gen();
	}
	@Override
	public Generator1<teststruct2, teststruct2> gen_func5() {
		// TODO Auto-generated method stub
		return null;
	}
	
	CStructureArrayMemory<testcstruct2[]> m_cstruct_m1= new CStructureArrayMemory<testcstruct2[]>(new testcstruct2[1024]);
	
	@Override
	public CStructureArrayMemory<testcstruct2[]> get_cstruct_m1() {
		return m_cstruct_m1;		
	}
	
	CStructureMultiDimArrayMemory<testcstruct2[]> m_cstruct_m2 = new CStructureMultiDimArrayMemory<testcstruct2[]>(new CStructureMultiDimArray(new int[] { 3, 3 }, new testcstruct2[9]));
	@Override
	public CStructureMultiDimArrayMemory<testcstruct2[]> get_cstruct_m2() {
		return m_cstruct_m2;
	}
	@Override
	public List<double[]> get_d1() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public void set_d1(List<double[]> value) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public List<double[]> get_d2() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public void set_d2(List<double[]> value) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public Map<Integer, double[]> get_d3() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public void set_d3(Map<Integer, double[]> value) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public Map<Integer, double[]> get_d4() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public void set_d4(Map<Integer, double[]> value) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public List<MultiDimArray> get_d5() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public void set_d5(List<MultiDimArray> value) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public Map<Integer, MultiDimArray> get_d6() {
		// TODO Auto-generated method stub
		return null;
	}
	@Override
	public void set_d6(Map<Integer, MultiDimArray> value) {
		// TODO Auto-generated method stub
		
	}
	
	Pipe<int[]> p1;
	@Override
	public Pipe<int[]> get_p1() {		
		return p1;
	}
	@Override
	public void set_p1(Pipe<int[]> value) {
		p1=value;
		
	}
	Pipe<int[]> p2;
	@Override
	public Pipe<int[]> get_p2() {
		return p2;
	}
	@Override
	public void set_p2(Pipe<int[]> value) {
		p2=value;		
	}
	Pipe<MultiDimArray> p3;
	@Override
	public Pipe<MultiDimArray> get_p3() {
		return p3;
	}
	@Override
	public void set_p3(Pipe<MultiDimArray> value) {
		p3=value;		
	}
	Wire<int[]> w1;
	@Override
	public Wire<int[]> get_w1() {
		return w1;
	}
	@Override
	public void set_w1(Wire<int[]> value) {
		w1=value;		
	}
	Wire<int[]> w2;
	@Override
	public Wire<int[]> get_w2() {
		return w2;
	}
	@Override
	public void set_w2(Wire<int[]> value) {
		w2=value;
		
	}
	
	Wire<MultiDimArray> w3;
	@Override
	public Wire<MultiDimArray> get_w3() {
		return w3;
	}
	@Override
	public void set_w3(Wire<MultiDimArray> value) {
		w3=value;		
	}
	@Override
	public vector3 get_testastruct1() {
		transform a1 = new transform();
        ServiceTest2_cstruct.fill_transform(a1, 74637);
        return a1.translation;
	}
	@Override
	public void set_testastruct1(vector3 value) {
		transform a1 = new transform();
        ServiceTest2_cstruct.fill_transform(a1, 3956378);
        a1.translation = value;
        ServiceTest2_cstruct.verify_transform(a1, 3956378);
		
	}
	@Override
	public transform get_testastruct2() {
		transform a2 = new transform();
        ServiceTest2_cstruct.fill_transform(a2, 1294);
        return a2;		
	}
	@Override
	public void set_testastruct2(transform value) {
		ServiceTest2_cstruct.verify_transform(value, 827635);
		
	}
	@Override
	public transform[] get_testastruct3() {
		return ServiceTest2_cstruct.fill_transform_array(8, 837512);		
	}
	@Override
	public void set_testastruct3(transform[] value) {
		ServiceTest2_cstruct.verify_transform_array(value, 6, 19274);
		
	}
	@Override
	public AStructureMultiDimArray get_testastruct4() {
		return ServiceTest2_cstruct.fill_transform_multidimarray(7, 2, 66134);
	}
	@Override
	public void set_testastruct4(AStructureMultiDimArray value) {
		ServiceTest2_cstruct.verify_transform_multidimarray(value, 5, 2, 6385);		
	}
	@Override
	public AStructureMultiDimArray get_testastruct5() {
		return ServiceTest2_cstruct.fill_transform_multidimarray(3, 2, 773142);
	}
	@Override
	public void set_testastruct5(AStructureMultiDimArray value) {
		ServiceTest2_cstruct.verify_transform_multidimarray(value, 3, 2, 7732);		
	}
	
	
}