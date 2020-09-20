using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using com.robotraconteur.testing.TestService5;
using RobotRaconteur;
using System.Threading;
using System.IO;
using System.Threading.Tasks;

namespace RobotRaconteurNETTest
{
    public class ServiceTestClient3
    {

        public void ConnectService(string url)
        {
            r = (asynctestroot)RobotRaconteurNode.s.ConnectService(url);
        }

        public void DisconnectService()
        {
            RobotRaconteurNode.s.DisconnectService(r);
        }

        
        asynctestroot r;

        public void RunFullTest(string url)
        {
            ConnectService(url);

            TestProperties();

            //TestFunctions();

            DisconnectService();
        }

        public void TestProperties()
        {
            if (r.d1 != 8.5515)
            {
                throw new Exception("");
            }

            r.d1 = 3.0819;
            
            bool thrown1 = false;
            try
            {
                var res = r.err;
            }
            catch(ArgumentException err)
            {
                thrown1= true;
                if (err.Message != "Test message 1")
                {
                    throw new Exception("");
                }
            }
            if (!thrown1)
            {
                throw new Exception("");
            }

            bool thrown2 = false;
            try
            {
                r.err = 10;
            }
            catch(InvalidOperationException err)
            {
                thrown2= true;
                if (err.Message != "")
                {
                    throw new Exception("");
                }
            }
            if (!thrown2)
            {
                throw new Exception("");
            }
        }

        public void TestFunctions()
        {
            r.f1();
            r.f2(247);

            bool thrown1 = false;
            try
            {
                r.err_func();
            }
            catch (InvalidOperationException)
            {
                thrown1 = true;
            }
            if (!thrown1)
            {
                throw new Exception("");
            }

            bool thrown2 = false;
            try
            {
                r.err_func2();
            }
            catch (com.robotraconteur.testing.TestService5.asynctestexp)
            {
                thrown2 = true;
            }
            if (!thrown2)
            {
                throw new Exception("");
            }
        }
    }
}