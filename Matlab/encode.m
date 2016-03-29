function [out, nb_clips] = encode(input, mu, phi, maximum, buffer_length)
% ENCODE takes a signal and applies adaptive stepsize differential encoding
% on it to compress it lossy. 
%   input: input signal as integers ( int16 )
%   mu: a parameter for the 1 step prediction of the next value.
%   phi: a parameter that determines the stepsize - variance fraction
%   buffer_length: the amount of samples over which the variance is
%   calculated
%   Maximum: the maximum absolute value of the output. Larger values are
%   clipped.  This determines the bitrate.
%   returns:
%     out: the output signal
%     nb_clips: the amount of difference that had to be clipped due to
%     maximum. Only for analysis purposes.
    
    %preallocation
    out = int16(zeros(length(input),1));
    %init
    prediction = int16(0);
    stepsize = 1; 
    prev_dequantized_sample = 0;
    nb_clips = 0;
    buffer = int16(zeros(buffer_length,1));
    buffersum = int32(0);
    %main loop
    for i = 1:length(input)
        sample = input(i);
        %get the difference, quantize it using current stepsize
        difference = int32(sample) - int32(prediction);
        %TODO decide on rounding (matlab divison) or truncating (c
        %division) -> floor vs round
        to_trunc = double(difference)/double(stepsize);
        quantized_difference = int16(fix(to_trunc));
        nb_clips = nb_clips + double(abs(quantized_difference>maximum));

        %clip the value to the given maximum
        quantized_difference = min(maximum, quantized_difference);
        quantized_difference = max(-(maximum+1), quantized_difference);
        out(i) = quantized_difference;
       
        dequantized_difference = int32(quantized_difference) * int32(stepsize);
        
        %calculate the variance of the dequantized difference 
        %(dequantized => sender and receiver use same info)
        buffersum = buffersum - int32(buffer(mod(i,buffer_length)+1))...
            + int32(abs(dequantized_difference));
        buffer(mod(i,buffer_length)+1) = abs(dequantized_difference);
        %TODO decide on rounding (matlab divison) or truncating (c
        %division)
        %update the stepsize
        to_trunc = double(buffersum)*double(phi)/double(buffer_length*2^15);
        stepsize = max(int16(fix(to_trunc)),1);  %TODO: better solution? Problem: zero input=>zero var=>zero stepsize
        %get the dequantized sample, and predict the next one
        dequantized_sample = int16(dequantized_difference) + prediction;
        prediction = dequantized_sample -...
            int16((int32(mu) * int32(prev_dequantized_sample))/2^15);    
        prev_dequantized_sample = dequantized_sample;
    end
end