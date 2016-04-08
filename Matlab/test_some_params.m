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
synthesis_scalings = {14,{{15},{15}}};

%adaptive quantization paramaters
mus = int16([.6 .01 0.95 0]*(2^15-1));
phis = int16([.157 .50 0.9 0]*(2^15-1)); 
maxima = 2.^([5, 4, 3, 1]-1)-1;
buffer_lengths = [10 10 10 1];

pesqs = zeros(8,1);
snrs = zeros(8,1);
inputs = {'8000/words_f.wav',
'8000/words_m.wav',
'8000/f116.wav', 
'8000/f216.wav',
'8000/m116.wav',
'8000/m216.wav',
'8000/belasting.wav',
'8000/bir.wav'};

parfor i =1:8
    [~,~,pesqs(i), snrs(i)] = run(inputs{i}, filter_lengths, Astops, analysis_scalings, synthesis_scalings, mus, phis, maxima, buffer_lengths);
end
averagepesq = mean(pesqs)
averagesrn = mean(snrs)