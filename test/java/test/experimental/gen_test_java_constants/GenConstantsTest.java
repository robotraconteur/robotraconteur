package experimental.gen_test_java_constants;

import com.robotraconteur.*;

// Checks the values of the numeric constants generated for experimental.gen_test_java_constants.robdef. In the
// package of the generated code because the generated constants class is not public.
public class GenConstantsTest
{
    static void check(boolean v, String name)
    {
        if (!v)
        {
            throw new RuntimeException("Unexpected value of generated constant " + name);
        }
    }

    public static void run()
    {
        check(experimental__gen_test_java_constantsConstants.f1 == 1.5f, "f1");
        check(experimental__gen_test_java_constantsConstants.f2.length == 3, "f2");
        check(experimental__gen_test_java_constantsConstants.f2[2] == -3.0f, "f2");
        check(experimental__gen_test_java_constantsConstants.d1 == 2.5, "d1");
        check(experimental__gen_test_java_constantsConstants.i1 == -9223372036854775807L, "i1");
        check(experimental__gen_test_java_constantsConstants.i2[0] == 9223372036854775807L, "i2");
        check(experimental__gen_test_java_constantsConstants.i2[2] == 16L, "i2");
        check(experimental__gen_test_java_constantsConstants.u0.value == (byte)7, "u0");
        check(experimental__gen_test_java_constantsConstants.u1.value == (short)65535, "u1");
        check(experimental__gen_test_java_constantsConstants.u2[0].value == (short)65535, "u2");
        check(experimental__gen_test_java_constantsConstants.u3.value == -1, "u3");
        check(experimental__gen_test_java_constantsConstants.u4[0].value == -1, "u4");
        check(experimental__gen_test_java_constantsConstants.u4[1].value == 2, "u4");
        check(experimental__gen_test_java_constantsConstants.u5.value == -1L, "u5");
        check(experimental__gen_test_java_constantsConstants.u6[0].value == 0L, "u6");
        check(experimental__gen_test_java_constantsConstants.u6[1].value == -1L, "u6");
    }
}
