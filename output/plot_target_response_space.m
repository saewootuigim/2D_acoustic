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

fid = fopen('u_res_x_tr.dat','r');
u_res_x_uf = fread(fid,[nNodeX-2,nTstep],'double');
fclose(fid);
fid = fopen('u_res_y_tr.dat','r');
u_res_y_uf = fread(fid,[nNodeY-1,nTstep],'double');
fclose(fid);
fid = fopen('u_res_x_trfr.dat','r');
u_res_x = fread(fid,[nNodeX-2,nTstep],'double');
fclose(fid);
fid = fopen('u_res_y_trfr.dat','r');
u_res_y = fread(fid,[nNodeY-1,nTstep],'double');
fclose(fid);

[ max_val_u_res_x_uf, max_loc_u_res_x_uf ] = max(max(abs(u_res_x_uf)));
[ max_val_u_res_y_uf, max_loc_u_res_y_uf ] = max(max(abs(u_res_x_uf)));
[ max_val_u_res_x, max_loc_u_res_x ] = max(max(abs(u_res_x)));
[ max_val_u_res_y, max_loc_u_res_y ] = max(max(abs(u_res_y)));

u_res_x_uf = u_res_x_uf/max_val_u_res_x_uf;
u_res_y_uf = u_res_y_uf/max_val_u_res_y_uf;
u_res_x = u_res_x/max_val_u_res_x;
u_res_y = u_res_y/max_val_u_res_y;

x = linspace(h,(nNodeX-1)*h,nNodeX-2);
y = linspace(h,nNodeY*h,nNodeY-1);

figure('Position',[58.6000  272.2000  660.0000  420.0000])

for i1 = 1 : nTstep
    clf
    
    subplot(1,2,1)
    hold on
    plot(x,u_res_x_uf(:,i1),'k-')
    plot(x,u_res_x(:,i1),'k--')
    plot([9,9],[-1,1])
    ylim([-1.1,1.1])
    xlim([min(x),max(x)])
    
    subplot(1,2,2)
    hold on
    plot(y,u_res_y_uf(:,i1),'k-')
    plot(y,u_res_y(:,i1),'k--')
    plot([5.4,5.4],[-1,1])
    ylim([-1.1,1.1])
    xlim([min(y),max(y)])
    
    title(sprintf('time step %4i',i1))
    
    print(sprintf('frames/step%04i',i1),'-dpng')
end