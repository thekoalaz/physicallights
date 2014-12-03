function c_new = removeModel(c)
c_m = size(c,1);
c_n = size(c,2);

center_m = round(c_m / 2);
center_n = round(c_n / 2);

c_new = c;
c_new( center_m - c_m/40 :  center_m + c_m/40 , ...
    center_n - c_n/40 :  center_n + c_n/40 ) = ...
    zeros( c_m/20 + 1, c_n/20 + 1);
