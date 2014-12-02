function calibrate(im, im_depth)
figure, imshow(im);

search_space = ginput(2); close;
search_space = floor(search_space);

centroids = axisDetect(im, search_space);
centroids(1,3) = im_depth(round(centroids(1,2)), round(centroids(1,1)));
centroids(2,3) = im_depth(round(centroids(2,2)), round(centroids(2,1)));

dlmwrite('.\data\calibrate.txt', centroids(:), 'delimiter', '\n', 'precision', 3);
fileID = fopen('.\data\calibrate.txt', 'w');
fprintf(fileID, '%f\n', centroids');
fclose(fileID);