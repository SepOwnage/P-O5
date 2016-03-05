function [reconstructed_links_with_codec] = run(depth, filter_lengths, Astops, scalings, mus, phis)

    input = LoadWav('combined_8000.wav');

    %Scale input
    input = int16(input/max(abs(input))*(2^15-1));
    
    %% analysis
    [subbands_links, ~, filters] = analysis(input,filter_lengths, Astops, scalings, depth);
    %subbands_links(:,1:4) = 0;

    %% compression (adaptive quantization)
    subbands_links_codeced = zeros(size(subbands_links));
    quantized_data = zeros(size(subbands_links));
    for sb = 1:size(subbands_links,2)
       quantized = encode(subbands_links(:,sb), mus(sb), phis(sb));
       quantized_data(:,sb) = quantized;
       dequantized = decode(quantized,mus(sb),phis(sb));
       subbands_links_codeced(:,sb) = (dequantized);
    end

    %% synthesis
%    reconstructed_links = synthesis(subbands_links,filters);
%    reconstructed_rechts = synthesis(subbands_rechts,filter);

    reconstructed_links_with_codec = synthesis(subbands_links_codeced, filters, scalings);
