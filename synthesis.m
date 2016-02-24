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

A1_input = low + high;
A0_input = low - high;

A0 = filter(1:2:end)*2;
A1 = filter(2:2:end)*2;

A1_out = conv(A1,A1_input);
A0_out = conv(A0,A0_input);
%A1_out = fftfilt(A1,A1_input);
%A0_out = fftfilt(A0,A0_input);

A1_upsampled = zeros(size(A1_out,1)*2,1);
A1_upsampled(2:2:end) = A1_out;
A0_upsampled = zeros(size(A0_out,1)*2,1);
A0_upsampled(1:2:end) = A0_out;

out = A0_upsampled + A1_upsampled;




end

