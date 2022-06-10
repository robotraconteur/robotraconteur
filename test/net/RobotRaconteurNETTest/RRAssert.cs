using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using RobotRaconteur;
using System.IO;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace RobotRaconteurNETTest
{
static class RRAssert
{
    public static void AreEqual<T>(T a, T b, [CallerFilePath] string sourceFilePath = "",
                                   [CallerLineNumber] int sourceLineNumber = 0)
        where T : IComparable, IComparable<T>
    {
        if (a.CompareTo(b) != 0)
        {
            Console.WriteLine("Failure: {0} does not equal {1} at {2}:{3}", a, b, sourceFilePath, sourceLineNumber);
            throw new Exception("Unit test failure");
        }
    }

    public static void AreEqual(object a, object b, [CallerFilePath] string sourceFilePath = "",
                                [CallerLineNumber] int sourceLineNumber = 0)
    {
        if (!object.Equals(a, b))
        {
            Console.WriteLine("Failure: {0} does not equal {1} at {2}:{3}", a, b, sourceFilePath, sourceLineNumber);
            throw new Exception("Unit test failure");
        }
    }

    public static void AreNotEqual(object a, object b, [CallerFilePath] string sourceFilePath = "",
                                   [CallerLineNumber] int sourceLineNumber = 0)
    {
        if (object.Equals(a, b))
        {
            Console.WriteLine("Failure: {0} equals {1} at {2}:{3}", a, b, sourceFilePath, sourceLineNumber);
            throw new Exception("Unit test failure");
        }
    }

    public static void Fail([CallerFilePath] string sourceFilePath = "", [CallerLineNumber] int sourceLineNumber = 0)
    {
        Console.WriteLine("Failure: at {0}:{1}", sourceFilePath, sourceLineNumber);
        throw new Exception("Unit test failure");
    }

    public static void IsTrue(bool val, [CallerFilePath] string sourceFilePath = "",
                              [CallerLineNumber] int sourceLineNumber = 0)
    {
        if (!val)
        {
            Console.WriteLine("Failure: {0} is not true {1}:{2}", val, sourceFilePath, sourceLineNumber);
            throw new Exception("Unit test failure");
        }
    }

    public static void IsFalse(bool val, [CallerFilePath] string sourceFilePath = "",
                               [CallerLineNumber] int sourceLineNumber = 0)
    {
        if (val)
        {
            Console.WriteLine("Failure: {0} is not false {1}:{2}", val, sourceFilePath, sourceLineNumber);
            throw new Exception("Unit test failure");
        }
    }

    public static void ThrowsException<T>(Action f, [CallerFilePath] string sourceFilePath = "",
                                          [CallerLineNumber] int sourceLineNumber = 0)
        where T : Exception
    {
        bool thrown = false;
        try
        {
            f();
        }
        catch (T)
        {
            thrown = true;
        }
        if (!thrown)
        {
            Console.WriteLine("Failure: does not throw at {0}:{1}", sourceFilePath, sourceLineNumber);
            throw new Exception("Unit test failure");
        }
    }

    public static async Task ThrowsExceptionAsync<T>(Func<Task> f, [CallerFilePath] string sourceFilePath = "",
                                                     [CallerLineNumber] int sourceLineNumber = 0)
        where T : Exception
    {
        bool thrown = false;
        try
        {
            await f();
        }
        catch (T)
        {
            thrown = true;
        }
        if (!thrown)
        {
            Console.WriteLine("Failure: does not throw at {0}:{1}", sourceFilePath, sourceLineNumber);
            throw new Exception("Unit test failure");
        }
    }
}
}