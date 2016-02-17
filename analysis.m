function [ output_args ] = analysis()
%ANALYSIS Summary of this function goes here
%   Detailed explanation goes here

fs = 8000;
df = fs/10;
Astop = 60;
fstep = 10;
Niter = 600;
FLength = 25;
[h0, h1, f0, f1] = QMF_design(fs, df, Astop, fstep, Niter, FLength)

end

