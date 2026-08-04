#!/bin/bash
# Try a preview orientation live: ./orient.sh 0..7
# bit0 rotate CW, bit1 sensor mirror, bit2 sensor vflip
exec uvx --from pyserial python -c "
import serial,sys,time
s=serial.Serial('/dev/ttyACM0',115200,timeout=0.3)
s.write(sys.argv[1].encode()); s.flush()
t=time.time()
while time.time()-t<2:
    d=s.read(256)
    if d: sys.stdout.write(d.decode('utf8','replace'))
" "${1:-0}"
