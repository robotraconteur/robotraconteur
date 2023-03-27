function RobotRaconteurMatlabEmptyArrayTest(url_test1, url_test2)
    c=RobotRaconteur.ConnectService(url_test1);
    c2=RobotRaconteur.ConnectService(url_test2);
    run_empty_test(@d2)
    run_empty_test(@i32_2)
    run_empty_test(@str1)
    run_empty_test(@list_d1)
    run_empty_test(@testnamedarray3)
    
    function d2()
        c.d2 = [];
    end

    function i32_2()
        c.i32_2 = int32([]);
    end

    function str1()
       c.str1 = '';
    end

    function list_d1()
       c.list_d1 = {}; 
    end

    function testnamedarray3()
       c2.testnamedarray3 = [];
    end

   function run_empty_test(f)
       try
        f();
       catch e
          if contains(e.message, 'RobotRaconteur.DataTypeError')
             disp(e)
             error('Invalid empty data handling in mex file')             
          end
       end
    end

end