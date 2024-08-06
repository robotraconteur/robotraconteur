import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import numpy as np


class ValueTypesExample_impl:
    # Scalar numbers
    @property
    def a_double(self):
        return 5.78

    @a_double.setter
    def a_double(self, value):
        assert (value == 49.3)

    @property
    def b_int(self):
        return 4557

    @b_int.setter
    def b_int(self, value):
        assert (value == 359)

    @property
    def c_byte(self):
        return 0x1A

    @c_byte.setter
    def c_byte(self, value):
        assert (value == 31)

    @property
    def d_cdouble(self):
        return 23.7 + 1j * 5.3

    @d_cdouble.setter
    def d_cdouble(self, value):
        assert (value == 1.2 + 3.4j)

    @property
    def e_bool(self):
        return True

    @e_bool.setter
    def e_bool(self, value):
        assert (value == False)

    @property
    def meaning_of_life(self):
        return 42

    # Numeric Arrays
    @property
    def a_double_array(self):
        return [0.016, 0.226]

    @a_double_array.setter
    def a_double_array(self, value):
        np.testing.assert_allclose(value, [0.582, 0.288, 0.09, 0.213, 0.98])

    @property
    def a_double_array_fixed(self):
        return [0.13, 0.27, 0.15]

    @a_double_array_fixed.setter
    def a_double_array_fixed(self, value):
        np.testing.assert_allclose(value, [0.21, 0.12, 0.39])

    @property
    def a_double_array_maxlen(self):
        return [0.7, 0.16, 0.16, 0.05, 0.61, 0.9]

    @a_double_array_maxlen.setter
    def a_double_array_maxlen(self, value):
        np.testing.assert_allclose(value, [0.035, 0.4])

    @property
    def a_double_marray_fixed(self):
        return np.array([[0.29, 0.66], [0.41, 0.6], [0.4, 0.2]], dtype=np.float64)

    @a_double_marray_fixed.setter
    def a_double_marray_fixed(self, value):
        np.testing.assert_allclose(value, [[0.3, 0.6], [0.4, 0.6], [0.5, 0.2]])

    @property
    def a_double_marray(self):
        return np.array([[0.72, 0.4], [0.05, 0.07]], dtype=np.float64)

    @a_double_marray.setter
    def a_double_marray(self, value):
        np.testing.assert_allclose(value, [[0.3], [0.01]])

    @property
    def c_byte_array(self):
        return [0x1A, 0x2B]

    @c_byte_array.setter
    def c_byte_array(self, value):
        np.testing.assert_array_equal(value, [0x3C, 0x4D, 0x5E, 0x6F, 0x70])

    # Strings
    @property
    def f_string(self):
        return "An example string read from the service"

    @f_string.setter
    def f_string(self, value):
        assert (value == "An example string written to the service")

    # Structures
    @property
    def k_struct(self):
        s = RRN.NewStructure("experimental.value_types.MyStructure")
        fill_MyStructure(s)
        return s

    @k_struct.setter
    def k_struct(self, value):
        assert_MyStructure(value)

    # NamedArrays
    @property
    def g_vector(self):
        a = np.zeros((1,), dtype=RRN.GetNamedArrayDType("experimental.value_types.MyVector3"))
        a[0]["x"] = 4.0
        a[0]["y"] = 5.0
        a[0]["z"] = 6.0
        return a

    @g_vector.setter
    def g_vector(self, value):
        assert (value[0]["x"] == 1.0)
        assert (value[0]["y"] == 2.0)
        assert (value[0]["z"] == 3.0)

    @property
    def g_vector_array(self):
        f = np.zeros((2,), dtype=RRN.GetNamedArrayDType("experimental.value_types.MyVector3"))
        f[0]["x"] = 7.0
        f[0]["y"] = 8.0
        f[0]["z"] = 9.0
        f[1]["x"] = 10.0
        f[1]["y"] = 11.0
        f[1]["z"] = 12.0
        return f

    @g_vector_array.setter
    def g_vector_array(self, value):
        assert (value[0]["x"] == 1.0)
        assert (value[0]["y"] == 2.0)
        assert (value[0]["z"] == 3.0)
        assert (value[1]["x"] == 4.0)
        assert (value[1]["y"] == 5.0)
        assert (value[1]["z"] == 6.0)

    @property
    def h_quaternion(self):
        j = np.zeros((1,), dtype=RRN.GetNamedArrayDType("experimental.value_types.MyQuaternion"))
        j[0]["w"] = 0.707
        j[0]["x"] = 0.0
        j[0]["y"] = 0.707
        j[0]["z"] = 0.0
        return j

    @h_quaternion.setter
    def h_quaternion(self, value):
        assert (value[0]["w"] == 1.0)
        assert (value[0]["x"] == 0.0)
        assert (value[0]["y"] == 0.0)
        assert (value[0]["z"] == 0.0)

    @property
    def i_pose(self):
        l = np.zeros((1,), dtype=RRN.GetNamedArrayDType("experimental.value_types.MyPose"))
        l[0]["orientation"]["w"] = 0.707
        l[0]["orientation"]["x"] = 0.0
        l[0]["orientation"]["y"] = 0.707
        l[0]["orientation"]["z"] = 0.0
        l[0]["position"]["x"] = 4.0
        l[0]["position"]["y"] = 5.0
        l[0]["position"]["z"] = 6.0
        return l

    @i_pose.setter
    def i_pose(self, value):
        assert (value[0]["orientation"]["w"] == 1.0)
        assert (value[0]["orientation"]["x"] == 0.0)
        assert (value[0]["orientation"]["y"] == 0.0)
        assert (value[0]["orientation"]["z"] == 0.0)
        assert (value[0]["position"]["x"] == 1.0)
        assert (value[0]["position"]["y"] == 2.0)
        assert (value[0]["position"]["z"] == 3.0)


def fill_MyStructure(s, i=0):
    s.a = 52 + i
    s.b = np.array([110, 120], dtype=np.uint32)
    s.c = "String from structure service"
    s.d = [
        "string c",
        "string d",
        "string e"
    ]


def assert_MyStructure(u, i=0):
    assert u is not None
    assert (u.a == 5 + i)
    np.testing.assert_array_equal(u.b, np.array([10, 20], dtype=np.uint32))
    assert (u.c == "String from structure client")
    assert (len(u.d) == 2)
    assert (u.d[0] == "string a")
    assert (u.d[1] == "string b")


def main():
    with RR.ServerNodeSetup("experimental.value_types", 53223) as node_setup:

        # Register the service type
        RRN.RegisterServiceTypeFromFile("../robdef/experimental.value_types.robdef")

        # Create the object
        obj = ValueTypesExample_impl()

        # Register the service
        RRN.RegisterService("values_example", "experimental.value_types.ValueTypesExample", obj)

        # Wait for the user to quit
        input("Server started, press enter to quit...")


if __name__ == '__main__':
    main()
