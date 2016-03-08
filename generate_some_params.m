%an example to show the format of the parameters for run
filter_lengths = {64,{{32,{{16}}},{32}}};
Astops = {60,{{60,{{60}}},{60}}};
mus = [.125 .125 0.025 .275 1 ];
phis = [.2 .2 .5 .55 0 ];  %negative make no sense
maxima = 2.^([5, 5, 4, 3, 1]-1)-1;
scalings = {16,{{16,{{16}}},{16}}};
encode_version = 1;
buffer_lengths = [7 7 9 2 1];