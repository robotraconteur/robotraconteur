//This file is automatically generated. DO NOT EDIT!
package com.robotraconteur.testing.TestService3;
import java.util.*;
import com.robotraconteur.*;
public class pixel_stub extends NamedArrayStub<pixel,UnsignedBytes> {
    public UnsignedBytes getNumericArrayFromNamedArrayStruct(pixel s) {
    return s.getNumericArray();
    }
    public pixel getNamedArrayStructFromNumericArray(UnsignedBytes m) {
    pixel s = new pixel();
    s.assignFromNumericArray(m,0);
    return s;
    }
    public UnsignedBytes getNumericArrayFromNamedArray(pixel[] s) {
    return pixel.getNumericArray(s);
    }
    public pixel[] getNamedArrayFromNumericArray(UnsignedBytes m) {
    pixel[] s = new pixel[m.value.length / 3];
    for (int i=0; i<s.length; i++) s[i] = new pixel();
    pixel.assignFromNumericArray(s,m,0);
    return s;
    }
    public String getTypeName() { return "com.robotraconteur.testing.TestService3.pixel"; }}
