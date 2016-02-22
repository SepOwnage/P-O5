function [ out_low out_high ] = get_subbands(input,filter, depth);
%GET_SUBBANDS Summary of this function goes here
%   Detailed explanation goes here
if(mod(length(input),2))
    warning('added zeros in get_subbands')
    input = [input; 0];
end
input_e0 = [input(1:2:end);0]; %zero to make it as long as the next
input_e1 = [0; input]; %delay
input_e1 = input_e1(1:2:end); % decimated

A0 = filter(1:2:end)*2;
A1 = filter(2:2:end)*2;

%A0_out = conv(A0,input_e0);
%A1_out = conv(A1,input_e1);
A0_out = fftfilt(A0,input_e0);
A1_out = fftfilt(A1,input_e1);

out_low = A0_out + A1_out;
out_high = A0_out - A1_out;

if (depth > 1)
    [out_low_low out_low_high] = get_subbands(out_low,filter,depth-1);
    [out_high_low out_high_high] = get_subbands(out_high,filter,depth-1);
    out_low = [out_low_low out_low_high];
    out_high = [out_high_low out_high_high];
end


end

