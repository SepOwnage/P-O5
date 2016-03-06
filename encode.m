function [out, nb_clips] = encode(input, mu, phi, maximum, encode_version, buffer_length)
    out = int16(zeros(length(input),1));
    
    prediction = 0;
    stepsize = 1; %check scaling
    prev_dequantized_sample = 0;
    nb_clips = 0;
    buffer = zeros(buffer_length,1);
    buffersum = 0;
    for i = 1:length(input)
        sample = input(i);
        difference = sample - prediction;
        quantized_difference = round(difference/stepsize);
        nb_clips = nb_clips + double(abs(quantized_difference>maximum));
        
        if encode_version==1
            quantized_difference = min(maximum, quantized_difference);
            quantized_difference = max(-maximum, quantized_difference);
            out(i) = quantized_difference;
        else
            out(i) = quantized_difference;
            out(i) = min(maximum, out(i));
            out(i) = max(-maximum, out(i));
        end
        
        
        dequantized_difference = quantized_difference * stepsize;
        
        buffersum = buffersum - buffer(mod(i,buffer_length)+1)...
            + abs(quantized_difference);
        buffer(mod(i,buffer_length)+1) = abs(quantized_difference);
        stepsize = max(phi*buffersum/buffer_length,1);  %TODO: better solution? Problem: zero input=>zero var=>zero stepsize
        dequantized_sample = dequantized_difference + prediction;
        prediction = dequantized_sample - mu * prev_dequantized_sample;  
        prev_dequantized_sample = dequantized_sample;
    end
end