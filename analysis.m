function [ output_links, output_rechts ] = analysis(input, filter, depth)
%ANALYSIS Summary of this function goes here
%   Detailed explanation goes here

input_links = input(1:2:end);
[ out_low, out_high ] = get_subbands(input_links,filter, depth);
output_links = [out_low out_high];

input_rechts = input(2:2:end);
[ out_low, out_high ] = get_subbands(input_rechts,filter, depth);
output_rechts = [out_low out_high];

end

