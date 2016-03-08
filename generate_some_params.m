%an example to show the format of the parameters for run
filter_lengths = {64,{{32,{{16}}},{32}}};
Astops = {60,{{60,{{60}}},{60}}};
mus = [.1 .1 0.25 0.25 .1];
phis = [46 50 23 23 32 ];  %negative make no sense
maxima = 2.^([6, 4, 4, 4, 2]-1)-1;
scalings = {16,{{15,{{16}}},{15}}};
depth = 3;
encode_version = 1;
buffer_lengths = [15 24 30 30 3 ];