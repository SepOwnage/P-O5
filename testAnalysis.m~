fs = 4000;
df = fs/10;
Astop = 60;
fstep = 10;
Niter = 600;
FLength = 25;
[h0, h1, f0, f1] = QMF_design(fs, df, Astop, fstep, Niter, FLength);
close all

h0_e0 = h0(1:2:end);
h0_e1 = h0(2:2:end);

input = LoadWav('bir.wav');
ilinks = input(1:2:end);
ilinks_e0 = ilinks(1:2:end);
ilinks_e1 = ilinks(2:2:end);



irechts = input(2:2:end);
irechts_e0 = irechts(1:2:end);
irechts_e1 = irechts(2:2:end);

output_e0 = h0_e0

soundsc(ilinks, 16000)
