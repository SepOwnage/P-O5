function [ out ] = synthesis( subbands, filters, scalings )
%SYNTHESIS Summary of this function goes here
%   Detailed explanation goes here



if(size(subbands,2) > 2)
    scalings_low = scalings(2:end);
    scalings_high = scalings(2:end);
    filters_low = filters(2:end);
    filters_high = filters(2:end);
    for i=1:length(filters_low)
        filters_low{i} = filters_low{i}{1};
        filters_high{i} = filters_high{i}{2};
        scalings_low{i}(end/2+1:end) = [];
        scalings_high{i}(1:end/2) = [];
    end;
    low = synthesis(subbands(:,1:end/2), filters_low, scalings_low);
    high = synthesis(subbands(:,end/2+1:end), filters_high, scalings_high);
    
    if(length(low)<length(high))
        low(end+1:end+length(high)-length(low)) = 0;
    else if (length(low)>length(high))
        high(end+1:end+length(low)-length(high)) = 0;
        end
    end
else
    low=subbands(:,1);
    high = subbands(:,2);
end
scaling = scalings{1};
filter = filters{1};

low = int32(low);
high = int32(high);

A1_input = low + high;
A0_input = low - high;

A1_input_downscaled = int16(A1_input/2);
A0_input_downscaled = int16(A0_input/2);

A0 = filter(1:2:end);
A1 = filter(2:2:end);

A0_out = int64(max(-2^39, min(conv(double(A0),double(A0_input_downscaled)),2^39-1)));
A1_out = int64(max(-2^39, min(conv(double(A1),double(A1_input_downscaled)),2^39-1)));
%A1_out = fftfilt(A1,A1_input);
%A0_out = fftfilt(A0,A0_input);

%would be hardcoded in final filter
% max_out_of_A0 = sum(double(A0(A0>0)));
% min_out_of_A0 = sum(double(A0(A0<0)));
% max_out_of_A1 = sum(double(A1(A1>0)));
% min_out_of_A1 = sum(double(A1(A1<0)));
% max_filter_out = max(abs([max_out_of_A0 max_out_of_A1...
%                           min_out_of_A0 min_out_of_A1]))*2^15;
% bits_to_shift = ceil(log2(max_filter_out))-15;

A0_out_downscaled = int16(A0_out/2^scaling);  
A1_out_downscaled = int16(A1_out/2^scaling);

A1_upsampled = int16(zeros(max(length(A1_out),length(A0_out))*2,1));
A1_upsampled(2:2:length(A1_out)*2) = A1_out_downscaled;
A0_upsampled = int16(zeros(max(length(A1_out),length(A0_out))*2,1));
A0_upsampled(1:2:length(A0_out)*2) = A0_out_downscaled;

out = A0_upsampled + A1_upsampled;  % not a real addition




end

