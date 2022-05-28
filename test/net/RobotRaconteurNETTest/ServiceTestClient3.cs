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

        // TestFunctions();

        DisconnectService();
    }

    public void TestProperties()
    {
        RRAssert.AreEqual(r.d1, 8.5515);

        r.d1 = 3.0819;

        bool thrown1 = false;
        try
        {
            var res = r.err;
        }
        catch (ArgumentException err)
        {
            thrown1 = true;
            RRAssert.AreEqual(err.Message, "Test message 1");
        }
        RRAssert.IsTrue(thrown1);

        bool thrown2 = false;
        try
        {
            r.err = 10;
        }
        catch (InvalidOperationException err)
        {
            thrown2 = true;
            RRAssert.AreEqual(err.Message, "");
        }
        RRAssert.IsTrue(thrown2);
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
        RRAssert.IsTrue(thrown1);

        bool thrown2 = false;
        try
        {
            r.err_func2();
        }
        catch (com.robotraconteur.testing.TestService5.asynctestexp)
        {
            thrown2 = true;
        }
        RRAssert.IsTrue(thrown2);
    }
}
}