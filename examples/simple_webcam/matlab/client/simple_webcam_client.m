function simple_webcam_client()

%Simple example Robot Raconteur webcam client

%Connect to the service
c_host=RobotRaconteur.ConnectService('rr+tcp://localhost:22355?service=multiwebcam');

%Use objref's to pull out the cameras. c_host is a "WebcamHost" type
%and is used to find the webcams
c1=c_host.get_webcams(0);
c2=c_host.get_webcams(1);

%Pull a frame from each camera, c1 and c2
frame1=WebcamImageToIM(c1.capture_frame());
frame2=WebcamImageToIM(c2.capture_frame());

%Show the images
figure
imshow(frame1);
title(c1.name)
figure
imshow(frame2);
title(c2.name)

RobotRaconteur.DisconnectService(c_host)

    %Helper function to convert raw images to "MATLAB" format
    function im=WebcamImageToIM(wim)
        b=reshape(wim.data(1:3:end),wim.width,wim.height)';
        g=reshape(wim.data(2:3:end),wim.width,wim.height)';
        r=reshape(wim.data(3:3:end),wim.width,wim.height)';

        im=cat(3,r,g,b);


    end

end
