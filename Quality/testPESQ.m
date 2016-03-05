run(depth, filter_lengths, Astops, scalings, mus, phis);

values = zeros(10,1);
for i = 1:10
    Astops(1) = [10*i];
    output = run(depth, filter_lengths, Astops, scalings, mus, phis);
    values(i) = alignpesq(input,output);   
end