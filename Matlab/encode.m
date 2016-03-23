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

    out = int16(zeros(length(input),1));
    prediction = 0;
    stepsize = 1; 
    prev_dequantized_sample = 0;
    nb_clips = 0;
    buffer = zeros(buffer_length,1);
    buffersum = 0;
    for i = 1:length(input)
        sample = input(i);
        difference = sample - prediction;
        %TODO decide on rounding (matlab divison) or truncating (c
        %division) -> floor vs round
        quantized_difference = int16(sign(difference*stepsize)*floor(abs(double(difference)/double(stepsize))));
        nb_clips = nb_clips + double(abs(quantized_difference>maximum));
        
        quantized_difference = min(maximum, quantized_difference);
        quantized_difference = max(-maximum, quantized_difference);
        out(i) = quantized_difference;
       
        
        dequantized_difference = quantized_difference * stepsize;
        
        buffersum = buffersum - buffer(mod(i,buffer_length)+1)...
            + abs(dequantized_difference);
        buffer(mod(i,buffer_length)+1) = abs(dequantized_difference);
        %TODO decide on rounding (matlab divison) or truncating (c
        %division)
        stepsize = max(phi*int16(sign(difference*stepsize)*floor(abs(double(buffersum)/double(buffer_length)))),1);  %TODO: better solution? Problem: zero input=>zero var=>zero stepsize
        dequantized_sample = dequantized_difference + prediction;
        prediction = dequantized_sample - mu * prev_dequantized_sample;  
        prev_dequantized_sample = dequantized_sample;
    end
end