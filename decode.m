function out = decode(input, mu, phi)
    out = zeros(length(input),1);
    
    prediction = 0;
    stepsize = 1; %check scaling
    prev_dequantized_sample = 0;
    var = 0;
    
    for i = 1:length(input)
        dequantized_difference = input(i) * stepsize;
        var = (var * (i-1) + abs(input(i)))/i;
        stepsize = max(phi*var,1);
        dequantized_sample = dequantized_difference + prediction;
        out(i) = dequantized_sample;
        prediction = dequantized_sample - mu * prev_dequantized_sample;  
        prev_dequantized_sample = dequantized_sample;
    end

end