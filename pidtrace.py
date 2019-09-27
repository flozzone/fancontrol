import serial
import struct
from datetime import datetime

with serial.Serial('/dev/ttyUSB0', 115200, timeout=2) as ser:
    with open("trace.csv", "w") as f:
        fmt = '@ffi'
        lbl = ["temp", "setPoint", "pwm"]

        f.write(",".join(["time"] + lbl) + "\n")
        while 1:
            s = ser.read(64)
            b = bytes(s)
            part = b[:struct.calcsize(fmt)]
            print(part)
            unpacked = struct.unpack(fmt, part)
            data = dict()
            for i in range(len(unpacked)):
                data[lbl[i]] = unpacked[i]

            now = datetime.now()

            f.write("{},{},{},{}\n".format(now.strftime("%H:%M:%S"), data["temp"], data["setPoint"], data["pwm"]))
            f.flush()
