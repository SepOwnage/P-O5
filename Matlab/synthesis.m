function [ out, bands_used ] = synthesis( subbands, filters, scalings, bands_used )
%SYNTHESIS Thakes subbands and recombines them using QMF filters to a
%single signal
%   subbands: a cell array that contains the subband signals as arrays
%   (integers)
%   filters: as outputed by analysis. (see analysis).
%   scalings: see analysis
%   bands_used: for recursive calls.  Set to 0 for top call.

if(length(filters)>1) %if not at a trivial level, do recursive call
    scalings_low = scalings{2}{1};
    filters_low = filters{2}{1};
    [low, bands_used] = synthesis(subbands, filters_low, scalings_low, bands_used);
    
    if length(filters{2})>1 %higher frequency subband was also splitted
        scalings_high = scalings{2}{2};
        filters_high = filters{2}{2};
        [high, bands_used] = synthesis(subbands, filters_high, scalings_high, bands_used);
    else
        high = subbands{bands_used+1};
        bands_used = bands_used+1;
    end
else %trivial case (deepest): just take two subbands
    low = subbands{bands_used+1};
    bands_used = bands_used+1;
    high = subbands{bands_used+1};
    bands_used = bands_used+1;
end

%extra the scaling factor and the filter
scaling = scalings{1};
filter = filters{1};

low = int32(low); %int32 for the incoming addition and substraction
high = int32(high);

%pad the signals if necessary
if length(low)> length(high)
    high(end+1:end + length(low)- length(high)) = 0;
else if length(low)< length(high) 
        low(end+1:end - length(low)+ length(high)) = 0;
    end
end
%---apply the QMF synthesis filter:---
%recombine subbands
A1_input = low + high;
A0_input = low - high;

A1_input_downscaled = int16(fix(double(A1_input)/double(2)));
A0_input_downscaled = int16(fix(double(A0_input)/double(2)));

%get filter polyphase componens
A0 = filter(1:2:end);
A1 = filter(2:2:end);

%apply the filter: convolute
A0_out = max(-2^39, min(conv(double(A0),double(A0_input_downscaled)),2^39-1));
A1_out = max(-2^39, min(conv(double(A1),double(A1_input_downscaled)),2^39-1));
%scale the result
A0_out_downscaled = int16(fix(A0_out/2^scaling));  
A1_out_downscaled = int16(fix(A1_out/2^scaling));

%upsample results and put them together
A1_upsampled = int16(zeros(max(length(A1_out),length(A0_out))*2,1));
A1_upsampled(2:2:length(A1_out)*2) = A1_out_downscaled;
A0_upsampled = int16(zeros(max(length(A1_out),length(A0_out))*2,1));
A0_upsampled(1:2:length(A0_out)*2) = A0_out_downscaled;

out = A0_upsampled + A1_upsampled;  % not a real addition (always one of two is zero)
end

