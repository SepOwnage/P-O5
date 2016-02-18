clear all

depth = 1;
fs = 4000;
df = fs/10;
Astop = 60;
fstep = 10;
Niter = 600;
FLength = 100;
[filter, h1, f0, f1] = QMF_design(fs, df, Astop, fstep, Niter, FLength);
close all

input = LoadWav('bir.wav');
[subbands_links, subbands_rechts] = analysis(input,filter,depth);
%subbands_links(:,1) = 0;

reconstructed_links = synthesis(subbands_links,filter);
reconstructed_rechts = synthesis(subbands_rechts,filter);

