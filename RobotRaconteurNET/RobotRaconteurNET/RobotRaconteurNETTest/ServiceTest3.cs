using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using System.Threading;
using System.Threading.Tasks;
using com.robotraconteur.testing.TestService5;

namespace RobotRaconteurNETTest
{

public class RobotRaconteurTestServiceSupport3
{

    public asynctestroot_impl testservice3;

    public void RegisterServices()
    {
        testservice3 = new asynctestroot_impl();
        var context = RobotRaconteurNode.s.RegisterService("RobotRaconteurTestService3",
                                                           "com.robotraconteur.testing.TestService5", testservice3);
    }

    public void UnregisterServices()
    {}
}

public class asynctestroot_impl : asynctestroot_default_impl, async_asynctestroot
{
    public async Task<double> async_get_d1(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        await Task.Delay(10);
        return 8.5515;
    }
    public async Task async_set_d1(double value, int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        await Task.Delay(10);
        if (value != 3.0819)
        {
            throw new ArgumentException("");
        }
    }
    public Task<string> async_get_s1(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public Task async_set_s1(string value, int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public Task<double[]> async_get_d2(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public Task async_set_d2(double[] value, int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public Task<List<string>> async_get_s2(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public Task async_set_s2(List<string> value, int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public Task<teststruct4> async_get_s3(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public Task async_set_s3(teststruct4 value, int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public async Task<double> async_get_err(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        await Task.Delay(10);
        throw new ArgumentException("Test message 1");
    }
    public async Task async_set_err(double value, int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        await Task.Delay(10);
        throw new InvalidOperationException("");
    }
    public async Task async_f1(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        await Task.Delay(10);
    }
    public async Task async_f2(int a, int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        await Task.Delay(10);
        if (a != 247)
        {
            throw new ArgumentException("");
        }
    }
    public async Task<int> async_f3(double a, double b, int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        await Task.Delay(10);
        return (int)(a + b);
    }
    public Task<Generator2<double>> async_f4(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public Task<Generator3<double>> async_f5(double a, int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public Task<Generator1<teststruct4, teststruct5>> async_f6(double a,
                                                               int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
    public async Task async_err_func(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        await Task.Delay(10);
        throw new InvalidOperationException("");
    }
    public async Task<double> async_err_func2(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        await Task.Delay(10);
        throw new com.robotraconteur.testing.TestService5.asynctestexp("");
    }
    public Task<Generator2<double>> async_err_func3(int rr_timeout = RobotRaconteurNode.RR_TIMEOUT_INFINITE)
    {
        throw new NotImplementedException("");
    }
}

}