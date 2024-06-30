
import com.robotraconteur.*;
import java.util.*;

import experimental.simplewebcam3.*;

public class WebcamHost_impl extends WebcamHost_default_impl implements WebcamHost
{

    public static class Webcam_name
    {

        public int index;
        public String name;

        public Webcam_name(int index, String name)
        {
            this.index = index;
            this.name = name;
        }
    }

    HashMap<Integer, Webcam_impl> webcams = new HashMap<Integer, Webcam_impl>();

    public WebcamHost_impl(Webcam_name[] names)
    {
        int camcount = 0;
        for (Webcam_name c : names)
        {
            Webcam_impl w = new Webcam_impl(c.index, c.name);
            webcams.put(camcount, w);
            camcount++;
            try
            {
                // Throw out first frame to ignore a possible bad frame
                w.capture_frame();
            }
            catch (Exception e)
            {}
        }
    }

    public synchronized void shutdown()
    {
        for (Webcam_impl w : webcams.values())
        {
            w.shutdown();
        }
    }

    @Override public synchronized Map<Integer, String> get_webcam_names()
    {
        HashMap<Integer, String> o = new HashMap<Integer, String>();
        for (int i : o.keySet())
        {
            o.put(i, webcams.get(i).get_name());
        }
        return o;
    }

    @Override public synchronized Webcam get_webcams(int ind)
    {

        return webcams.get(ind);
    }
}
