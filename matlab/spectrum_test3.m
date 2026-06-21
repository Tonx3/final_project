clear; clc; close all;

% ---------------- USER SETTINGS ----------------
port = "COM7";        
baud = 115200;

fs = 700;
N = 4096;
hop = 128;

maxFreq = 100;
historySec = 20;

notchFreqs = [50 100 150 200];
notchHalfWidth = 3;   % briše +/- 3 Hz oko harmonika
% ------------------------------------------------

s = serialport(port, baud);
configureTerminator(s, "CR/LF");
flush(s);

freq = (0:(N/2)) * fs / N;
freqMask = freq <= maxFreq;
freqPlot = freq(freqMask);

notchMask = false(size(freq));
for k = 1:length(notchFreqs)
    notchMask = notchMask | (abs(freq - notchFreqs(k)) <= notchHalfWidth);
end

numTimeBins = ceil(historySec * fs / hop);
specHistory = nan(length(freqPlot), numTimeBins);
timeAxis = linspace(-historySec, 0, numTimeBins);

sampleBuffer = zeros(N, 1);
fillCount = 0;

figure;
hImg = imagesc(timeAxis, freqPlot, specHistory);
axis xy;
xlabel("Time [s]");
ylabel("Frequency [Hz]");
title("Live EEG Spectrogram, 50 Hz harmonics removed");
colorbar;
colormap turbo;
ylim([0 maxFreq]);

disp("Reading serial data... Press Ctrl+C to stop.");

while ishandle(hImg)
    line = readline(s);
    value = str2double(strtrim(line));

    if isnan(value)
        continue;
    end

    sampleBuffer = [sampleBuffer(2:end); value];
    fillCount = fillCount + 1;

    if fillCount >= hop
        fillCount = 0;

        x = sampleBuffer;
        x = x - mean(x);

        w = hann(N);
        xw = x .* w;

        X = fft(xw);

        mag = abs(X / N);
        mag = mag(1:N/2+1);
        mag(2:end-1) = 2 * mag(2:end-1);

        % Remove 50 Hz and harmonics from displayed spectrum
        mag(notchMask) = NaN;

        mag = mag(freqMask);

        magDb = 20 * log10(mag + eps);

        specHistory = [specHistory(:, 2:end), magDb];

        set(hImg, "CData", specHistory);
        drawnow limitrate;
    end
end

clear s;