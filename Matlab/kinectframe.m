function [im_ir, im_depth] = kinectframe()

%Create objects
vid1 = videoinput('kinect', 1, 'Infrared_640x480');
vid1.FrameGrabInterval = 1;
vid1.FramesPerTrigger = 1;
triggerconfig(vid1, 'manual');

start(vid1);
im_ir = getsnapshot(vid1); 
stop(vid1);
clear vid1
pause(1);

vid2 = videoinput('kinect', 2, 'Depth_640x480');
vid2.FrameGrabInterval = 1;
vid2.FramesPerTrigger = 10;
%Crucial to save memory
triggerconfig(vid2, 'manual');

start(vid2);
%Send data to MATLAB (will not log frames to memory)
im_depth = getsnapshot(vid2);

stop(vid2);
clear vid2
figure
imagesc(im_depth)
% calibrate(im_ir, im_depth);
