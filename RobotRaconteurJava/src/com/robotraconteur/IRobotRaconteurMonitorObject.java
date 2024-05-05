package com.robotraconteur;

public interface IRobotRaconteurMonitorObject
{
    void robotRaconteurMonitorEnter();

    void robotRaconteurMonitorEnter(int timeout);

    void robotRaconteurMonitorExit();
}
