function calibrate()
[im, im_depth] = kinectframe();
% figure, imagesc(im_depth);
%get search space window and write to .mat file
im = im2double(im);
figure, imshow(im);
search_space = ginput(2); close;
search_space = floor(search_space);
save('data\search_space.mat', 'search_space');

centroids = axisDetect(im, search_space);
%append z
% centroids(1,3) = im_depth(round(centroids(1,2)), round(centroids(1,1)));
% centroids(2,3) = im_depth(round(centroids(2,2)), round(centroids(2,1)));
centroids(:,3) = getDepth(im_depth, centroids);

if( abs(centroids(1,1) - centroids(2,1)) < 50 ...
        && abs(centroids(1,2) - centroids(2,2)) < 50)
    disp('Recalibrating');
    updateCoordinates();
else
    writeCoord(centroids);
end

% exit;
