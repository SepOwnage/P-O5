function [reconstructed_links_with_codec, pesq, bitrate, differentials_clipped] = ...
    run(depth, filter_lengths, Astops, scalings,...
    mus, phis, maxima, encode_version, buffer_lengths)

    input = LoadWav('combined_8000_short.wav');

    %Scale input
    input = int16(input/max(abs(input))*(2^15-1));
    
    %% analysis
    [subbands_links, ~, filters] = analysis(input,filter_lengths, Astops, scalings, depth);
    %subbands_links(:,1:4) = 0;

    %% compression (adaptive quantization)
    subbands_links_codeced = cell(size(subbands_links));
    quantized_data = cell(size(subbands_links));
    differentials_clipped = zeros(1,size(subbands_links,2));
    for sb = 1:length(subbands_links)
       [quantized, nb_clips] = encode(subbands_links{sb},...
           mus(sb), phis(sb), maxima(sb), encode_version, buffer_lengths(sb));
       differentials_clipped(sb) = nb_clips;
       quantized_data{sb} = quantized;
       dequantized = decode(quantized,mus(sb),phis(sb),buffer_lengths(sb));
       subbands_links_codeced{sb} = (dequantized);
    end

%    bitrate = sum(ceil(log2(double(max(abs(quantized_data))+1)))+1)*8000/2^depth; %8000/2^depth: samplerate after filterbank;   +1:+ and -
    bitrate = log2(maxima+1);
    bitrate(bitrate>0) = bitrate(bitrate>0)+1;
    bitrate=sum(bitrate);

    %% synthesis
%    reconstructed_links = synthesis(subbands_links,filters);
%    reconstructed_rechts = synthesis(subbands_rechts,filter);

    reconstructed_links_with_codec = synthesis(subbands_links_codeced, filters, scalings,0);
    pesq = alignpesq(input(1:2:end),reconstructed_links_with_codec);
