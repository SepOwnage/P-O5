function [ output_links, output_rechts, filters ] = analysis(input, ...
    filter_lengths, Astops, scalings)
%ANALYSIS applies an analysis QMF filterbank on the given input.
%   Input must be interleaved left and right channel, of type int16.
%   filter_lengths is one of following structure
%       {filterlength}   or {filterlength, {filter_lengths}}
%       or {filterlength, {filter_lengths, filter_lengths}}
%     In the first case a filter of filterlength is applied, and it's 
%     outputs are not filtered (split) further. In the second case, the
%     lower frequency output band is filtered (split) further and in the 
%     last case both output are filtered (split) further.
%   Astops: stop band attenuations (of same structure as filter_lengths
%   scalings: amounts of bits to shift after convolutions (of same
%   structure as filter_lengths)
%
%   returns:
%     output_links: subbands of the left channel (as arrays in a cell
%     array)
%     output_rechts: subbands of the right channel (as arrays in a cell
%     array)
%     The used filters  (same structure as filter_lengths)


input_links = input(1:2:end);
[ out_low, out_high, filters ] = get_subbands(input_links, filter_lengths, Astops, scalings);
if(size(out_low,1)>size(out_high,1))
    out_high(end+1:end+size(out_low,1)-size(out_high,1),:)...
        = zeros(size(out_low,1)-size(out_high,1), size(out_low,2));
else
    out_low(end+1:end+size(out_high,1)-size(out_low,1),:)...
        = zeros(size(out_high,1)-size(out_low,1), size(out_low,2));
end
output_links = [out_low out_high];

input_rechts = input(2:2:end);
[ out_low, out_high ] = get_subbands(input_rechts, filter_lengths, Astops, depth);
output_rechts = [out_low out_high];

end

