clear; clc;

port = "COM7";
baud = 115200;
numChannels = 8;

HEADER = uint8([hex2dec("AA"), hex2dec("55")]);
packetLen = 2 + 1 + 1 + 4*numChannels;

s = serialport(port, baud);
flush(s);

disp("Reading binary UART packets... Ctrl+C to stop.");

while true
    b = read(s, 1, "uint8");

    if b ~= HEADER(1)
        continue;
    end

    b2 = read(s, 1, "uint8");

    if b2 ~= HEADER(2)
        continue;
    end

   payload = read(s, packetLen - 2, "uint8");

    timestamp_lsb = payload(1);
    pending_lsb   = payload(2);
    
    channels = zeros(1, numChannels, "int32");
    
    idx = 3;
    for ch = 1:numChannels
        channels(ch) = typecast(uint8(payload(idx:idx+3)), "int32");
        idx = idx + 4;
    end
    
    fprintf("TS_LSB=%u pending=%u", timestamp_lsb, pending_lsb);
    for ch = 1:numChannels
        fprintf(" CH%d=%ld", ch, channels(ch));
    end
    fprintf("\n");
end