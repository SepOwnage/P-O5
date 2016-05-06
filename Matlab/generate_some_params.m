%A small script to generate parameters for run

%filterbank parameters
%     64
%  32   32
filter_lengths = {64,{{32},{32}}};
%   50
% 50  50
Astops = {50,{{50},{300}}};
%   166
% 16  16
analysis_scalings = {15,{{15},{15}}};
synthesis_scalings = {13,{{15},{15}}};

%adaptive quantization paramaters
mus = int16([.6 .01 0.95 0]*(2^15-1));
phis = int16([.157 .50 0.9 0]*(2^15-1)); 
maxima = 2.^([5, 4, 3, 1]-1)-1;
buffer_lengths = [10 10 10 1];