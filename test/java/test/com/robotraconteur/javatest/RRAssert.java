package com.robotraconteur.javatest;

class RRAssert
{

    public static void areEqual(byte a, byte b)
    {
        if (a != b)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is not equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void areEqual(short a, short b)
    {
        if (a != b)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is not equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void areEqual(int a, int b)
    {
        if (a != b)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is not equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void areEqual(long a, long b)
    {
        if (a != b)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is not equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void areEqual(Object a, Object b)
    {
        if (a == null && b == null)
        {
            return;
        }
        if (a == null || b == null)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is not equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
        if (!a.equals(b))
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is not equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void areNotEqual(Object a, Object b)
    {
        if (a == null && b == null)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
        if (a == null || b == null)
        {
            return;
        }
        if (a.equals(b))
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void areNotEqual(byte a, byte b)
    {
        if (a == b)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void areNotEqual(short a, short b)
    {
        if (a == b)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void areNotEqual(int a, int b)
    {
        if (a == b)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void areNotEqual(long a, long b)
    {
        if (a == b)
        {
            System.out.println("Unit test failure: " + String.valueOf(a) + " is equal to " + String.valueOf(b));
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void fail()
    {
        throw new RuntimeException("Unit test failure");
    }

    public static void isTrue(boolean val)
    {
        if (!val)
        {
            throw new RuntimeException("Unit test failure");
        }
    }

    public static void isFalse(boolean val)
    {
        if (val)
        {
            throw new RuntimeException("Unit test failure");
        }
    }
}