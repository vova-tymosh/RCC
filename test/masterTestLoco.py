#!/usr/bin/python3
# 
# Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
# Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
# For more details go to https://github.com/vova-tymosh/RCC
# 
# The above copyright notice shall be included in all
# copies or substantial portions of the Software.
# 

import sys
import time
import logging
import argparse
from infra import *
from test_mb import tests_mb
from test_storage import tests_storage
from test_mqtt import tests_mq
from test_nrf import tests_nrf

# def test_boot():
#     s = SerialComm.openPort()
#     s.write('!')
#     time.sleep(1)
#     del s
#     return (True, 'Test Boot, bring device to known state')


def runTests(listOfTests):
    w = 80
    for test in listOfTests:
        result, name = test()
        if result:
            print(name + 'ok'.rjust(w - len(name), '.'))
        else:
            print(name + 'FAIL'.rjust(w - len(name), '.'))


if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s',
                        filename='rcc_test.log',
                        filemode='a')
    logging.error('*** Test Start ***')

    parser = argparse.ArgumentParser()
    parser.add_argument('--addr', type=int, help='Loco address')
    parser.add_argument('--port', type=int, help='Loco USB port')
    parser.add_argument('--pad', type=int, help='Add Pad tests, add specified port as Pad port')
    args = parser.parse_args()

    if args.addr:
        LocoSetting.locoaddr = args.addr
        TransportMqtt.locoaddr = LocoSetting.locoaddr
    else:
        print('*** Usage: !!!')
        exit(1)

    l = SerialComm.listPorts()
    if len(l) == 0:
        print('*** No serial ports found')
        exit(1)

    port = args.port if args.port is not None else 0
    if port >= len(l):
        port = 1
    SerialComm.locoPortIndex = port

    if args.pad is None:
        print('*** Skipping Pad tests, one device only:')
        print(f'  Loco: #{port} {l[port].device} - {l[port].description}')
        print()
        tests = []
        # tests += tests_mb
        # tests += tests_storage
        tests += tests_mq
        runTests(tests)
    else:
        pad = args.pad
        print('*** Got two devices:')
        print(f'  Pad:  #0 {l[pad].device} - {l[pad].description}')
        print(f'  Loco: #{port} {l[port].device} - {l[port].description}')
        print()
        tests = []
        # tests += tests_mb
        # tests += tests_storage
        tests += tests_mq
        SerialComm.locoPortIndex = port
        runTests(tests)
        tests = tests_nrf
        SerialComm.locoPortIndex = pad
        runTests(tests)

