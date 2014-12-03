function [im_ir, im_depth] = kinectframe()

%Create objects
vid1 = videoinput('kinect', 1, 'IR_640x480');
vid2 = videoinput('kinect', 2, 'Depth_640x480');

vid1.FrameGrabInterval = 1;
vid2.FrameGrabInterval = 1;

vid1.FramesPerTrigger = 1;
vid2.FramesPerTrigger = 1;

%Crucial to save memory
triggerconfig(vid1, 'manual');
triggerconfig(vid2, 'manual');

%Send data to MATLAB (will not log frames to memory)
start([vid1, vid2]);

im_ir = getsnapshot(vid1); 
im_depth = getsnapshot(vid2);
% calibrate(im_ir, im_depth);