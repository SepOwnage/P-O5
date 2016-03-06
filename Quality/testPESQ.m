results = zeros(21,2);
for i = 1:20
    phis(2) = i+20;
    [~, results(i,1), results(i,2)] = ...
        run(depth, filter_lengths, Astops, scalings,...
        mus, phis, maxima, encode_version, buffer_length);
    disp([i results(i,:)]);
end