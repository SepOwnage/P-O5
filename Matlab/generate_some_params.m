%A small script to generate parameters for run
filter_lengths = {64,{{32},{32}}};
Astops = {50,{{50},{50}}};
mus = [.125 .125 0.0250 .275 1 ];
phis = [.2 .2 .5 .35 0 ];  %negative make no sense
maxima = 2.^([5, 4, 3, 0]-1)-1;
scalings = {16,{{16,{{16}}},{16}}};
buffer_lengths = [7 7 9 2 1];