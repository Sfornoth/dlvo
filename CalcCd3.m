clear
clc
prefix_name = {'/home/pzhang/chen/permeable_bed/'};
middle_name = {'test_ibm_'};
pos = [49 49 49];
R = 5;
tau = 0.53;
dx = 1;
dt = 1;

num = 1:3;
name = strcat(prefix_name,middle_name,num2str(num(1),'%04d'),'.h5');
[data,domain] = getData(char(name),char('/Flbm_0'),2,true);
Nx = domain.Nx;
Ny = domain.Ny;
Nz = domain.Nz;
[x,y,z] = meshgrid(0:Nx-1, 0:Ny-1,0:Nz-1);
xx = x-pos(1);
yy = y-pos(2);
zz = z-pos(3);
kkk = find((xx.^2+yy.^2+zz.^2)>(1.0*R^2));
%plot(x(kkk),y(kkk),'*')
for i = 1:numel(num)
    name = strcat(prefix_name,middle_name,num2str(num(i),'%04d'),'.h5');
    [data,domain] = getData(char(name),char('/Flbm_0'),2,true);
    
    Flbmx = reshape(data(:,:,:,1),[Nx,Ny,Nz]);
    Flbmy = reshape(data(:,:,:,2),[Nx,Ny,Nz]);
    Flbmz = reshape(data(:,:,:,3),[Nx,Ny,Nz]);
    Flbmyy(i,1) = sum(Flbmy(:));
    Flbmxx(i,1) = sum(Flbmx(:));
    Flbmzz(i,1) = sum(Flbmz(:));
    [data,domain] = getData(char(name),char('/Velocity_0'),2,true);
    vx = reshape(data(:,:,:,1),[Nx,Ny,Nz]);
    vy = reshape(data(:,:,:,2),[Nx,Ny,Nz]);
    vz = reshape(data(:,:,:,3),[Nx,Ny,Nz]);
    v = (vx.^2 + vy.^2+vz.^2).^0.5;
    U(i,1) = mean(v(kkk));
    [data,domain] = getData(char(name),char('/Density_0'),2,false);
    rho = reshape(data,[Nx,Ny,Nz]);
    Rho(i,1) = mean(rho(kkk));
end
nu = (tau-0.5)/3;
Re = 2*R.*U./nu;
Cd = -2.0.*Flbmxx./(Rho.*U.^2.*pi*R^2);
loglog(Re,Cd,'*')
hold on
fplot(@(x) 24./x+6./(1+x.^0.5)+0.4,[min(Re) max(Re)],'g')
xxx = min(Re):0.001:max(Re);
yyy = 24/9.06^2.*(9.06./xxx.^0.5+1).^2;
loglog(xxx,yyy,'r')