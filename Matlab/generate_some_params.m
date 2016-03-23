%A small script to generate parameters for run

%filterbank parameters
%     64
%  32   32
filter_lengths = {64,{{32},{32}}};
%   50
% 50  50
Astops = {60,{{50},{50}}};
%   16
% 16  16
scalings = {17,{{15},{16}}};

%adaptive quantization paramaters
mus = int16([1 .1 0.125 0]*(2^15-1));
phis = int16([.25 .5 .9 0]*(2^15-1)); 
maxima = 2.^([5, 4, 3, 1]-1)-1;
buffer_lengths = [10 10 10 1];