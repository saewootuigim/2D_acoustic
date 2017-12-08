close all
clear
clc

a = importdata('model_info.txt');
nNodeX = a.data(1);
nNodeY = a.data(2);
nDOFall = a.data(3);
nDOFsrf = a.data(4);
h = a.data(5);

a = importdata('simulation_info.txt');
nTstep = a.data(1);
dt = a.data(2);
maxval = a.data(3);
step = a.data(4);

figure
colormap bone
set(gcf,'NextPlot','replacechildren')

tic
for i = 0 : step : nTstep-1
    fid = fopen(sprintf('frames/step%04i.dat',i),'r');
    u = fread(fid,[nNodeX,nNodeY],'double');
    fclose(fid);
    
    pcolor(u')
    shading flat
    caxis([-maxval,maxval]);
    title(sprintf('time step=%4i',i),'interpreter','latex')
    
    axis image
    
    print(sprintf('frames/step%04i',i),'-dpng')
end
toc