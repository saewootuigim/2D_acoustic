clear
close all
clc

%% Inputs
w = 2*pi/0.4;

% focal point
p = 1;

% grid in depth direction
dx = (2*pi/w)*0.1;
x=0+dx:dx:2*p;
nx=size(x,2);

% grid on surface (2D polar)
dh = dx*5;
L = 6;
h = -L*p:dh:L*p;
nh=size(h,2);


df = 1/(dh*nh);
f=-nh/2*df:df:nh/2*df-df;

%% memory allocation
va=zeros(nx,1 );
va(:)=complex(0,0);
nva=zeros(nx,1 );
nva(:)=complex(0,0);

data=zeros(nh,nh);
data(:,:)=complex(0,0);
fdata=zeros(nh,nh);
fdata(:,:)=complex(0,0);

%% data generation
R2=L*L*p*p;
for ii = 1:nh
    a=h(ii);
    for jj = 1:nh
        b=h(jj);
        r2=a*a+b*b;
        if r2<R2
            data(ii,jj) = -exp(1i*w*sqrt(p*p+r2))/(4*pi*sqrt(p*p+r2));
        end
    end
end

% conjugate
data(:)=data(:)';

%% Filtering
fdata=fft2(data);
fdata=fftshift(fdata);

for ii = 1:nh
    a=f(ii);
    for jj = 1:nh
        b=f(jj);
        k2=a*a+b*b;
        ialpha=1i*sqrt(w*w-k2*2*pi*2*pi);
        fdata(ii,jj)=-ialpha*fdata(ii,jj);
    end
end

ndata=ifft2(ifftshift(fdata));
%ndata=ifft2(fdata);

%% boundary integration
for ii = 1:nx
    for jj = 1:nh
        a=h(jj);
        for kk = 1:nh
            b=h(kk);
            tmp=sqrt(x(ii)*x(ii)+a*a+b*b);
            ker=-exp(1i*w*tmp)/(4*pi*tmp);
            va(ii) = va(ii) + (-2)*ker*data(jj,kk)*dh*dh;
            nva(ii) = nva(ii) + (-2)*ker*ndata(jj,kk)*dh*dh;
        end
    end
end

%% normalization
mv=max(abs(va));
nmv=max(abs(nva));

for ii = 1:nx
    va(ii)=va(ii)/mv;
    nva(ii)=nva(ii)/nmv;data(:)=data(:)';
end

%% plot
subplot(1,2,1);plot(x,abs(va))
title('original','interpreter','latex')
xlabel('$x_0$','interpreter','latex')
xlim([min(x),max(x)])

subplot(1,2,2);plot(x,abs(nva))
title('filtered','interpreter','latex')
xlabel('$x_0$','interpreter','latex')
xlim([min(x),max(x)])

%% save
fileID = fopen('ref.data','w');
for ii = 1:nx
    fprintf(fileID,'%20.15e\t%20.15e\t%20.15e\t%20.15e\n',x(ii),abs(va(ii)),real(va(ii)),imag(va(ii)));
end
fclose(fileID);

fileID = fopen('filt.data','w');
for ii = 1:nx
    fprintf(fileID,'%20.15e\t%20.15e\t%20.15e\t%20.15e\n',x(ii),abs(nva(ii)),real(nva(ii)),imag(nva(ii)));
end
fclose(fileID);
