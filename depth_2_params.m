%an example to show the format of the parameters for run
filter_lengths = cell(2,1);
filter_lengths{1}=[50];
filter_lengths{2} = [50 50];
%filter_lengths{3} = [50 50 50 50];
Astops = {[60]; [60 60]};
mus = [0.25 0.30 0.25 1 ];
phis = [36 39 36 1];  %negative make no sense
maxima = 2.^([6, 4, 2, 1]-1)-1;
scalings = cell(2,1);
scalings{1} = 16;
scalings{2} = [15 15];
%scalings{3} = [16 16 16 16];
depth = 2;
encode_version = 1;
buffer_lengths = [35 43 40 30];