%an example to show the format of the parameters for run
filter_lengths = {64,{{32,{{16}}},{32}}};
Astops = {60,{{60,{{60}}},{60}}};
mus = [1 1 1 1 1 ]* 0.19;
phis = [.1881 .1881 .3352 .5860 0 ];  %negative make no sense
maxima = 2.^([5, 5, 4, 3, 1]-1)-1;
scalings = {16,{{16,{{16}}},{16}}};
encode_version = 1;
buffer_lengths = [1 1 1 1 1]*8;