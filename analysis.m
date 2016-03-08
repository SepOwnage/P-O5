function [ output_links, output_rechts, filters ] = analysis(input, ...
    filter_lengths, Astops, scalings)
%ANALYSIS Summary of this function goes here
%   Detailed explanation goes here


% 
% if mod(length(input),2^(depth+1))
%     amount_to_add = 2^(depth+1) - mod(length(input),2^(depth+1));
%     input = [input; zeros(amount_to_add,1)*2];
% end

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

% input_rechts = input(2:2:end);
% [ out_low, out_high ] = get_subbands(input_rechts, filter_lengths, Astops, depth);
 output_rechts = [out_low out_high];

end

