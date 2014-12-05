function updateCoordinates()

[im, im_depth] = kinectframe();

load('data\search_space.mat');
centroids = axisDetect(im, search_space);
%append z
% centroids(1,3) = im_depth(round(centroids(1,2)), round(centroids(1,1)));
% centroids(2,3) = im_depth(round(centroids(2,2)), round(centroids(2,1)));
centroids(:,3) = getDepth(im_depth, centroids)
% figure, imagesc(im_depth);

if( abs(centroids(1,1) - centroids(2,1)) < 50 ...
        && abs(centroids(1,2) - centroids(2,2)) < 50)
    disp('Recalibrating');
    updateCoordinates();
else
    writeCoord(centroids);
end

exit;
