function out = decode(input, mu, phi, buffer_length)
% DECODE uses the quantized differential signal input to recreate the 
%   original signal.  It is the inverse of ENCODE (lossy).
%   input: quantized differential signal created by ENCODE.
%   mu: a parameter for the 1 step prediction of the next value.
%   phi: a parameter that determines the stepsize - variance fraction
%   buffer_length: the amount of samples over which the variance is
%   calculated
%   The last three parameters should be the same as used for encoding.
    
    %preallocation
    out = int16(zeros(length(input),1));
    %init
    prediction = int16(0);
    stepsize = 1;
    prev_dequantized_sample = 0;
    buffer = zeros(buffer_length,1);
    buffersum = int32(0);
    %go through input sequentially
    for i = 1:length(input)
        quantized_difference = input(i);
        dequantized_difference = int32(quantized_difference) * int32(stepsize);
        %update the buffersum (don't recalculate fully)
        buffersum = buffersum - int32(buffer(mod(i,buffer_length)+1))...
            + int32(abs(dequantized_difference));
        buffer(mod(i,buffer_length)+1) = abs(dequantized_difference);
        %extract used stepsize from buffersum (~ variance)
        %TODO decide on rounding (matlab divison) or truncating (c
        %division)
        %update the stepsize
        to_trunc = double(buffersum)*double(phi)/double(buffer_length*2^15);
         stepsize = max(int16(fix(to_trunc)),1);  %TODO: better solution? Problem: zero input=>zero var=>zero stepsize  %TODO: better solution? Problem: zero input=>zero var=>zero stepsize
        %get the dequantized sample (=output)
        dequantized_sample = int16(dequantized_difference) + prediction;
        out(i) = dequantized_sample;
        %predict the next value
        prediction = dequantized_sample -...
            int16(fix(  double(mu) * double(prev_dequantized_sample)/double(2^15))  );   
        prev_dequantized_sample = dequantized_sample;
    end

end
