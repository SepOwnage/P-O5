function out = encode(input, mu, phi)
    out = zeros(length(input),1);
    
    prediction = 0;
    stepsize = 1/8; %check scaling
    prev_dequantized_sample = 0;
    var = 0;
    
    for i = 1:length(input)
        sample = input(i);
        difference = sample - prediction;
        quantized_difference = round(difference/stepsize);
        out(i) = quantized_difference;
        dequantized_difference = quantized_difference * stepsize;
        var = (var * (i-1) + abs(quantized_difference))/i;
        stepsize = max(phi*var,1e-4);  %TODO: better solution? Problem: zero input=>zero var=>zero stepsize
        dequantized_sample = dequantized_difference + prediction;
        prediction = dequantized_sample - mu * prev_dequantized_sample;  
        prev_dequantized_sample = dequantized_sample;
    end
end