package com.robotraconteur;

class AndroidHardwareDirectorImpl extends AndroidHardwareDirector
{
    public AndroidHardwareDirectorImpl()
    {
        super();
        setDirector(this);
    }

    @Override protected void connectBluetooth2(SWIGTYPE_p_void p)
    {
        AndroidBluetoothConnector_params p2 = AndroidHardwareHelper.voidToAndroidBluetoothConnector_params(p);
        AndroidBluetoothConnector c = new AndroidBluetoothConnector();
        c.connectBluetooth(p2);
    }

    protected void finalize()
    {
        clearDirector(this);
        super.finalize();
    }

    static final AndroidHardwareDirectorImpl impl = new AndroidHardwareDirectorImpl();
}