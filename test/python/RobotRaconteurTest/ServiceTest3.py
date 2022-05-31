from __future__ import absolute_import

import RobotRaconteur as RR


class asynctestroot_impl(object):

    def RRServiceObjectInit(self, ctx, service_path):
        self._node = ctx.GetNode()

    def async_get_d1(self, handler):
        self._node.PostToThreadPool(lambda: handler(8.5515, None))

    def async_set_d1(self, value, handler):
        if value != 3.0819:
            self._node.PostToThreadPool(
                lambda: handler(RR.InvalidArgumentException()))
            return
        self._node.PostToThreadPool(lambda: handler(None))

    def async_get_err(self, handler):
        self._node.PostToThreadPool(lambda: handler(
            None, RR.InvalidArgumentException("Test message 1")))

    def async_set_err(self, val, handler):
        self._node.PostToThreadPool(
            lambda: handler(RR.InvalidOperationException("")))

    def async_f1(self, handler):
        self._node.PostToThreadPool(lambda: handler(None))

    def async_f2(self, a, handler):
        if a != 247:
            self._node.PostToThreadPool(
                lambda: handler(RR.InvalidArgumentException()))
            return
        self._node.PostToThreadPool(lambda: handler(None))

    def async_f3(self, a, b, handler):
        res = int(a + b)
        self._node.PostToThreadPool(lambda: handler(res, None))

    def async_err_func(self, handler):
        self._node.PostToThreadPool(
            lambda: handler(RR.InvalidOperationException()))

    def async_err_func2(self, handler):
        asynctestexp = self._node.GetExceptionType(
            "com.robotraconteur.testing.TestService5.asynctestexp")
        self._node.PostToThreadPool(lambda: handler(None, asynctestexp("")))
