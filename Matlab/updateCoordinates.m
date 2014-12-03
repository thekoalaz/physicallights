function updateCoordinates()

[im, im_depth] = kinectframe();

load('data\search_space.mat');
centroids = axisDetect(im, search_space);
%append z
centroids(1,3) = im_depth(round(centroids(1,2)), round(centroids(1,1)));
centroids(2,3) = im_depth(round(centroids(2,2)), round(centroids(2,1)));

writeCoord(centroids);