%an example to show the format of the parameters for run
filter_lengths = cell(2,1);
filter_lengths{1}=[50];
filter_lengths{2} = [50 50];
filter_lengths{3} = [50 50 50 50];
Astops = {[60]; [60 60]; [60 60 60 60]};
mus = [.1 .1 0.25 0.25 .1 .1 0.2 1 ];
phis = [46 50 23 23 32 50 16 1];  %negative make no sense
maxima = 2.^([6, 4, 4, 4, 2, 2, 2, 1]-1)-1;
scalings = cell(2,1);
scalings{1} = 16;
scalings{2} = [15 15];
scalings{3} = [16 16 16 16];
depth = 3;
encode_version = 1;
buffer_lengths = [15 24 30 30 3 3 3 30];