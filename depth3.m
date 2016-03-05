function [ output_args ] = depth3(  inputvector )
%DEPTH2 Summary of this function goes here
%   Detailed explanation goes here
%  inputvector:
%    level 1 filter length
%    level 2 low filter length
%    level 2 high filter length
%    level 3 low low f l
%    level 3 low high f l
%    level 3 high low f l
%    level 3 high high f l
%    level 1 filter attenuation
%    level 2 low attenuation
%    level 2 high attenuation
%    level 3 low low attenuation
%    level 3 low high attenuation
%    level 3 high low attenuation
%    level 3 high high attenuation
%    level 1 filter scaling
%    level 2 low scaling
%    level 2 high scaling
%    level 3 low low scaling
%    level 3 low high scaling
%    level 3 high low scaling
%    level 3 high high scaling
%    8 mus
%    8 phis
    filter_lengths = cell(3,1);
    filter_lengths{1} = inputvector(1);
    filter_lengths{2} = inputvector(2:3);
    filter_lengths{3} = inputvector(4:7);
    Astops = cell(2,1);
    Astops{1} = inputvector(8);
    Astops{2} = inputvector(9:10);
    Astops{3} = inputvector(11:14);
    scalings = cell(3,1);
    scalings{1} = inputvector(15);
    scalings{2} = inputvector(16:17);
    scalings{3} = inputvector(18:19);
    mus = inputvector(20:27);
    phis= inputvector(28:25);
    out = run(3, filter_lengths, Astops, scalings, mus, phis);
    %  calcalute and return score
end

