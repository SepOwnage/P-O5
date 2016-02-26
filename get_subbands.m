function [ out_low_downscaled, out_high_downscaled ] = get_subbands(input,filter, depth);
%GET_SUBBANDS Summary of this function goes here
%   Detailed explanation goes here
if(mod(length(input),2))
    %warning('added zeros in get_subbands')
    input = [input; 0];
end
input_e0 = [input(1:2:end);0]; %zero to make it as long as the next
input_e1 = [0; input]; %delay
input_e1 = input_e1(1:2:end); % decimated

A0 = filter(1:2:end);
A1 = filter(2:2:end);

A0_out = int64(max(-2^39, min(conv(double(A0),double(input_e0)),2^39-1)));
A1_out = int64(max(-2^39, min(conv(double(A1),double(input_e1)),2^39-1)));
%A0_out = fftfilt(A0,input_e0);
%A1_out = fftfilt(A1,input_e1);
A0_out_downscaled = int32(bitshift(A0_out, 24));  %32 for MATLAB, numbers dont go over 16
A1_out_downscaled = int32(bitshift(A1_out, 24));


out_low = A0_out_downscaled + A1_out_downscaled;
out_high = A0_out_downscaled - A1_out_downscaled;

out_low_downscaled = int16(bitshift(out_low,1));
out_high_downscaled = int16(bitshift(out_high,1));

if (depth > 1)
    [out_low_low, out_low_high] = get_subbands(out_low_downscaled,filter,depth-1);
    [out_high_low, out_high_high] = get_subbands(out_high_downscaled,filter,depth-1);
    out_low_downscaled = [out_low_low out_low_high];
    out_high_downscaled = [out_high_low out_high_high];
end


end

