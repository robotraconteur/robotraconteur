function camera_client_capture_frame(url)

% Simple example Robot Raconteur standard camera client
% This program will capture a single frame and
% display it.

if nargin == 0
    url = 'rr+tcp://localhost:59823?service=camera';
end

% Connect to the camera
c=RobotRaconteur.ConnectService(url);

%Capture a frame
rr_image=c.capture_frame();

%Convert the frame to an image
img=standard_image_to_mat(rr_image);

%Show the images
figure
imshow(img);

RobotRaconteur.DisconnectService(c)
