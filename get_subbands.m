function [ out_low_downscaled, out_high_downscaled, filters_out ] =...
    get_subbands(input,filter_lengths, Astops, depth)
%GET_SUBBANDS Summary of this function goes here
%   Detailed explanation goes here
if(mod(length(input),2))
    %warning('added zeros in get_subbands')
    input = [input; 0];
end
input_e0 = [input(1:2:end);0]; %zero to make it as long as the next
input_e1 = [0; input]; %delay
input_e1 = input_e1(1:2:end); % decimated

%create filter
FLength = filter_lengths{1};
filter_lengths(1) = [];
Astop = Astops{1};
Astops(1) = [];

fs = 8e3;
df = fs/10;
fstep = 10;
Niter = 600;
filter = QMF_design(fs, df, Astop, fstep, Niter, FLength);
filter = int16(filter/max(abs(filter))*(2^15-1));


%apply filter

A0 = filter(1:2:end);
A1 = filter(2:2:end);

%would be hardcoded in final filter
max_out_of_A0 = sum(double(A0(A0>0)));
min_out_of_A0 = sum(double(A0(A0<0)));
max_out_of_A1 = sum(double(A1(A1>0)));
min_out_of_A1 = sum(double(A1(A1<0)));
max_filter_out = max(abs([max_out_of_A0 max_out_of_A1...
                          min_out_of_A0 min_out_of_A1]))*2^15;
bits_to_shift = ceil(log2(max_filter_out))-15;

A0_out = int64(max(-2^39, min(conv(double(A0),double(input_e0)),2^39-1)));
A1_out = int64(max(-2^39, min(conv(double(A1),double(input_e1)),2^39-1)));
%A0_out = fftfilt(A0,input_e0);
%A1_out = fftfilt(A1,input_e1);
A0_out_downscaled = int32(A0_out/2^bits_to_shift);  %32 for MATLAB, numbers dont go over 16
A1_out_downscaled = int32(A1_out/2^bits_to_shift);


out_low = A0_out_downscaled + A1_out_downscaled;
out_high = A0_out_downscaled - A1_out_downscaled;

out_low_downscaled = int16(out_low/2);
out_high_downscaled = int16(out_high/2);

filters_out = cell(depth,1);
filters_out{1}= filter;

if (depth > 1)
    filter_lengths_low = filter_lengths;
    filter_lengths_high = filter_lengths;
    Astops_low = Astops;
    Astops_high = Astops;
    for i=1:length(filter_lengths)
        filter_lengths_low{i}(end/2+1:end)=[];
        filter_lengths_high{i}(1:end/2)=[];
        Astops_low{i}(end/2+1:end) = [];
        Astops_high{i}(1:end/2) = [];
    end
        
    [out_low_low, out_low_high, filters_low] = get_subbands(out_low_downscaled,...
        filter_lengths_low, Astops_low,depth-1);
    [out_high_low, out_high_high, filters_high] = get_subbands(out_high_downscaled,...
        filter_lengths_high, Astops_high,depth-1);
    if(size(out_low_low,1)>size(out_low_high,1))
        out_low_high(end+1:end+size(out_low_low,1)-size(out_low_high,1),:)...
            = zeros(size(out_low_low,1)-size(out_low_high,1), size(out_low_low,2));
    else
        out_low_low(end+1:end+size(out_low_high,1)-size(out_low_low,1),:)...
            = zeros(size(out_low_high,1)-size(out_low_low,1), size(out_low_low,2));
    end
    
    if(size(out_high_low,1)>size(out_high_high,1))
        out_high_high(end+1:end+size(out_high_low,1)-size(out_high_high,1),:)...
            = zeros(size(out_high_low,1)-size(out_high_high,1), size(out_high_low,2));
    else
        out_high_low(end+1:end+size(out_high_high,1)-size(out_high_low,1),:)...
            = zeros(size(out_high_high,1)-size(out_high_low,1), size(out_high_low,2));
    end
    
       
    out_low_downscaled = [out_low_low out_low_high];
    out_high_downscaled = [out_high_low out_high_high];
    
    for i = 2:depth
        filters_out{i} = [filters_low{i-1} filters_high{i-1}];
    end
end


end

