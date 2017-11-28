close all
clear
clc

fid = fopen('u_trg.dat','r');
u_trg = fread(fid,'double');
fid = fopen('sgnl.dat','r');
sgnl = fread(fid,'double');
subplot(2,1,1);plot(u_trg)
subplot(2,1,2);plot(sgnl)