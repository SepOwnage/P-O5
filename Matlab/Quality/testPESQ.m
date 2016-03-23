for i = 1:20
    phis(3) = .05*i*(2^15-1);
    [~,~,results(i)] = run(filter_lengths, Astops, scalings,mus, phis, maxima, buffer_lengths);
    disp([i results(i)]);
end
       