clear; clc; close all;

% ---------------- USER SETTINGS ----------------
port = "COM7";        % promijeni na svoj COM port
baud = 115200;

fs = 650;             % sample rate signala koji želiš koristiti za FFT
N = 2048;             % FFT block size

useWindow = true;     % Hann window
maxFreq = 200;         % prikaz do 60 Hz, za EEG dosta
% ------------------------------------------------

s = serialport(port, baud);
configureTerminator(s, "CR/LF");
flush(s);

buffer = zeros(N, 1);
idx = 1;

freq = (0:(N/2)) * fs / N;

figure;
h = plot(freq, zeros(size(freq)));
grid on;
xlabel("Frequency [Hz]");
ylabel("Magnitude");
title("Live EEG Spectrum");

xlim([0 maxFreq]);

disp("Reading serial data... Press Ctrl+C to stop.");

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

        % Remove DC offset
        x = x - mean(x);

        % Optional window
        if useWindow
            w = hann(N);
            xw = x .* w;
        else
            xw = x;
        end

        % FFT
        X = fft(xw);

        % Single-sided magnitude spectrum
        mag = abs(X / N);
        mag = mag(1:N/2+1);
        mag(2:end-1) = 2 * mag(2:end-1);

        set(h, "YData", mag);
        drawnow limitrate;

        idx = 1;
    end
end

clear s;