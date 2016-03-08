results = zeros(20,1);
large_results = zeros(20*3*4*7,3*7+1);
large_counter = 1;
for sb = 5:7
    for loopiter = 1:4
        for i = 1:20
            buffer_lengths(sb) = 3*i;
            [~, results(i), ~] = ...
                run(depth, filter_lengths, Astops, scalings,...
                mus, phis, maxima, encode_version, buffer_lengths);
            %disp([i results(i,:)]);
            large_results(large_counter,:) =...
                [buffer_lengths(1:7) phis(1:7) mus(1:7) results(i)];
            large_counter = large_counter+1;
        end
        [score,imax] = max(results);
        buffer_lengths(sb) = 3*imax;
        disp({'sb', sb, 'loopiter', loopiter, 'buffer_length done', buffer_lengths(sb), 'score', score });
        
        for i = 1:20
            phis(sb) = 10+2*i;
            [~, results(i), ~] = ...
                run(depth, filter_lengths, Astops, scalings,...
                mus, phis, maxima, encode_version, buffer_lengths);
            %disp([i results(i,:)]);
            large_results(large_counter,:) =...
                [buffer_lengths(1:7) phis(1:7) mus(1:7) results(i)];
            large_counter = large_counter+1;
        end
        [score,imax] = max(results);
        phis(sb) = 10+2*imax;
        disp({'sb', sb, 'loopiter', loopiter, 'phis done', phis(sb), 'score', score});
        
        for i = 1:20
            mus(sb) = i*0.05;
            [~, results(i), ~] = ...
                run(depth, filter_lengths, Astops, scalings,...
                mus, phis, maxima, encode_version, buffer_lengths);
            %disp([i results(i,:)]);
            large_results(large_counter,:) =...
                [buffer_lengths(1:7) phis(1:7) mus(1:7) results(i)];
            large_counter = large_counter+1;
            
        end
        [score,imax] = max(results);
        mus(sb) = imax*0.05;
        disp({'sb', sb, 'loopiter', loopiter, 'mus done', mus(sb), 'score', score});
    end
end
