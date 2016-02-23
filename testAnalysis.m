depth = 5;
fs = 16e3;
df = fs/10;
Astop = 60;
fstep = 10;
Niter = 600;
FLength = 100;
[filter, h1, f0, f1] = QMF_design(fs, df, Astop, fstep, Niter, FLength);
close all

input = LoadWav('bir.wav');
%input= [0 0 0 0 0 0 0 0 1 1 0 0 0 0 0 0 0 0 0 0]';
% sinus = sin(2*pi*3000*(0:1/16000:2)');
% input = zeros(length(sinus)*2,1);
% input(1:2:end) = sinus;
% input(2:2:end) = sinus;

ch=chirp((0:1/16e3:5),50,5,8000);
input = zeros(length(ch)*2,1);
input(1:2:end) = ch;
input(2:2:end) = ch;

[subbands_links, subbands_rechts] = analysis(input,filter,depth);
subbands_links(:,1:4) = 0;
reconstructed_links = synthesis(subbands_links,filter);
reconstructed_rechts = synthesis(subbands_rechts,filter);

