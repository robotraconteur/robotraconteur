import RobotRaconteur as RR
from RobotRaconteur.RobotRaconteurPythonUtil import PackMessageElement, UnpackMessageElement


def test_pack_unicode_string_non_ascii():
    # A non-ASCII unicode string must be packed as UTF-8 (on Python 2 the ASCII codec was used
    # and the pack failed with "Invalid string")
    s = u"\u00e9t\u00e9 \u65e5\u672c"
    m = PackMessageElement(s, "string value")
    s2 = UnpackMessageElement(m, "string value")
    assert s2 == s
