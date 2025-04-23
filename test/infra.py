import serial
import time
import subprocess
import select
import sys
import logging


def millis():
    return round(time.time() * 1000)

def get_unblocked_input():
    output, _, _ = select.select([sys.stdin], [], [], 0)
    if output:
        return sys.stdin.readline()
    return None

def findSerial():
    output = subprocess.check_output(["ls /dev/cu.usb*"], shell=True)
    usbmodem = output.decode('utf-8').strip().split('\n')
    if len(usbmodem) == 0:
        return None
    return usbmodem[0]

def openSerial(ser_name):
    return serial.Serial(ser_name, 115200)

def writeSerial(s, data):
    d = data.encode('utf-8')
    logging.info("Write to serial: %s"%d)
    s.write(d)

def printSerial(s, printLog=False):
    for i in range(1000):
        if s.in_waiting > 0:
            data = s.readline().decode('utf-8')
            logging.info("Read from serial: %s"%data.strip())

def readSerial(s):
    buffer = ''
    for i in range(10000):
        if s.in_waiting > 0:
            data = s.readline().decode('utf-8')
            logging.info("Read from serial22: %s"%data)
            if data:
                buffer += data
    logging.info("Read from serial: %s"%buffer)
    return buffer

def readSerialFloat(s):
    data = readSerial(s)
    try:
        if data:
            data = data.split()[0]
            return float(data)
    except:
        pass
    return 0.0
