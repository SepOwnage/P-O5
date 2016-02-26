depth = 2;
FLength = 50;
mu = .5;
phi = .0001;



fs = 16e3;
df = fs/10;
Astop = 60;
fstep = 10;
Niter = 600;
[filter, h1, f0, f1] = QMF_design(fs, df, Astop, fstep, Niter, FLength);
close all

input = LoadWav('bir.wav');
%input= [0 0 0 0 0 0 0 0 1 1 0 0 0 0 0 0 0 0 0 0]';
% sinus = sin(2*pi*3000*(0:1/16000:2)');
% input = zeros(length(sinus)*2,1);
% input(1:2:end) = sinus;
% input(2:2:end) = sinus;


%Scale input
input = int16(input/max(abs(input))*(2^15-1));
filter = int16(filter/max(abs(filter))*(2^15-1));

[subbands_links, subbands_rechts] = analysis(input,filter,depth);
%subbands_links(:,1:4) = 0;

subbands_links_codeced = zeros(size(subbands_links));
quantized_data = zeros(size(subbands_links));
for sb = 1:size(subbands_links,2)
   quantized = encode(subbands_links(:,sb), mu, phi);
   quantized_data(:,sb) = quantized;
   dequantized = decode(quantized,mu,phi);
   subbands_links_codeced(:,sb) = (dequantized);
end


reconstructed_links = synthesis(subbands_links,filter);
reconstructed_rechts = synthesis(subbands_rechts,filter);

reconstructed_links_with_codec = synthesis(subbands_links_codeced, filter);
