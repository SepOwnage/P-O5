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
mus = int16([.125 .125 0.0250 .275]*(2^15-1));
phis = int16([.2 .2 .5 .35]*(2^15-1));  %(negative make no sense)
maxima = 2.^([5, 4, 3, 1]-1)-1;
buffer_lengths = [3 3 3 2];