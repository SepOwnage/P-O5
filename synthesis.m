function [ out ] = synthesis( subbands, filter )
%SYNTHESIS Summary of this function goes here
%   Detailed explanation goes here



if(size(subbands,2) > 2)
    low = synthesis(subbands(:,1:end/2), filter);
    high = synthesis(subbands(:,end/2+1:end), filter);
else
    low=subbands(:,1);
    high = subbands(:,2);
end

low= int32(low);
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
max_out_of_A0 = sum(double(A0(A0>0)));
min_out_of_A0 = sum(double(A0(A0<0)));
max_out_of_A1 = sum(double(A1(A1>0)));
min_out_of_A1 = sum(double(A1(A1<0)));
max_filter_out = max(abs([max_out_of_A0 max_out_of_A1...
                          min_out_of_A0 min_out_of_A1]))*2^15;
bits_to_shift = ceil(log2(max_filter_out))-15;

A0_out_downscaled = int16(A0_out/2^bits_to_shift);  
A1_out_downscaled = int16(A1_out/2^bits_to_shift);

A1_upsampled = int16(zeros(size(A1_out,1)*2,1));
A1_upsampled(2:2:end) = A1_out_downscaled;
A0_upsampled = int16(zeros(size(A0_out,1)*2,1));
A0_upsampled(1:2:end) = A0_out_downscaled;

out = A0_upsampled + A1_upsampled;  % not a real addition




end

