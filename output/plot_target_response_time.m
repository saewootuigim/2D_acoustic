close all
clear
clc

fid = fopen('u_trg_0.dat','r');
u_trg_0 = fread(fid,'double');
fclose(fid);
% fid = fopen('u_trg_1.dat','r');
% u_trg_1 = fread(fid,'double');
% fclose(fid);
fid = fopen('u_trg_2.dat','r');
u_trg_2 = fread(fid,'double');
fclose(fid);
% fid = fopen('u_trg_3.dat','r');
% u_trg_3 = fread(fid,'double');
% fclose(fid);
% fid = fopen('u_trg_4.dat','r');
% u_trg_4 = fread(fid,'double');
% fclose(fid);
% fid = fopen('u_trg_5.dat','r');
% u_trg_5 = fread(fid,'double');
% fclose(fid);

u_trg_0 = u_trg_0/max(abs(u_trg_0));
% u_trg_1 = u_trg_1/max(abs(u_trg_1));
u_trg_2 = u_trg_2/max(abs(u_trg_2));
% u_trg_3 = u_trg_3/max(abs(u_trg_3));
% u_trg_4 = u_trg_4/max(abs(u_trg_4));
% u_trg_5 = u_trg_5/max(abs(u_trg_5));

subplot(5,1,1);plot(u_trg_0)%;xlim([1,297*2-1])
title('forward')
% subplot(5,1,2);plot(u_trg_1);%xlim([length(u_trg_1)-297*2,length(u_trg_1)])
% title('TR unfiltered')
subplot(5,1,3);plot(u_trg_2);%xlim([length(u_trg_2)-297*2,length(u_trg_2)])
title('TR filtered real part')
% subplot(5,1,4);plot(u_trg_3);%xlim([length(u_trg_3)-297*2,length(u_trg_3)])
% title('TR filtered imag part')
% subplot(6,1,5);plot(u_trg_4);xlim([length(u_trg_4)-100,length(u_trg_4)])
% subplot(5,1,5);plot(u_trg_5);%xlim([length(u_trg_5)-297*2,length(u_trg_5)])
% title('TR filtered modulus')
% xlabel('time step')