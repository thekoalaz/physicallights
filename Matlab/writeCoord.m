function writeCoord(coord)
%write coordinates to a text file. Values parsed by '\n'
dlmwrite('.\data\calibrate.txt', coord(:), 'delimiter', '\n', 'precision', 3);
fileID = fopen('.\data\calibrate.txt', 'w');
fprintf(fileID, '%f\n', coord');
fclose(fileID);