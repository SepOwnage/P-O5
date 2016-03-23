function [ PESQ ] = alignpesq( input, output )
%ALIGNPESQ Aligns the input and output and then calculates the pesq score
% The best alignment is chosen to be the one with minimal sum of absolute
% difference between the two signals
%%%%%%%%%%%%%%%%%%%%Alignment
searching_size = 1000;
values = zeros(searching_size,1);

%Scale input and output to 1
input_scaled = double(input)/double(max(abs(input)));
output_scaled = double(output)/double(max(abs(output)));

%Find alignment difference as the minimum difference
for i = 1:searching_size
    input_scaled_aligned = [zeros(i,1); input_scaled];
    input_scaled_aligned_sized = input_scaled_aligned;
    output_scaled_sized = output_scaled;
    %pad if necessary
    if length(input_scaled_aligned) > length(output_scaled)
        output_scaled_sized = [output_scaled; zeros(length(input_scaled_aligned)-length(output_scaled),1)];
    else
        input_scaled_aligned_sized = [input_scaled_aligned; zeros(length(output_scaled)-length(input_scaled_aligned),1)];
    end
    %get the absolute difference between the signals with this alignment
    values(i) = sum(abs(input_scaled_aligned_sized - output_scaled_sized));
end
[M, index] = min(values);
%calculate the pesq rate for this best alignment
input_aligned = [zeros(index,1); input_scaled];
PESQ = pesq(input_aligned, output_scaled, 8000);

end

