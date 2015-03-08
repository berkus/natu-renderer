%clear all;
close all;
clc;

%image = imread('Noise_53.bmp');
%size = size(image);
%image_r = zeros(size(1), size(2));
%image_g = zeros(size(1), size(2));
%image_b = zeros(size(1), size(2));
%for x=1:size(1)
%    for y=1:size(2)
%        image_r(x,y) = double(image(x,y,1))/255.0;
%        image_g(x,y) = double(image(x,y,2))/255.0;
%        image_b(x,y) = double(image(x,y,3))/255.0;
%    end
%end

xaxis = 1:size(1);
yaxis = 1:size(2);

origin = [0, 0];
dir = [2.66 1];%rand(1,2);
dir = dir/norm(dir);
step = 0.5;
samples = 1000;
cut = zeros(samples,3);
xvals = zeros(samples, 1);
for s=1:samples
    xvals(s,1) = (s*step).*dir(1);
    position = origin + (s*step).*dir;
    cut(s, 1) = interp2(xaxis, yaxis, image_r, position(1), position(2));
    cut(s, 2) = interp2(xaxis, yaxis, image_g, position(1), position(2));
    %cut(s, 3) = interp2(xaxis, yaxis, image_b, position(1), position(2));
    
end


subplot(1,2,2)
hold on
xlabel('vzdálenost od poèátku ~ èas');
ylabel('f(x)');
plot (xvals(:), cut(:, 1), 'r');
plot (xvals(:), cut(:, 2), 'g');
%plot ((1:samples), cut(:, 3), 'b');
subplot(1,2,1)
hold on
imagesc(image);
axis image;
x = [0 xvals(samples,1)];
c = - dir(2)*origin(1) + dir(1)*origin(2);
y = (dir(2)*x-c)/dir(1);
plot (x,y, 'white');
hold off;




