from RobotRaconteur.Client import *
import pytest


def _handler(ev):
    pass


def test_timer_period_not_whole_microseconds():
    # Durations that are not a whole number of microseconds were converted with
    # boost::lexical_cast<int64_t>(double), which threw bad_lexical_cast outside the
    # SWIG exception handler and aborted the interpreter.
    node = RR.RobotRaconteurNode()
    node.Init()
    try:
        t = node.CreateTimer(1.0 / 30.0, _handler, True)
        assert abs(t.GetPeriod() - 1.0 / 30.0) < 1e-6

        t.SetPeriod(1.0 / 3.0)
        assert abs(t.GetPeriod() - 1.0 / 3.0) < 1e-6

        t.SetPeriod(2)
        assert t.GetPeriod() == 2.0

        with pytest.raises(OverflowError):
            t.SetPeriod(1e300)
        with pytest.raises(OverflowError):
            t.SetPeriod(float("nan"))
        with pytest.raises(OverflowError):
            t.SetPeriod(2**70)
    finally:
        node.Shutdown()
