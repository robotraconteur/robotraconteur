import RobotRaconteur as RR

_robdef = """
service experimental.service_constants

constant int32 myint 5
constant uint32 myhex 0x10
constant int16 myneg -3
constant double mydbl 2.5
constant int32[] myarr {1, 2}
constant string mystr "hello"
"""


def test_service_constants_types():
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        node.RegisterServiceType(_robdef)
        c = node.GetConstants("experimental.service_constants")
        # Scalar integer constants are returned as int like floats, arrays and enums are converted
        assert c["myint"] == 5 and isinstance(c["myint"], int)
        assert c["myhex"] == 16
        assert c["myneg"] == -3
        assert c["mydbl"] == 2.5
        assert c["myarr"] == [1, 2]
        assert c["mystr"] == "hello"
    finally:
        node.Shutdown()
