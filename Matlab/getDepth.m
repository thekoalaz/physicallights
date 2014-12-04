function depth = getDepth(im_depth, centroids)
depth = zeros(2,1);
while(depth(1) == 0)
    depth(1) = im_depth(floor(centroids(1,2)), floor(centroids(1,1)));
    if(centroids(1,2) ~= size(im_depth,1))
        centroids(1,2) = centroids(1,2) + 1;
    else
        centroids(1,1) = centroids(1,1) + 1;
    end
end

while(depth(2) == 0)
    depth(2) = im_depth(floor(centroids(2,2)), floor(centroids(2,1)));
    if(centroids(2,2) ~= size(im_depth,2))
        centroids(2,2) = centroids(1,2) + 1;
    else
        centroids(2,1) = centroids(2,1) + 1;
    end
end