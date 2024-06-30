function simple_webcam_client_memory()

%Simple example Robot Raconteur webcam client

%Connect to the service
c1=RobotRaconteur.ConnectService('rr+tcp://localhost:22355?service=webcam');

%Capture the frame to the buffers
frame_size=c1.capture_frame_to_buffer();

%Pull out the buffer memory and show the image
buffer=c1.buffer;
size(buffer)
%Note: buffer(:) pulls the entire array
frame=bufWebcamImageToIM(buffer(:),frame_size);
figure
imshow(frame)


%Pull out the multidimbuffer memory
multidimbuffer=c1.multidimbuffer;

%Print out the size of the buffer
size(multidimbuffer)

%Pull out a segment
frame_segment=multidimbuffer(1:100,1:100,1);
figure
imshow(frame_segment)

%Disconnect from the service
RobotRaconteur.DisconnectService(c1);

    %Helper function to convert raw buffer data to "MATLAB" format
    function im=bufWebcamImageToIM(data,wim)
        b=reshape(data(1:3:end),wim.width,wim.height)';
        g=reshape(data(2:3:end),wim.width,wim.height)';
        r=reshape(data(2:3:end),wim.width,wim.height)';

        im=cat(3,r,g,b);
    end

end
