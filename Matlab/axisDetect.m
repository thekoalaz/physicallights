function centroids = axisDetect(I_ir, ss)

%template as a cone
template = zeros(30,30);
template(12:20, 14:18) = ones(9,5);
templateS = imresize(template, .75);
templateL = imresize(template, 1.5);

%reduce search space to window assumption
I = I_ir(ss(1,2) : ss(2,2), ss(1,1) : ss(2,1));

%get rid of noise (S&P)
I = medfilt2(I);

% optm_angle = optimalAxis(I, template);
% disp(optm_angle);
% optm_angle = 0;
% template = imrotate(template, optm_angle);
cL = conv2(double(I), templateL);
% c = removeModel(c);
% c = nonMaxSupr(c);

% peaks = c > (max(max(c))/5); %threshold;
peak1 = cL == (max(max(cL))); %axis1
stats = regionprops(peak1, 'Centroid', 'Extrema');
pos1 = stats.Centroid;

cS = conv2(double(I), templateS);
cS = removeModel(cS, pos1);
peak2 = cS == (max(max(cS))); %axis2
stats = regionprops(peak2, 'Centroid', 'Extrema');
pos2 = stats.Centroid;

% stats = regionprops(peaks, 'Centroid', 'Extrema');
% c1 = stats(1).Centroid;
% c2 = stats(2).Centroid;

centroids = [pos1; pos2];
%pad it to have original image positions
centroids(:,1) = centroids(:,1) + ss(1,1) - size(template,2)/2;
centroids(:,2) = centroids(:,2) + ss(1,2) - size(template,1)/2;

figure, subplot(1,2,1), imshow(I_ir);
hold on, plot(centroids(1,1), centroids(1,2), 'x', 'MarkerSize', 10);
plot(centroids(2,1), centroids(2,2), 'x', 'MarkerSize', 10);
hold off, subplot(1,2,2), imshow(peaks);
pause(.005);