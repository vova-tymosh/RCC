#!/usr/bin/python3

import time
import logging
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

tests = []
# tests += [test_boot]
# tests += tests_mb
tests += tests_storage
tests += tests_mq





if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s',
                        filename='rcc_test.log',
                        filemode='a')
    logging.error('*** Test Start ***')

    l = SerialComm.listPorts()
    if len(l) == 0:
        print('*** No serial ports found')
        exit(1)
    elif len(l) == 1:
        print('*** Got one port, skipping Pad tests')
    else:
        print('*** Got two ports, assigning as following:')
        print(f'  Pad:  {l[0].device} - {l[0].description}')
        print(f'  Loco: {l[1].device} - {l[1].description}')
        tests += tests_nrf
    print()

    w = 80
    for test in tests:
        result, name = test()
        if result:
            print(name + 'ok'.rjust(w - len(name), '.'))
        else:
            print(name + 'FAIL'.rjust(w - len(name), '.'))
