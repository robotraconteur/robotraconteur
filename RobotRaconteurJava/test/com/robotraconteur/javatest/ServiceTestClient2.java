package com.robotraconteur.javatest;

import com.robotraconteur.testing.TestService1.*;
import com.robotraconteur.testing.TestService2.*;
import com.robotraconteur.testing.TestService3.*;

import java.util.*;

import com.robotraconteur.*;
import com.robotraconteur.Pipe.PipeEndpoint;

import java.util.concurrent.*;

public class ServiceTestClient2 {
	
	public final void RunFullTest(String url)
	{
		connectService(url);
		
		if (r.get_testenum1_prop() != testenum1.anothervalue)
			throw new RuntimeException();
		
		r.set_testenum1_prop(testenum1.hexval1);
		r.get_o4();
		testWirePeekPoke();
		asyncTestWirePeekPoke();

		testEnums();
		testCStructs();
		
		testGenerators();
		testMemories();
		
		testAStructs();
		
		disconnectService();		
	}

	private testroot3 r;

	public final void connectService(String url)
	{		
		r = (testroot3)RobotRaconteurNode.s().connectService(url);		
	}	

	public final void disconnectService()
	{
		RobotRaconteurNode.s().disconnectService(r);
	}
	
	public void testWirePeekPoke()
	{		
		int[] v = r.get_peekwire().peekInValue();
		if (v[0] != 56295674) throw new RuntimeException();
		
		int[] v_poke1=new int[] {75738265};
		
		r.get_pokewire().pokeOutValue(v_poke1);
		int[] v2 = r.get_pokewire().peekOutValue();
		if (v2[0] != 75738265) throw new RuntimeException();

		Wire<int[]>.WireConnection w = r.get_pokewire().connect();
		for (int i=0; i<3; i++)
		{
			int[] v_setvalue1=new int[] {8638356};
			w.setOutValue(v_setvalue1);
		}

		try
		{
		Thread.sleep(100);
		}
		catch (InterruptedException exp) {}
		
		int[] v3 = r.get_pokewire().peekOutValue();
		if (v3[0] != 8638356) throw new RuntimeException();

	}
	
	Object async_err_lock=new Object();
	RuntimeException async_err=null;
	CountDownLatch async_wait=new CountDownLatch(1);
	
	protected final void testAsync_err(Exception exp)
	{
		synchronized(async_err_lock)
		{
			if (async_err instanceof RuntimeException)
			{
				async_err=(RuntimeException)exp;
			}
			else
			{
				async_err=new RuntimeException("");
			}
			async_wait.countDown();
		}
			
	}
	
	 public void asyncTestWirePeekPoke()
        {
            r.get_peekwire().asyncPeekInValue(new TestAsyncPeekPoke1());

            try {
			
				async_wait.await();
			
			} catch (InterruptedException e) {
				
			}

            synchronized (async_err_lock)
            {
                if (async_err != null)
                    throw async_err;
            }
        }
		
		protected class TestAsyncPeekPoke1 implements Action3<int[], TimeSpec, RuntimeException>
		{
			public void action(int[] value, TimeSpec ts, RuntimeException exp)
			{
				if (exp!=null)
				{
					testAsync_err(exp);
					return;				
				}
				
				try
				{
					if (value[0] != 56295674)
					{
						throw new RuntimeException();
					}
					
					int[] v_out=new int[] {75738261};
					r.get_pokewire().asyncPokeOutValue(v_out, new TestAsyncPeekPoke2());
					
				}
				catch (Exception e)
				{
					testAsync_err(e);
				}
			
			}
			
		}
		
		protected class TestAsyncPeekPoke2 implements Action1<RuntimeException>
		{
			public void action(RuntimeException exp)
			{
				if (exp!=null)
				{
					testAsync_err(exp);
					return;				
				}
				
				try
				{					
					r.get_pokewire().asyncPeekOutValue(new TestAsyncPeekPoke3());
					
				}
				catch (Exception e)
				{
					testAsync_err(e);
				}			
			}			
		}
	
	
		protected class TestAsyncPeekPoke3 implements Action3<int[], TimeSpec, RuntimeException>
		{
			public void action(int[] value, TimeSpec ts, RuntimeException exp)
			{
				if (exp!=null)
				{
					testAsync_err(exp);
					return;				
				}
				
				try
				{
					if (value[0] != 75738261)
					{
						throw new RuntimeException();
					}

                    async_wait.countDown();

                    System.out.println("Async wire peek-poke test complete");					
				}
				catch (Exception e)
				{
					testAsync_err(e);
				}
			
			}
			
		}
		
		public void testEnums()
        {
            if (r.get_testenum1_prop() != testenum1.anothervalue)
                throw new RuntimeException("");

            r.set_testenum1_prop(testenum1.hexval1);

        }

        public void testCStructs()
        {
            testcstruct1 s1 = new testcstruct1();
            ServiceTest2_cstruct.fill_testcstruct1(s1, 563921043);
            ServiceTest2_cstruct.verify_testcstruct1(s1, 563921043);

            MessageElementData s1_m = RobotRaconteurNode.s().packStructure(s1);
            testcstruct1 s1_1 = RobotRaconteurNode.s().<testcstruct1[]>unpackStructure(s1_m)[0];
            ServiceTest2_cstruct.verify_testcstruct1(s1_1, 563921043);

            teststruct3 s2 = ServiceTest2_cstruct.fill_teststruct3(858362);
            ServiceTest2_cstruct.verify_teststruct3(s2, 858362);
            MessageElementData s2_m = RobotRaconteurNode.s().packStructure(s2);
            teststruct3 s2_1 = RobotRaconteurNode.s().<teststruct3>unpackStructure(s2_m);
            ServiceTest2_cstruct.verify_teststruct3(s2_1, 858362);

            testcstruct1 p1 = r.get_testcstruct1_prop();
            ServiceTest2_cstruct.verify_testcstruct1(p1, 563921043);
            testcstruct1 p2 = new testcstruct1();
            ServiceTest2_cstruct.fill_testcstruct1(p2, 85932659);
            r.set_testcstruct1_prop(p2);

            testcstruct1 f1 = r.testcstruct1_func2();
            ServiceTest2_cstruct.verify_testcstruct1(f1, 95836295);
            testcstruct1 f2 = new testcstruct1();
            ServiceTest2_cstruct.fill_testcstruct1(f2, 29546592);
            r.testcstruct1_func1(f2);

            ServiceTest2_cstruct.verify_teststruct3(r.get_teststruct3_prop(), 16483675);
            r.set_teststruct3_prop(ServiceTest2_cstruct.fill_teststruct3(858362));
        }
        
        public void testGenerators()
        {
        	Generator1<UnsignedBytes,UnsignedBytes> g = r.gen_func4();
            for (int i=0; i<3; i++)
            {
                g.next(new UnsignedBytes(new byte[] { }));
            }
            UnsignedBytes b2 = g.next(new UnsignedBytes(new byte[] { 2, 3, 4 }));
            g.abort();
            try
            {
            g.next(new UnsignedBytes(new byte[] { 2, 3, 4 }));
            }
            catch (OperationAbortedException e) {}
            
            Generator2<double[]> g2 = r.gen_func1();
            List<double[]> res=g2.nextAll();
            System.out.println(res);
            
            Generator2<double[]> g3 = r.gen_func1();
            g3.next();
            g3.abort();
            try
            {
            	g3.next();
            }
            catch(OperationAbortedException e)
            {
            	System.out.println("Caught operation aborted");
            }
            
            Generator2<double[]> g4 = r.gen_func1();
            g4.next();
            g4.close();
            try
            {
            	g4.next();
            }
            catch(StopIterationException e)
            {
            	System.out.println("Caught operation aborted");
            }            
        }
        
        public void testMemories()
        {
        	test_m1();
        	test_m2();
        }
        
        public void test_m1()
        {
        	testcstruct2[] o1 = new testcstruct2[32];

            for (int i = 0; i < o1.length; i++)
            {
            	o1[i] = new testcstruct2(); 
                ServiceTest2_cstruct.fill_testcstruct2(o1[i], 59174 + i);
            }

            if (r.get_cstruct_m1().length() != 1024) throw new RuntimeException("");

            r.get_cstruct_m1().write(52, o1, 3, 17);

            testcstruct2[] o2 = new testcstruct2[32];

            r.get_cstruct_m1().read(53, o2, 2, 16);

            for (int i = 2; i < 16; i++)
            {
                ServiceTest2_cstruct.verify_testcstruct2(o2[i], 59174 + i + 2);
            }
        }
        
        public void test_m2()
        {
        	CStructureMultiDimArray s = new CStructureMultiDimArray(new int[] { 3, 3 }, new testcstruct2[9]);

            for (int i = 0; i < 9; i++)
            {
            	((testcstruct2[])s.cstruct_array)[i] = new testcstruct2();
                ServiceTest2_cstruct.fill_testcstruct2(((testcstruct2[])s.cstruct_array)[i], 75721 + i);
            }

            r.get_cstruct_m2().write(new long[] { 0, 0 }, s, new long[] { 0, 0 }, new long[] { 3, 3 });

            CStructureMultiDimArray s2 = new CStructureMultiDimArray(new int[] { 3, 3 }, new testcstruct2[9]);

            r.get_cstruct_m2().read(new long[] { 0, 0 }, s2, new long[] { 0, 0 }, new long[] { 3, 3 });

            for (int i = 0; i < 9; i++)
            {
                ServiceTest2_cstruct.verify_testcstruct2(((testcstruct2[])s2.cstruct_array)[i], 75721 + i);
            }        
        }
        
        public void testAStructs()
        {
        	transform a1 = new transform();
            ServiceTest2_cstruct.fill_transform(a1, 3956378);
            r.set_testastruct1(a1.translation);

            transform a1_1 = new transform();
            a1_1.rotation = a1.rotation;
            a1_1.translation = r.get_testastruct1();
            transform a1_2 = new transform();
            ServiceTest2_cstruct.fill_transform(a1_2, 74637);
            a1_1.rotation = a1_2.rotation;
            ServiceTest2_cstruct.verify_transform(a1_1, 74637);

            transform a2 = new transform();
            ServiceTest2_cstruct.fill_transform(a2, 827635);
            r.set_testastruct2(a2);

            transform a2_1 = r.get_testastruct2();
            ServiceTest2_cstruct.verify_transform(a2_1, 1294);

            r.set_testastruct3(ServiceTest2_cstruct.fill_transform_array(6, 19274));
            ServiceTest2_cstruct.verify_transform_array(r.get_testastruct3(), 8, 837512);

            r.set_testastruct4(ServiceTest2_cstruct.fill_transform_multidimarray(5, 2, 6385));
            ServiceTest2_cstruct.verify_transform_multidimarray(r.get_testastruct4(), 7, 2, 66134);

            r.set_testastruct5(ServiceTest2_cstruct.fill_transform_multidimarray(3, 2, 7732));
            ServiceTest2_cstruct.verify_transform_multidimarray(r.get_testastruct5(), 3, 2, 773142);

        }
        	
	
}