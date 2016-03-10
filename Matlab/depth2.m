function [ output_args ] = depth2(  inputvector )
%DEPTH2 Summary of this function goes here
%   Detailed explanation goes here
%  inputvector:
%    level 1 filter length
%    level 2 low filter length
%    level 2 high filter length
%    level 1 attenuation
%    level 2 low attenuation
%    level 2 high attenuation
%    level 1 scaling
%    level 2 low scaling
%    level 2 high scaling
%    4 mus
%    4 phis
    filter_lengths = cell(2,1);
    filter_lengths{1} = inputvector(1);
    filter_lengths{2} = inputvector(2:3);
    Astops = cell(2,1);
    Astops{1} = inputvector(4);
    Astops{2} = inputvector(5:6);
    scalings = cell(2,1);
    scalings{1} = inputvector(7);
    scalings{2} = inputvector(8:9);
    mus = inputvector(10:13);
    phis= inputvector(14:18);
    out = run(2, filter_lengths, Astops, scalings, mus, phis);
    %  calcalute and return score
end

