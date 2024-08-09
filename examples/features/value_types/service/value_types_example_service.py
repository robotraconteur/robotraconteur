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

    @property
    def j_pod(self):
        a = np.zeros((1,), dtype=RRN.GetNamedArrayDType("experimental.value_types.MyPod"))
        a[0]["a"] = 0.791
        a[0]["b"] = 1077
        a[0]["c"] = [61, 52, 33, 24]
        a[0]["d"]["len"] = 2
        a[0]["d"]["array"][0:2] = [1, 2]
        a[0]["e"] = [[7, 8, 9], [10, 11, 12]]
        a[0]["f"]["x"] = 20.1
        a[0]["f"]["y"] = 20.2
        a[0]["f"]["z"] = 20.3
        return a

    @j_pod.setter
    def j_pod(self, value):
        assert (value[0]["a"] == 0.928)
        assert (value[0]["b"] == 8374)
        np.testing.assert_array_equal(value[0]["c"], [8, 9, 10, 11])
        assert (value[0]["d"]["len"] == 1)
        np.testing.assert_array_equal(value[0]["d"]["array"][0:1], [17])
        np.testing.assert_array_equal(value[0]["e"], [[1, 2, 3], [4, 5, 6]])
        assert (value[0]["f"]["x"] == 10.1)
        assert (value[0]["f"]["y"] == 10.2)
        assert (value[0]["f"]["z"] == 10.3)

    @property
    def j_pod_array(self):
        d = np.zeros((2,), dtype=RRN.GetNamedArrayDType("experimental.value_types.MyPod"))
        d[0]["a"] = 0.791
        d[0]["b"] = 1077
        d[0]["c"] = [61, 52, 33, 24]
        d[0]["d"]["len"] = 2
        d[0]["d"]["array"][0:2] = [1, 2]
        d[0]["e"] = [[7, 8, 9], [10, 11, 12]]
        d[0]["f"]["x"] = 20.1
        d[0]["f"]["y"] = 20.2
        d[0]["f"]["z"] = 20.3
        d[1]["a"] = 0.03
        d[1]["b"] = 693
        d[1]["c"] = [5, 6, 7, 8]
        d[1]["d"]["len"] = 1
        d[1]["d"]["array"][0] = 3
        d[1]["e"] = [[19, 20, 21], [22, 23, 24]]
        d[1]["f"]["x"] = 40.1
        d[1]["f"]["y"] = 40.2
        d[1]["f"]["z"] = 40.3
        return d

    @j_pod_array.setter
    def j_pod_array(self, value):
        assert (value[0]["a"] == 0.928)
        assert (value[0]["b"] == 8374)
        np.testing.assert_array_equal(value[0]["c"], [8, 9, 10, 11])
        assert (value[0]["d"]["len"] == 1)
        np.testing.assert_array_equal(value[0]["d"]["array"][0:1], [17])
        np.testing.assert_array_equal(value[0]["e"], [[1, 2, 3], [4, 5, 6]])
        assert (value[0]["f"]["x"] == 10.1)
        assert (value[0]["f"]["y"] == 10.2)
        assert (value[0]["f"]["z"] == 10.3)
        assert (value[1]["a"] == 0.67)
        assert (value[1]["b"] == 123)
        np.testing.assert_array_equal(value[1]["c"], [1, 2, 3, 4])
        assert (value[1]["d"]["len"] == 2)
        np.testing.assert_array_equal(value[1]["d"]["array"][0:2], [5, 6])
        np.testing.assert_array_equal(value[1]["e"], [[13, 14, 15], [16, 17, 18]])
        assert (value[1]["f"]["x"] == 30.1)
        assert (value[1]["f"]["y"] == 30.2)
        assert (value[1]["f"]["z"] == 30.3)

    # Containers
    @property
    def l_double_map(self):
        return {1: 5.1, 5: 6.2}

    @l_double_map.setter
    def l_double_map(self, value):
        assert (len(value) == 3)
        assert (value[1] == 1.1)
        assert (value[2] == 2.2)
        assert (value[5] == 3.3)

    @property
    def l_double_array_map(self):
        return {
            "key3": np.array([5.1, 5.2, 5.3], dtype=np.float64),
            "key4": np.array([6.1, 6.2, 6.3], dtype=np.float64)
        }

    @l_double_array_map.setter
    def l_double_array_map(self, value):
        assert (len(value) == 2)
        np.testing.assert_array_equal(value["key1"], [1.1, 1.2, 1.3])
        np.testing.assert_array_equal(value["key2"], [2.1, 2.2, 2.3])

    @property
    def m_string_list(self):
        return ["string 4", "string 5"]

    @m_string_list.setter
    def m_string_list(self, value):
        assert (len(value) == 3)
        assert (value[0] == "string 1")
        assert (value[1] == "string 2")
        assert (value[2] == "string 3")

    @property
    def m_string_map_int32(self):
        return {
            87: "string 4",
            1: "string 5"
        }

    @m_string_map_int32.setter
    def m_string_map_int32(self, value):
        assert (len(value) == 3)
        assert (value[12] == "string 1")
        assert (value[100] == "string 2")
        assert (value[17] == "string 3")

    @property
    def m_string_map_string(self):
        return {
            "key3": "string 3",
            "key4": "string 4",
            "key5": "string 5"
        }

    @m_string_map_string.setter
    def m_string_map_string(self, value):
        assert (len(value) == 2)
        assert (value["key1"] == "string 1")
        assert (value["key2"] == "string 2")

    @property
    def n_vector_map(self):
        r = np.zeros((1,), dtype=RRN.GetNamedArrayDType("experimental.value_types.MyVector3"))
        r[0]["x"] = 4.0
        r[0]["y"] = 5.0
        r[0]["z"] = 6.0
        return {1: r}

    @n_vector_map.setter
    def n_vector_map(self, value):
        assert (len(value) == 1)
        assert (value[1][0]["x"] == 1.0)
        assert (value[1][0]["y"] == 2.0)
        assert (value[1][0]["z"] == 3.0)

    @property
    def o_struct_list(self):
        s = RRN.NewStructure("experimental.value_types.MyStructure")
        fill_MyStructure(s)
        return [s]

    @o_struct_list.setter
    def o_struct_list(self, value):
        assert (len(value) == 1)
        assert_MyStructure(value[0])

    @property
    def p_varvalue_double_array(self):
        return RR.VarValue(np.array([1.4, 2.5, 3.6], dtype=np.float64), "double[]")

    @p_varvalue_double_array.setter
    def p_varvalue_double_array(self, value):
        np.testing.assert_array_equal(value.data, [1.1, 2.2, 3.3])
        assert value.datatype == "double[]"

    @property
    def q_varvalue_string(self):
        return RR.VarValue("varvalue string from service", "string")

    @q_varvalue_string.setter
    def q_varvalue_string(self, value):
        assert value.data == "varvalue string from client"
        assert value.datatype == "string"

    @property
    def r_varvalue_struct(self):
        my_structure_type = RRN.GetStructureType("experimental.value_types.MyStructure")
        s = my_structure_type()
        fill_MyStructure(s)
        return RR.VarValue(s, "experimental.value_types.MyStructure")

    @r_varvalue_struct.setter
    def r_varvalue_struct(self, value):
        assert_MyStructure(value.data)
        assert value.datatype == "experimental.value_types.MyStructure"

    @property
    def s_varvalue_map2(self):
        return {
            "key3": RR.VarValue(np.array([4, 5], dtype=np.int32), "int32[]"),
            "key4": RR.VarValue("string 4", "string")
        }

    @s_varvalue_map2.setter
    def s_varvalue_map2(self, value):
        assert len(value) == 2
        np.testing.assert_array_equal(value["key1"].data, [2, 3])
        assert value["key1"].datatype == "int32[]"
        assert value["key2"].data == "string 2"
        assert value["key2"].datatype == "string"

    @property
    def t_struct_null(self):
        return None

    @t_struct_null.setter
    def t_struct_null(self, value):
        assert value is None


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
