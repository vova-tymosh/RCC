from infra import *

l = SerialComm.listPorts()
if len(l) == 0:
    print('*** No serial ports found')
    exit(1)

SerialComm.locoPortIndex = 0
s = SerialComm.openPort()
print(f'Upload settings to device:  {l[0].device} - {l[0].description}')
updateSettings(s)
del s
