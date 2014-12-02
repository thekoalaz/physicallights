function optm = optimalAxis(im, template)
%For now assuming 2 detected objects
proportion = zeros(20,2);

i = 1;
for angle = 0 : 5 : 90
    mask = imrotate(template, angle);
    c = conv2(double(im), mask);
    peaks = c > (max(max(c))/3);
    stats = regionprops(peaks, 'MajorAxisLength', 'MinorAxisLength');
    proportion(i,1) = angle;
    proportion(i,2) = (stats(1).MajorAxisLength/stats(1).MinorAxisLength + ...
        stats(2).MajorAxisLength/stats(2).MinorAxisLength) / 2;
    i = i + 1;
end

temp = max(proportion(:,2));
[idx_m, idx_n] = find(proportion == temp);
optm = proportion(idx_m, 1);

%deal with multiple possibilities
if(size(optm,1) > 1)
    if(proportion(idx_m(1) - 1, 2) > proportion(idx_m(end) + 1, 2))
        optm = proportion(idx_m(1), 1);
    else
        optm = proportion(idx_m(end), 1);
    end
end