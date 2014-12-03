function B = nonMaxSupr(A)

windowSize = round(size(A,1) / 10);
for i = 1 + windowSize : size(A,1) - windowSize
    for j = 1 + windowSize : size(A,2) - windowSize
        if( A(i,j) ~= max(max(A(...
                i - windowSize : i + windowSize, j - windowSize : j + windowSize))) )
            A(i,j) = 0;
        end
    end
end
B = A;