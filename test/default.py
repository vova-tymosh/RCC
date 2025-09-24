from infra import *

l = SerialComm.listPorts()
if len(l) == 0:
    print('*** No serial ports found')
    exit(1)

s = SerialComm.openPort(0)
print(f'Upload settings from default.cfg (your wifi & network config) to device:  {l[0].device} - {l[0].description}')
updateSettings(s)
del s
