function c_new = removeModel(c)
c_m = size(c,1);
c_n = size(c,2);

center_m = round(c_m / 2);
center_n = round(c_n / 2);

window_m = round(c_m/40);
window_n = round(c_n/40);

c_new = c;
c_new( center_m - window_m :  center_m + window_m , ...
    center_n - window_n :  center_n + window_n ) = ...
    zeros( window_m*2 + 1, window_n*2 + 1);
