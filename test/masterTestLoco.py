#!/usr/bin/python3

import time
import logging
from test_local import *
from test_mqtt import tests_mq


# tests = [test_motor_bemf, ]

def test_boot(s):
    test_name = 'Boot the thing'
    yn = input('\tBoot/reset the device and confirm the Red light is on. (Y/n)')
    return (yn.lower() != 'n', test_name)

# tests = [test_boot, test_voltage,
#     test_current, test_current_with_load, test_motor_bemf,
#     test_f0_on, test_f0_blinking, test_f1_blinking,
#     test_motor_forward, test_motor_backward]

tests = []
tests += tests_mq

logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(message)s',
                    filename='rcc_test.log',
                    filemode='a')
logging.error('*** Test Start ***')


if __name__ == '__main__':
    ser_name = findSerial()
    if ser_name == None:
        print('No serial port found')
        exit(1)
    s = openSerial(ser_name)

    w = 80
    for test in tests:
        result, name = test(s)
        if result:
            print(name + 'ok'.rjust(w - len(name), '.'))
        else:
            print(name + 'FAIL'.rjust(w - len(name), '.'))

    s.close()
