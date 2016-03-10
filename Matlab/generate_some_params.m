%an example to show the format of the parameters for run
filter_lengths = {60,{{30,{{16}}},{30}}};
Astops = {60,{{60,{{60}}},{60}}};
mus = [.125 .125 0.0250 .275 1 ];
phis = [.2 .2 .5 .35 0 ];  %negative make no sense
maxima = 2.^([5, 4, 3, 0]-1)-1;
scalings = {16,{{16,{{16}}},{16}}};
encode_version = 1;
buffer_lengths = [7 7 9 2 1];