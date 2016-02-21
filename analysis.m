function [ output_links, output_rechts ] = analysis(input, filter, depth)
%ANALYSIS Summary of this function goes here
%   Detailed explanation goes here

if mod(length(input),2^(depth+1))
    amount_to_add = 2^(depth+1) - mod(length(input),2^(depth+1));
    input = [input; zeros(amount_to_add,1)*2];
end

input_links = input(1:2:end);
[ out_low, out_high ] = get_subbands(input_links,filter, depth);
output_links = [out_low out_high];

input_rechts = input(2:2:end);
[ out_low, out_high ] = get_subbands(input_rechts,filter, depth);
output_rechts = [out_low out_high];

end

