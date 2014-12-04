function c_new = removeModel(c, position)
% c_m = size(c,1);
% c_n = size(c,2);

% window_m = c_m/20;
% window_n = c_n/20;

window_m = 40;
window_n = 40;

min_m = max(0, position(2) - window_m/2);
max_m = min(size(c,1), position(2) + window_m/2);
min_n = max(0, position(1) - window_n/2);
max_n = min(size(c,2), position(1) + window_n/2);

c_new = c;
c_new( min_m :  max_m , min_n :  max_n ) = ...
    zeros( max_m - min_m + 1, max_n - min_n + 1 );
