%A small script to generate parameters for run

%filterbank parameters
%     64
%  32   32
filter_lengths = {64,{{32},{32}}};
%   50
% 50  50
Astops = {50,{{50},{50}}};
%   16
% 16  16
scalings = {16,{{16},{16}}};

%adaptive quantization paramaters
mus = [.125 .125 0.0250 .275];
phis = [.2 .2 .5 .35];  %(negative make no sense)
maxima = 2.^([5, 4, 3, 0]-1)-1;
buffer_lengths = [7 7 9 2];