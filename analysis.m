function [ output_links, output_rechts ] = analysis(input)
%ANALYSIS Summary of this function goes here
%   Detailed explanation goes here

fs = 4000;
df = fs/10;
Astop = 60;
fstep = 10;
Niter = 600;
FLength = 24;
[filter, h1, f0, f1] = QMF_design(fs, df, Astop, fstep, Niter, FLength);
close all
depth = 2;
input_links = input(1:2:end);
[ out_low, out_high ] = get_subbands(input_links,filter, depth);
output_links = [out_low out_high];

input_rechts = input(2:2:end);
[ out_low, out_high ] = get_subbands(input_rechts,filter, depth);
output_rechts = [out_low out_high];

end

