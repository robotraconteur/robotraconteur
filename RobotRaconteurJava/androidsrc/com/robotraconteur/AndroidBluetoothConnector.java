package com.robotraconteur;

import android.bluetooth.*;
import android.content.*;
import android.os.*;
import android.util.*;

import java.io.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.*;

class AndroidBluetoothConnector implements Runnable
{
    Boolean connected = false;
    AndroidBluetoothConnector_params p;
    Thread t;

    public void connectBluetooth(AndroidBluetoothConnector_params p)
    {
        this.p = p;

        t = new Thread(this);
        t.run();
    }

    public void run()
    {
        UUID service_uuid = UUID.fromString("25bb0b62-861a-4974-a1b8-18ed5495aa07");

        long start = System.currentTimeMillis();
        BluetoothAdapter a = BluetoothAdapter.getDefaultAdapter();
        Set<BluetoothDevice> devs = a.getBondedDevices();

        if (devs.size() == 0)
        {
            AndroidHardwareHelper.connectBluetooth_error(p, "No bluetooth nodes available not found");
            return;
        }

        for (BluetoothDevice dev : devs)
        {
            try
            {

                dev.fetchUuidsWithSdp();
            }
            catch (Exception e)
            {}
        }

        while (true)
        {
            synchronized (this)
            {
                if (connected)
                {
                    return;
                }
                long now = System.currentTimeMillis();
                if ((now - start > 5000))
                {
                    AndroidHardwareHelper.connectBluetooth_error(p, "Bluetooth node not found");
                    return;
                }

                HashSet<BluetoothDevice> devs2 = new HashSet<BluetoothDevice>();
                for (BluetoothDevice dev : devs)
                {
                    boolean found = false;
                    ParcelUuid[] service_uuids = dev.getUuids();
                    if (service_uuids != null)
                    {
                        for (ParcelUuid u : service_uuids)
                        {
                            if (u.getUuid().compareTo(service_uuid) == 0)
                            {
                                found = true;
                                ReadNodeInfo nodereader = new ReadNodeInfo(dev);
                            }
                        }
                    }
                    if (!found)
                    {
                        devs2.add(dev);
                    }
                }

                devs = devs2;
            }

            try
            {
                Thread.sleep(100);
            }
            catch (InterruptedException e)
            {}
        }
    }

    protected void connected(BluetoothSocket sock)
    {
        synchronized (this)
        {
            try
            {

                if (connected)
                {
                    try
                    {
                        sock.close();
                    }
                    catch (IOException e)
                    {}
                    return;
                }
                else
                {
                    connected = true;

                    Log.d("RR", "Bluetooth Connected");

                    int fd0 = AndroidHardwareHelper.connectBluetooth_success(p);

                    ParcelFileDescriptor fd1 = ParcelFileDescriptor.adoptFd(fd0);
                    FileDescriptor fd2 = fd1.getFileDescriptor();
                    BluetoothSocketAdaptor a = new BluetoothSocketAdaptor(sock, fd2);
                }
            }
            catch (Exception e)
            {}
        }
    }

    class ReadNodeInfo implements Runnable
    {
        BluetoothDevice dev;
        public ReadNodeInfo(BluetoothDevice dev)
        {
            this.dev = dev;
            Thread t = new Thread(this);
            t.start();
        }

        public void run()
        {
            BluetoothSocket s = null;
            try
            {
                UUID service_uuid = UUID.fromString("25bb0b62-861a-4974-a1b8-18ed5495aa07");
                s = dev.createInsecureRfcommSocketToServiceRecord(service_uuid);
                s.connect();
                Message m = new Message();
                MessageHeader h = new MessageHeader();
                m.setHeader(h);
                MessageEntry mm = new MessageEntry();
                mm.setEntryType(MessageEntryType.MessageEntryType_StreamOp);
                mm.setMemberName("GetRemoteNodeID");
                m.getEntries().add(mm);

                vector_int8_t v1 = AndroidHardwareHelper.messageToVector(m);
                byte[] v2 = new byte[v1.size()];
                for (int i = 0; i < v2.length; i++)
                {
                    v2[i] = v1.get(i);
                }

                s.getOutputStream().write(v2, 0, v2.length);

                byte[] v3 = new byte[4096];

                int v3_l = s.getInputStream().read(v3);
                while (v3_l < 8)
                {
                    v3_l += s.getInputStream().read(v3, v3_l, v3.length - v3_l);
                }

                ByteBuffer b = ByteBuffer.wrap(v3, 0, v3_l);
                b.order(ByteOrder.LITTLE_ENDIAN);
                int v3_l_1 = b.getInt(4);

                while (v3_l < v3_l_1)
                {
                    v3_l += s.getInputStream().read(v3, v3_l, v3.length - v3_l);
                }

                vector_int8_t v4 = new vector_int8_t(v3_l);
                for (int i = 0; i < v3_l; i++)
                {
                    v4.set(i, v3[i]);
                }

                Message m2 = AndroidHardwareHelper.vectorToMessage(v4);
                String device_nodename = m2.getHeader().getSenderNodeName();
                NodeID device_nodeid = m2.getHeader().getSenderNodeID();

                boolean match = false;

                String nodename = p.getTarget_nodename();
                NodeID nodeid = p.getTarget_nodeid();

                if (!nodeid.isAnyNode() && !nodename.isEmpty())
                {
                    if (nodeid.equals(device_nodeid) && nodename.equals(device_nodename))
                    {
                        match = true;
                    }
                }
                else if (!nodename.isEmpty())
                {
                    if (nodename.equals(device_nodename))
                    {
                        match = true;
                    }
                }
                else if (!nodeid.isAnyNode())
                {
                    if (nodeid.equals(device_nodeid))
                    {
                        match = true;
                    }
                }

                if (match)
                {
                    connected(s);
                }
                else
                {
                    try
                    {
                        s.close();
                    }
                    catch (Exception e)
                    {}
                }
            }
            catch (Exception e)
            {
                if (s != null)
                {
                    try
                    {
                        s.close();
                    }
                    catch (Exception e1)
                    {}
                }
            }
        }
    }

    static class BluetoothSocketAdaptor
    {
        public BluetoothSocketAdaptor(BluetoothSocket s, FileDescriptor fd)
        {
            final BluetoothSocket s1 = s;
            final FileDescriptor fd1 = fd;

            Thread t1 = new Thread(new Runnable() {
                public void run()
                {
                    OutputStream out = null;
                    try
                    {
                        InputStream in = s1.getInputStream();
                        out = new FileOutputStream(fd1);
                        while (true)
                        {
                            byte[] buf = new byte[4096];
                            int n = in.read(buf, 0, buf.length);
                            if (n <= 0)
                            {
                                return;
                            }
                            out.write(buf, 0, n);
                        }
                    }
                    catch (Exception e)
                    {
                        e.printStackTrace(System.out);
                    }
                    finally
                    {
                        try
                        {
                            s1.close();
                        }
                        catch (Exception e1)
                        {}
                        try
                        {
                            if (out != null)
                            {
                                out.close();
                            }
                        }
                        catch (Exception e1)
                        {}
                    }
                }
            });

            Thread t2 = new Thread(new Runnable() {
                public void run()
                {
                    InputStream in = null;
                    try
                    {
                        in = new FileInputStream(fd1);
                        OutputStream out = s1.getOutputStream();
                        while (true)
                        {
                            byte[] buf = new byte[4096];
                            int n = in.read(buf, 0, buf.length);
                            if (n <= 0)
                            {
                                return;
                            }
                            out.write(buf, 0, n);
                        }
                    }
                    catch (Exception e)
                    {
                        e.printStackTrace(System.out);
                    }
                    finally
                    {
                        try
                        {
                            s1.close();
                        }
                        catch (Exception e1)
                        {}
                        try
                        {
                            if (in != null)
                            {
                                in.close();
                            }
                        }
                        catch (Exception e1)
                        {}
                    }
                }
            });

            t1.start();
            t2.start();
        }
    }
}
