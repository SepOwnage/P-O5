results = zeros(21,2);
for i = 1:20
    buffer_lengths(2) = 30+i;
    [~, results(i,1), results(i,2)] = ...
        run(depth, filter_lengths, Astops, scalings,...
        mus, phis, maxima, encode_version, buffer_lengths);
    disp([i results(i,:)]);
end