function depth = getDepth(im_depth, centroids)
depth = zeros(2,1);
while(depth(1) == 0)
    depth(1) = im_depth(round(centroids(1,2)), round(centroids(1,1)));
    centroids(1,2) = centroids(1,2) + 1;
end
% disp(centroids(1,2));

while(depth(2) == 0)
    depth(2) = im_depth(round(centroids(2,2)), round(centroids(2,1)));
    centroids(2,2) = centroids(2,2) + 1;
end
% disp(centroids(2,2));