clear
close all
clc

a = importdata('model_info.txt');
nNodeX = a.data(1);
nNodeY = a.data(2);
nDOFall = a.data(3);
nDOFsrf = a.data(4);

a = importdata('simulation_info.txt');
nTstep = a.data(1);
T = a.data(2);

fid = fopen('u_srf_filter_imag.dat','r');
u = fread(fid,[nDOFsrf,nTstep],'double')';
fclose(fid);

max = max(max(abs(u)));

pcolor(u);shading flat
colormap jet
caxis([-max,max])