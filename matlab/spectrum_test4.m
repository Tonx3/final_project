clear; clc; close all;

port = "COM7";
baud = 115200;

fs = 675;
N = 1024;
maxFreq = 250;

notchFreqs = [50 100 150 200];
notchHalfWidth = 3;   % briše +/- 3 Hz oko harmonika

s = serialport(port, baud);
configureTerminator(s, "CR/LF");
flush(s);

buffer = zeros(N, 1);
idx = 1;

freq = (0:(N/2)) * fs / N;
freqMask = freq <= maxFreq;
freqPlot = freq(freqMask);

figure;
h = plot(freqPlot, zeros(size(freqPlot)));
grid on;
xlabel("Frequency [Hz]");
ylabel("Magnitude");
title("Live EEG Spectrum, 50 Hz harmonics removed");
xlim([0 maxFreq]);

while ishandle(h)
    line = readline(s);
    value = str2double(strtrim(line));

    if isnan(value)
        continue;
    end

    buffer(idx) = value;
    idx = idx + 1;

    if idx > N
        x = buffer;
        x = x - mean(x);

        w = hann(N);
        x = x .* w;

        X = fft(x);

        mag = abs(X / N);
        mag = mag(1:N/2+1);
        mag(2:end-1) = 2 * mag(2:end-1);

        % Remove 50 Hz and harmonics from DISPLAY spectrum
        for k = 1:length(notchFreqs)
            f0 = notchFreqs(k);
            bad = abs(freq - f0) <= notchHalfWidth;
            mag(bad) = NaN;
        end

        set(h, "YData", mag(freqMask));
        drawnow limitrate;

        idx = 1;
    end
end

clear s;