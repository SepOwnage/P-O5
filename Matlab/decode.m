function out = decode(input, mu, phi, buffer_length)
% DECODE uses the quantized differential signal input to recreate the 
%   original signal.  It is the inverse of ENCODE (lossy).
%   input: quantized differential signal created by ENCODE.
%   mu: a parameter for the 1 step prediction of the next value.
%   phi: a parameter that determines the stepsize - variance fraction
%   buffer_length: the amount of samples over which the variance is
%   calculated
%   The last three parameters should be the same as used for encoding.

    out = zeros(length(input),1);
    
    prediction = 0;
    stepsize = 1;
    prev_dequantized_sample = 0;
    buffer = zeros(buffer_length,1);
    buffersum = 0;
    for i = 1:length(input)
        quantized_difference = input(i);
        dequantized_difference = quantized_difference * stepsize;
        %update the buffersum (don't recalculate fully)
        buffersum = buffersum - buffer(mod(i,buffer_length)+1)...
            + abs(dequantized_difference);
        buffer(mod(i,buffer_length)+1) = abs(dequantized_difference);
        %extract used stepsize from buffersum (~ variance)
        stepsize = max(phi*buffersum/buffer_length,1);
        dequantized_sample = dequantized_difference + prediction;
        out(i) = dequantized_sample;
        prediction = dequantized_sample - mu * prev_dequantized_sample;  
        prev_dequantized_sample = dequantized_sample;
    end

end