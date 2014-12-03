function centroids = axisDetect(I_ir, ss)

%template as a cone
template = zeros(14,6);
template(3:12, 3:4) = ones(10,2);

%reduce search space to window assumption
I = I_ir(ss(1,2) : ss(2,2), ss(1,1) : ss(2,1));

%get rid of noise (S&P)
I = medfilt2(I);

% optm_angle = optimalAxis(I, template);
% disp(optm_angle);
optm_angle = 0;
template = imrotate(template, optm_angle);
c = conv2(double(I), template);
c = removeModel(c);
c = nonMaxSupr(c);
figure, imagesc(c);

peaks = c > (max(max(c))/5); %threshold;
% peaks = bwmorph(peaks, 'skel', 10);
% peaks = bwmorph(peaks, 'spur');
% [x,y] = find(peaks == 1);
% 
% %adjust to conv size and cropped window
% x = x - size(template,1)/2 + ss(1,2);
% y = y - size(template,2)/2 + ss(1,1);
% 
% %find separate objects
% deriv_filt = [-1; 1];
% dx = conv(x, deriv_filt);
% % dy = conv(y, deriv_filt);
% temp = abs(dx(2:end-1)) > 5;
% x1 = x(1 : find(temp));
% x2 = x(find(temp) + 1 : end);
% y1 = y(1 : find(temp));
% y2 = y(find(temp) + 1 : end);
% 
% %keep center of objects
% x1 = x1( ceil(size(x1,1)/2) );
% x2 = x2( ceil(size(x2,1)/2) );
% y1 = y1( ceil(size(y1,1)/2) );
% y2 = y2( ceil(size(y2,1)/2) );
% x = [x1, x2];
% y = [y1, y2];

stats = regionprops(peaks, 'Centroid', 'Extrema');
c1 = stats(1).Centroid;
c2 = stats(2).Centroid;
centroids = [c1; c2];
centroids(:,1) = centroids(:,1) + ss(1,1) - size(template,2)/2;
centroids(:,2) = centroids(:,2) + ss(1,2) - size(template,1)/2;
% stats(1).Extrema(1,:)

figure, subplot(1,2,1), imshow(I_ir);
hold on, plot(centroids(1,1), centroids(1,2), 'x', 'MarkerSize', 10);
plot(centroids(2,1), centroids(2,2), 'x', 'MarkerSize', 10);
hold off, subplot(1,2,2), imshow(peaks);
pause(.005);