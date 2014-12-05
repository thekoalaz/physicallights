function [im_ir, im_depth] = kinectframe()

%Create objects
vid1 = videoinput('kinect', 1, 'Infrared_640x480');

vid1.FrameGrabInterval = 1;

vid1.FramesPerTrigger = 1;

%Crucial to save memory
triggerconfig(vid1, 'manual');

%Send data to MATLAB (will not log frames to memory)
start(vid1);
im_ir = getsnapshot(vid1);
stop(vid1);


vid2 = videoinput('kinect', 2, 'Depth_640x480');
src = getselectedsource(vid2);
src.DepthMode = 'Near';

vid2.FrameGrabInterval = 1;
vid2.FramesPerTrigger = 1;
triggerconfig(vid2, 'manual');

start(vid2);
im_depth = getsnapshot(vid2);
stop(vid2);
    
