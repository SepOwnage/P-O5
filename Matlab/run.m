function [reconstructed_links_with_codec, reconstructed_rechts_with_codec,...
    pesq, snr, pesq_calculated_delay, differentials_clipped] = run(filter_lengths, Astops, scalings,...
                                        mus, phis, maxima, buffer_lengths)
% RUN: runs QMF-filterbank analysis, adaptive stepsize differential
% encoding followed by the corresponding decoding and synthesis.
%  filter_lengths, Astops, scalings: see analysis
%  mus, phis, maxima, buffer_lengths: arrays that contain the mu, phi,
%  maximum and buffer_length value that indicate how each subband should be
%  encode by encode.  See encode.
%  returns:
%    -reconstructed_links_with_codec: the reconstructed left channel after
%        analysis, encoding, decoding and synthesing
%    -reconstructed_rechts_with_codec: the reconstructed right channel after
%        analysis, encoding, decoding and synthesing
%    -pesq: calculated pesq score. Based on left channel only.
%    -differentials_clipped: amount of encoded differences that had to be
%        clipped. For analysis purposes only.
    input = LoadWav('8000/combined_8000_short.wav');

    %Scale input
    input = int16(input/max(abs(input))*(2^15-1));
    
    %% analysis
    [subbands_links, subbands_rechts, filters] = analysis(input,filter_lengths, Astops, scalings);

    %% compression (adaptive quantization)
    % left channel
    subbands_links_codeced = cell(size(subbands_links));
    quantized_data = cell(size(subbands_links));
    differentials_clipped = zeros(1,size(subbands_links,2));
    for sb = 1:length(subbands_links)
       [quantized, nb_clips] = encode(subbands_links{sb},...
           mus(sb), phis(sb), maxima(sb), buffer_lengths(sb));
       differentials_clipped(sb) = nb_clips;
       quantized_data{sb} = quantized;
       dequantized = decode(quantized,mus(sb),phis(sb),buffer_lengths(sb));
       subbands_links_codeced{sb} = (dequantized);
    end
    %right channel
    subbands_rechts_codeced = cell(size(subbands_rechts));
    quantized_data = cell(size(subbands_rechts));
    differentials_clipped = zeros(1,size(subbands_rechts,2));
    for sb = 1:length(subbands_rechts)
       [quantized, nb_clips] = encode(subbands_rechts{sb},...
           mus(sb), phis(sb), maxima(sb), buffer_lengths(sb));
       differentials_clipped(sb) = nb_clips;
       quantized_data{sb} = quantized;
       dequantized = decode(quantized,mus(sb),phis(sb),buffer_lengths(sb));
       subbands_rechts_codeced{sb} = (dequantized);
    end
    %% synthesis
    reconstructed_links_with_codec = synthesis(subbands_links_codeced, filters, scalings,0);
    reconstructed_rechts_with_codec = synthesis(subbands_rechts_codeced, filters, scalings,0);
    %% pesq score
    pesq = alignpesq(input(1:2:end),reconstructed_links_with_codec);
    %only works for symmetrical depth 2 
    delay = ((filter_lengths{1}-1)+(filter_lengths{2}{1}{1}-1)*2);
    pesq_calculated_delay = alignpesq(input(1:2:end),reconstructed_links_with_codec(1+delay:end-delay));
    snr = snrseg(double(reconstructed_links_with_codec(1+delay:end-delay)), double(input(1:2:end)), 8e3);
    close all;