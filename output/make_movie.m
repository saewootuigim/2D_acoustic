v = VideoWriter('TR','MPEG-4');
%v.CompressionRatio = 3;
%v.Quality = 80;
open(v)
for i = 0 : 2:nTstep-1
    A = imread(sprintf('frames/step%04i.png',i));
    writeVideo(v,A)
end
close(v)