function out = decode(input, mu, phi, buffer_length)
    out = zeros(length(input),1);
    
    prediction = 0;
    stepsize = 1; %check scaling
    prev_dequantized_sample = 0;
    buffer = zeros(buffer_length,1);
    buffersum = 0;
    for i = 1:length(input)
        quantized_difference = input(i);
        dequantized_difference = quantized_difference * stepsize;
        buffersum = buffersum - buffer(mod(i,buffer_length)+1)...
            + abs(dequantized_difference);
        buffer(mod(i,buffer_length)+1) = abs(dequantized_difference);
        stepsize = max(phi*buffersum/buffer_length,1);
        dequantized_sample = dequantized_difference + prediction;
        out(i) = dequantized_sample;
        prediction = dequantized_sample - mu * prev_dequantized_sample;  
        prev_dequantized_sample = dequantized_sample;
    end

end