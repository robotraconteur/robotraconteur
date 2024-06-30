function simple_webcam_client_stream()

%Simple example Robot Raconteur webcam client

%Connect to the service
c1=RobotRaconteur.ConnectService('rr+tcp://localhost:22355?service=webcam');

%Connect to the stream pipe
p=c1.frame_stream.Connect(-1);

%Start the streaming, ignore if there is an error
try
   c1.start_streaming();
catch
end

%Loop through and show 20 seconds of the live feed
%Note: the framerate may be very low for this example...
figure

for i=1:100
   %If there is a packet available, receive and show
   while (p.Available > 0)
       im=WebcamImageToIM(p.ReceivePacket());
       clf
       imshow(im)
   end
   pause(.2)
end

%You can also "SendPacket" on a pipe, but we do not in this example

%Close the pipe
p.Close();

%Stop the camera streaming
c1.stop_streaming();

%Disconnect from the service
RobotRaconteur.DisconnectService(c1);

    %Helper function to convert raw images to "MATLAB" format
    function im=WebcamImageToIM(wim)
        b=reshape(wim.data(1:3:end),wim.width,wim.height)';
        g=reshape(wim.data(2:3:end),wim.width,wim.height)';
        r=reshape(wim.data(3:3:end),wim.width,wim.height)';

        im=cat(3,r,g,b);
    end

end
