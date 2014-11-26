% matlab -r matlab_filename_here -logfile c:\temp\logfile
function hello()
hello_str = 'Hello World';
fileID = fopen('hello.txt','w');
fprintf(fileID,'%s\n', hello_str);
fclose(fileID);
exit;
