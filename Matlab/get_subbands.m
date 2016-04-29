function [ out_low_downscaled, out_high_downscaled, filters_out ] =...
    get_subbands(input,filter_lengths, Astops, scalings)
%GET_SUBBANDS splits input into subbands using QMF-filters
%   input: the input signal (1 channel, integers)
%   filter_lengths, Astops, scalings: see analysis
%   returns:
%     out_low_downscaled: the lower frequency signals (as arrays in a cell
%     array)
%     out_high_downscaled: the higher frequency signals (as arrays in a cell
%     array)
%     filters_out: the created and used filters. (see analysis)

%pad if necessary
if(mod(length(input),2))
    input = [input; 0];
end
input_e0 = [input(1:2:end);0]; %zero to make it as long as the next
input_e1 = [0; input]; %delay element       |
input_e1 = input_e1(1:2:end); % decimated   |=> e1 always starts with a 0 

%CREATE FILTE
%get the parameters from arguments
FLength = filter_lengths{1};
Astop = Astops{1};
%set basic parameters that are always used
fs = 8e3; % (use of this?)
df = fs/10;
fstep = 10;
Niter = 600;
%create and scale filter
filter = QMF_design(fs, df, Astop, fstep, Niter, FLength);
filter = int16(filter/max(abs(filter))*(2^15-1));
filter = filter';

%APPLY FILTER
%extract polyphase components of the filter
A0 = filter(1:2:end);
A1 = filter(2:2:end);
%do the convolution and clip to range of DSP
A0_out = conv(double(A0),double(input_e0));
A0_out = mod(A0_out, 2^32);
A0_out(A0_out>=2^31) = int32(A0_out(A0_out>=2^31)  - 2^32);
A1_out = conv(double(A1),double(input_e1));
A1_out = mod(A1_out, 2^32);
A1_out(A1_out>=2^31) = int32(A1_out(A1_out>=2^31)  - 2^32);
%TODO: keep track of amount of clips?  (same with synthesis)

%extract the scaling factor and apply it
scaling = scalings{1};

A0_out_downscaled = int32(fix(double(A0_out)/double(2^scaling)));  %32 for next lines already, 
A1_out_downscaled = int32(fix(double(A1_out)/double(2^scaling)));  %numbers dont go over 16

%combine the polyphase outputs to get the subbands
out_low = A0_out_downscaled + A1_out_downscaled;
out_high = A0_out_downscaled - A1_out_downscaled;

%again, scale the result
out_low_downscaled = {int16(fix(double(out_low)/double(2)))};
out_high_downscaled = {int16(fix(double(out_high)/double(2)))};


if length(Astops)>1  % if this, further splitting of the lower subband must be done
    %----furter splitting of the lower band:----
    %get the params
    filter_lengths_low = filter_lengths{2}{1};
    Astops_low = Astops{2}{1};
    scalings_low = scalings{2}{1};
    %recursive call
    [out_low_low, out_low_high, filters_low] = get_subbands(out_low_downscaled{1},...
        filter_lengths_low, Astops_low, scalings_low);
    %collect results   
    out_low_downscaled = [out_low_low out_low_high];
    
    if length(Astops{2})>1 % if this, further splitting of the higher subband must be done too
        %----furter splitting of the higher band:----
        %get the params
        filter_lengths_high = filter_lengths{2}{2};
        Astops_high = Astops{2}{2};
        scalings_high = scalings{2}{2};
        %recursive call
        [out_high_low, out_high_high, filters_high] = get_subbands(out_high_downscaled{1},...
        filter_lengths_high, Astops_high, scalings_high);
        %collect results   
        out_high_downscaled = [out_high_low out_high_high];
        filters_out = cell(2,1);
        filters_out{1} = filter;
        filters_out{2} = {filters_low, filters_high};
    else
        filters_out = cell(2,1);
        filters_out{1} = filter;
        filters_out{2} = {filters_low};
    end

else
   filters_out = cell(1,1);
   filters_out{1}= filter; 
    
end %end of if

end %end of function

