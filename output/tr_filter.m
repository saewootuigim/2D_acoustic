close all
clear
clc

a = importdata('model_info.txt');
nNodeX = a.data(1);
nNodeY = a.data(2);
nDOFall = a.data(3);
nDOFsrf = a.data(4);
dx = a.data(5);

a = importdata('simulation_info.txt');
nTstep = a.data(1);
dt = a.data(2);
maxval = a.data(3);
step = a.data(4);

fid = fopen('u_srf.dat','r');
u_srf = fread(fid,[nDOFsrf,nTstep],'double');
fclose(fid);

u_fft = fftshift(fft2(u_srf));

w = linspace(-pi/dt,pi/dt,nTstep);
k = linspace(-pi/dx,pi/dx,nDOFsrf);

for i1 = 1 : length(w)
    for i2 = 1 : length(k)
        ialpha = 1i*sqrt(w(i1)^2-k(i2)^2);
        u_fft(i2,i1) = -ialpha*u_fft(i2,i1);
    end
end

u_ifft = ifft2(ifftshift(u_fft));

max_u_ifft = max(max(abs(u_ifft)));

j = 15;

subplot(2,2,1); pcolor(u_srf); shading flat
subplot(2,2,2); pcolor(abs(u_fft)); shading flat
subplot(2,2,3); surf(imag(u_ifft(j+1:end-j,j+1:end-j))); shading flat;% caxis([-max_u_ifft,max_u_ifft]*.0001)
subplot(2,2,4); pcolor(imag(u_ifft(j+1:end-j,j+1:end-j))); shading flat

temp_store = imag(u_ifft(j+1:end-j,j+1:end-j));
u_ifft = zeros(size(u_ifft));
u_ifft(j+1:end-j,j+1:end-j) = temp_store;

subplot(2,2,3); pcolor(u_ifft); shading flat;

fid = fopen('u_srf_filter.dat','w');
fwrite(fid,u_ifft,'double');
fclose(fid);