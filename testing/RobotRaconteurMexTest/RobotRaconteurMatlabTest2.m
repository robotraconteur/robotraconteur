function RobotRaconteurMatlabTest2(url)

    v = create_testpod1_array(10, 10000);
    verify_testpod1_array(v, 10, 10000)

    v3 = fill_teststruct3(185392);
    verify_teststruct3(v3,185392);
     
    c=RobotRaconteur.ConnectService(url);
    assignin('base','c',c)
    
    c.testpod1_prop
    verify_testpod1(c.testpod1_prop,563921043)    
    c.testpod1_prop = fill_testpod1(85932659);

    verify_teststruct3(c.teststruct3_prop, 16483675);
    c.teststruct3_prop = (fill_teststruct3(858362));
    
    g=c.gen_func1();
    g.Next();
    g.Next();
    g.async_Next(@gen_func1_handler, 15);
    pause(1)
    RobotRaconteur.ProcessRequests()
    g.async_Close(@gen_func1_close,15);
    pause(1)
    RobotRaconteur.ProcessRequests()
    
    pod_m1_v1=fill_testpod2(59174);
    for j=2:32
        pod_m1_v1(j,1) = fill_testpod2(59174+j-1);
    end

    pod_m1 = c.pod_m1;
    assert(all(size(pod_m1) == [1024 1]))    
    pod_m1(53:69) = pod_m1_v1(4:21);
    pod_m1_v2 = pod_m1(54:69);
    
    for j=1:15        
        assert(isequal(pod_m1_v1(j+4),pod_m1_v2(j)))
    end
    
    pod_m2_v1=fill_testpod2(75721);
    for j=2:9
        pod_m2_v1(j,1) = fill_testpod2(75721+j-1);
    end
    pod_m2_v1 = reshape(pod_m2_v1, [3,3]);
        
    pod_m2 = c.pod_m2;
    pod_m2(1:3,1:3) = pod_m2_v1;
    pod_m2_v2 = pod_m2(1:3,1:3);
    assert(isequal(pod_m2_v1,pod_m2_v2))  
    
    transform_m1_v1=fill_transform(79174);
    for j=2:32
        transform_m1_v1(:,j) = fill_transform(79174+j-1);
    end

    transform_m1 = c.namedarray_m1;
    assert(all(size(transform_m1) == [512 1]))    
    transform_m1(:,24:44) = transform_m1_v1(:,4:24);
    transform_m1_v2 = transform_m1(:,25:42);
    
    for j=1:18
        assert(isequal(transform_m1_v1(:,j+4),transform_m1_v2(:,j)))
    end
    
    c.b1 = true;
    assert(c.b1 == true)
    c.b2 = [true, false, false, true, true, true, false, true]';
    assert(all(c.b2 == [true, false, true, true, false, true, false]'))
    c.b3 = [true,false]';
    assert(all(all(c.b3 == reshape([false,true,true,false],2,2))))
    
    c.b4 = {true};
    b4_2 = c.b4;
    assert(all(b4_2{1} == true));
    c.b5 = {[true, false]'};
    b5_2 = c.b5;
    assert(all(all(b5_2{1} == [false,true,false,false]')))
    c.b6 = {[true,false]'};
    b6_2 = c.b6;
    assert(all(all(b6_2{1} == reshape([false,true,true,false],2,2))))
    
    c_m5_v1_1 = [true, false, false, true, true, false, false, false, true, true]';
    c_m5 = c.c_m5;
    c_m5(100:107) = c_m5_v1_1(2:9);
    assert(all(c_m5(100:107) == c_m5_v1_1(2:9)))
    
    c_m6_v1_1 = reshape(c_m5_v1_1,2,5);
    c_m6 = c.c_m6;
    c_m6(1:2,1:5) = c_m6_v1_1;
    assert(all(all(c_m6(1:2,1:5) == c_m6_v1_1)))
    
    disp('Done!')

    function gen_func1_handler(key, res, err)
       disp('Generator next callback')
       res
       err
       key
    end

    function gen_func1_close(key, err)
       disp('Generator closed calback')
       key
       err       
    end
    
    function d = create_double_array(gen, len)
        d = zeros(len,1);
        for i = 1:len
            d(i) = gen.get_double();
        end        
    end

    function verify_double_array(gen, d, len)
        assert(numel(d) == len)
        for i = 1:len
            assert (d(i) == gen.get_double())
        end        
    end

    function d = create_uint32_array(gen, len)
        d = uint32(zeros(len,1));
        for i = 1:len
            d(i) = gen.get_uint32();
        end        
    end

    function verify_uint32_array(gen, d, len)
        assert(numel(d) == len)
        for i = 1:len
            assert (d(i) == gen.get_uint32())
        end        
    end

    function d = create_int8_array(gen, len)
        d = int8(zeros(len,1));
        for i = 1:len
            d(i) = gen.get_int8();
        end        
    end

    function verify_int8_array(gen, d, len)
        assert(numel(d) == len)
        for i = 1:len
            assert (d(i) == gen.get_int8())
        end        
    end

    function s = fill_testpod1(seed)
        gen = ServiceTest2_test_sequence_gen(seed);
        s=struct;
        s.d1 = gen.get_double();
        s.d2 = create_double_array(gen, 6);
        s.d3 = create_double_array(gen, mod(gen.get_uint32(),6));
        s.d4 = reshape(create_double_array(gen,9),3,3);

        s.s1 = fill_testpod2(gen.get_uint32());
        s.s2 = create_testpod2_array(gen, 8);
        s.s3 = create_testpod2_array(gen, mod(gen.get_uint32(), 9));
        s.s4 = reshape(create_testpod2_array(gen, 8),2,4);
        
        s.t1 = fill_transform(gen.get_uint32());
        s.t2 = zeros(7,4);
        for i=1:4
           s.t2(:,i) = fill_transform(gen.get_uint32());
        end
        
        t3_len = mod(gen.get_uint32(), 15);
        s.t3 = zeros(7,t3_len);
        for i=1:t3_len
            s.t3(:,i) = fill_transform(gen.get_uint32());
        end
        
        t4_1=zeros(7,8);
        for i=1:8
           t4_1(:,i) = fill_transform(gen.get_uint32());
        end
        s.t4 = reshape(t4_1, 7, 2, 4);        
    end

    function verify_testpod1(s, seed)
        gen = ServiceTest2_test_sequence_gen(seed);
        assert(s.d1 == gen.get_double())
        verify_double_array(gen, s.d2, 6)
        verify_double_array(gen, s.d3, mod(gen.get_uint32(), 6))

        verify_double_array(gen, s.d4, 9)
        assert(all(size(s.d4) == [3,3]))
        verify_testpod2(s.s1, gen.get_uint32())
        verify_testpod2_array(gen, s.s2, 8)
        verify_testpod2_array(gen, s.s3, mod(gen.get_uint32(), 9))
        verify_testpod2_array(gen, s.s4, 8)
        assert(all(size(s.s4)==[2,4]))
        
        verify_transform(s.t1, gen.get_uint32())
        for i=1:4
            verify_transform(s.t2(:,i), gen.get_uint32())
        end
        
        t3_len = mod(gen.get_uint32(), 15);
        assert (t3_len == size(s.t3,2))
        
        for i=1:size(s.t3,2)
            verify_transform(s.t3(:,i), gen.get_uint32())
        end
        
        assert(all(size(s.t4) == [7, 2,4]))
        t4_1 = reshape(s.t4,7,8);
        for i=1:8
           verify_transform(t4_1(:,i), gen.get_uint32())
        end
    end

    function s = fill_testpod2(seed)
        gen = ServiceTest2_test_sequence_gen(seed);
        s=struct;
        s.i1 = gen.get_int8();
        s.i2 = create_int8_array(gen, 15);
        s.i3 = create_int8_array(gen, mod(gen.get_uint32(), 15));    
    end

    function verify_testpod2(s, seed)
        gen = ServiceTest2_test_sequence_gen(seed);
        assert (s.i1 == gen.get_int8())
        verify_int8_array(gen, s.i2, 15)
        verify_int8_array(gen, s.i3, mod(gen.get_uint32(), 15))
    end

    function s = create_testpod1_array(len, seed)
        gen = ServiceTest2_test_sequence_gen(seed);
        if (len > 0)
        s = fill_testpod1(gen.get_uint32());
        for i=2:len
           s(i,1) = fill_testpod1(gen.get_uint32());
        end
        else
            s=struct([]);
        end
    end
    
    function verify_testpod1_array(v, len, seed)

        gen = ServiceTest2_test_sequence_gen(seed);
        assert (numel(v) == len)
        for i = 1:len
            verify_testpod1(v(i), gen.get_uint32());
        end
    end
    
    function s = create_testpod1_multidimarray(m, n, seed)
       s1 =  create_testpod1_array(m*n,seed);
       s=reshape(s1,m,n);
    end

    function verify_testpod1_multidimarray(v, m, n, seed)
        verify_testpod1_array(v,m*n,seed)
        assert(all(size(v) == [m,n]))
    end

    function s = create_testpod2_array(gen, len)
        if (len > 0)
        s = fill_testpod2(gen.get_uint32());
        for i=2:len
           s(i,1) = fill_testpod2(gen.get_uint32());
        end
        else
           s=struct([]);
        end
    end

    function verify_testpod2_array(gen, v, len)
        assert (numel(v) == len)
        for i=1:len   
            verify_testpod2(v(i), gen.get_uint32());
        end
    end

    function o = fill_teststruct3(seed)
        o=struct;
        gen = ServiceTest2_test_sequence_gen(seed);
        o.s1 = fill_testpod1(gen.get_uint32());
        s2_seed = gen.get_uint32();
        o.s2 = create_testpod1_array(mod(s2_seed, 17), s2_seed);
        o.s3 = create_testpod1_array(11, gen.get_uint32());
        s4_seed = gen.get_uint32();
        o.s4 = create_testpod1_array(mod(s4_seed, 16), s4_seed);
        o.s5 = create_testpod1_multidimarray(3, 3, gen.get_uint32());
        s6_seed = gen.get_uint32();
        o.s6 = create_testpod1_multidimarray(mod(s6_seed, 6), mod(s6_seed,3), s6_seed);
        o.s7 = {};
        o.s7{1}=(fill_testpod1(gen.get_uint32()));

        o.s8 ={};
        o.s8{1,1} = (create_testpod1_array(2, gen.get_uint32()));
        o.s8{2,1} = (create_testpod1_array(4, gen.get_uint32()));

        o.s9 = {};
        o.s9{1,1} = (create_testpod1_multidimarray(2, 3, gen.get_uint32()));
        o.s9{2,1} = (create_testpod1_multidimarray(4, 5, gen.get_uint32()));

        
        function b = add_varvalue(a)
           for i=1:numel(a)
            a(i).RobotRaconteurStructureType='com.robotraconteur.testing.TestService3.testpod1';
           end
           b=a;
        end
        
        o.s10 = add_varvalue(fill_testpod1(gen.get_uint32()));
        
        o.s11 = add_varvalue(create_testpod1_array(3, gen.get_uint32()));
        o.s12 = add_varvalue(create_testpod1_multidimarray(2, 2, gen.get_uint32()));

        s13 = add_varvalue(fill_testpod1(gen.get_uint32()));
        o.s13 = { s13};

        s14 = {};
        s14{1,1} = add_varvalue(create_testpod1_array(3, gen.get_uint32()));
        s14{2,1} = add_varvalue(create_testpod1_array(5, gen.get_uint32()));
        o.s14 = s14;

        s15 = {};
        s15{1,1} = add_varvalue(create_testpod1_multidimarray(7, 2, gen.get_uint32()));
        s15{2,1} = add_varvalue(create_testpod1_multidimarray(5, 1, gen.get_uint32()));
        o.s15 = s15;
        
        o.t1 = fill_transform(gen.get_uint32());
        o.t2 = create_transform_array(4,gen.get_uint32());
        o.t3 = create_transform_multidimarray(2,4,gen.get_uint32());
        
        o.t4 = create_transform_array(10, gen.get_uint32());
        o.t5 = create_transform_multidimarray(6, 5, gen.get_uint32());
        
        t6 = {};
        t6{1,1} = fill_transform(gen.get_uint32());
        o.t6 = t6;
        
        t7 = {};
        t7{1,1} = create_transform_array(4, gen.get_uint32());
        t7{2,1} = create_transform_array(4, gen.get_uint32());
        o.t7 = t7;
        
        t8 = {};
        t8{1,1} = create_transform_multidimarray(2, 4, gen.get_uint32());
        t8{2,1} = create_transform_multidimarray(2, 4, gen.get_uint32());
        o.t8 = t8;
        
        t9 = {};
        t9{1,1} = fill_transform(gen.get_uint32());
        o.t9 = t9;
        
        t10 = {};
        t10{1,1} = create_transform_array(3, gen.get_uint32());
        t10{2,1} = create_transform_array(5, gen.get_uint32());
        o.t10 = t10;
        
        t11 = {};
        t11{1,1} = create_transform_multidimarray(7, 2, gen.get_uint32());
        t11{2,1} = create_transform_multidimarray(5, 1, gen.get_uint32());
        o.t11 = t11;
    
    end

    function verify_teststruct3(v, seed)
        gen = ServiceTest2_test_sequence_gen(seed);

        verify_testpod1(v.s1, gen.get_uint32())
        s2_seed = gen.get_uint32();
        verify_testpod1_array(v.s2, mod(s2_seed, 17), s2_seed)
        verify_testpod1_array(v.s3, 11, gen.get_uint32())
        s4_seed = gen.get_uint32();
        verify_testpod1_array(v.s4, mod(s4_seed, 16), s4_seed)
        verify_testpod1_multidimarray(v.s5, 3, 3, gen.get_uint32())
        s6_seed = gen.get_uint32();
        verify_testpod1_multidimarray(v.s6, mod(s6_seed, 6), mod(s6_seed, 3), s6_seed)

        assert(numel(v.s7) == 1)
        s7_0 = v.s7{1};
        verify_testpod1(s7_0, gen.get_uint32())

        assert(numel(v.s8)==2)
        verify_testpod1_array(v.s8{1}, 2, gen.get_uint32())
        verify_testpod1_array(v.s8{2}, 4, gen.get_uint32())

        assert(numel(v.s9) == 2)
        verify_testpod1_multidimarray(v.s9{1}, 2, 3, gen.get_uint32())
        verify_testpod1_multidimarray(v.s9{2}, 4, 5, gen.get_uint32())

        s10 = v.s10;
        verify_testpod1(s10, gen.get_uint32())

        verify_testpod1_array(v.s11, 3, gen.get_uint32())
        verify_testpod1_multidimarray(v.s12, 2, 2, gen.get_uint32())

        s13 = v.s13{1};
        verify_testpod1(s13(1), gen.get_uint32())

        v14 = v.s14;
        assert(numel(v14) == 2)
        verify_testpod1_array(v14{1}, 3, gen.get_uint32())
        verify_testpod1_array(v14{2}, 5, gen.get_uint32())

        v15 = v.s15;
        assert  (numel(v15) == 2) 
        verify_testpod1_multidimarray(v15{1}, 7, 2, gen.get_uint32())
        verify_testpod1_multidimarray(v15{2}, 5, 1, gen.get_uint32())
    end

    function o = fill_transform(seed)
        o=zeros(7,1);
        gen = ServiceTest2_test_sequence_gen(seed);
        for i=1:7
            o(i) = gen.get_double();
        end        
    end

    function  verify_transform(v, seed)
        assert(all(size(v) == [7,1]))
        gen = ServiceTest2_test_sequence_gen(seed);
        for i=1:7
            assert (v(i) == gen.get_double());
        end
    end

    function o = create_transform_array(s, seed)
        gen = ServiceTest2_test_sequence_gen(seed);
        o=zeros(7,s);
        for i=1:s
           o(:,i) = fill_transform(gen.get_uint32());
        end        
    end

    function verify_transform_array(v, s, seed)
        assert(size(v,2) == s)
        gen = ServiceTest2_test_sequence_gen(seed);
        for i=1:s
           assert (v(:,i) == fill_transform_array(gen.get_uint32()));
        end
    end

    function s = create_transform_multidimarray(m, n, seed)
       s1 =  create_transform_array(m*n,seed);
       s=reshape(s1,7,m,n);
    end

    function verify_transform_multidimarray(v, m, n, seed)
        verify_transform_array(reshape(v,7,m*n),m*n,seed)
        assert(all(size(v) == [7,m,n]))
    end

    

end

