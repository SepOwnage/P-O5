%an example to show the format of the parameters for run
filter_lengths = cell(3,1);
filter_lengths{1}=[56];
filter_lengths{2} = [58 60];
filter_lengths{3} = [40 42 44 46];
Astops = {[60]; [60 60]; [60 60 60 60]};
mus = [.8 .8 .8 .8 .8 .8 .8 .8];
phis = [.8 .8 .8 .8 .8 .8 .8 .8];;
scalings = cell(3,1);
scalings{1} = 16;
scalings{2} = [16 16];
scalings{3} = [16 16 16 16];