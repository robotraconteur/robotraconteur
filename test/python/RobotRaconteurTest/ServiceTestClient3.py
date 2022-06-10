from __future__ import absolute_import

import RobotRaconteur as RR
RRN = RR.RobotRaconteurNode.s
import pytest


class ServiceTestClient3:
    def __init__(self):
        pass

    def RunFullTest(self, url):
        self.ConnectService(url)

        self.TestProperties()

        self.TestFunctions()

        self.DisconnectService()

    def ConnectService(self, url):
        self._r = RRN.ConnectService(url)

    def DisconnectService(self):
        RRN.DisconnectService(self._r)

    def TestProperties(self):
        _ = self._r.d1
        self._r.d1 = 3.0819

        try:
            _ = self._r.err
        except RR.InvalidArgumentException as e:
            assert e.message == "Test message 1"
        else:
            assert False

        with pytest.raises(RR.InvalidOperationException):
            self._r.err = 100

    def TestFunctions(self):

        self._r.f1()
        self._r.f2(247)

        res = self._r.f3(1, 2)
        assert res == 3

        with pytest.raises(RR.InvalidOperationException):
            self._r.err_func()

        asynctestexp = RRN.GetExceptionType(
            "com.robotraconteur.testing.TestService5.asynctestexp", self._r)
        try:
            self._r.err_func2()
        except asynctestexp:
            pass
        else:
            assert False
