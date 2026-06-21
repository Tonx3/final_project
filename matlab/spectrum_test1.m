clear; clc;

port = "COM7";        % promijeni na svoj COM port
baud = 115200;

s = serialport(port, baud);
configureTerminator(s, "CR/LF");
flush(s);

fs = 250;             % očekivani sampling rate
windowSec = 5;
N = fs * windowSec;

data = nan(1, N);
t = linspace(-windowSec, 0, N);

figure;
h = plot(t, data);
grid on;
xlabel("Time [s]");
ylabel("ADC counts");
title("ADS1299 EEG live");

while ishandle(h)
    line = readline(s);
    value = str2double(line);

    if ~isnan(value)
        data = [data(2:end), value];

        set(h, "YData", data);
        drawnow limitrate;
    end
end

clear s;