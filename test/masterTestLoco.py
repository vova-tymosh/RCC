#!/usr/bin/python3

import time
import logging
from infra import *
from test_local import tests_local
from test_storage import tests_storage
from test_mqtt import tests_mq
from test_nrf_local import tests_nrf

def test_boot():
    s = openSerial()
    writeSerial(s, '!')
    s.close()
    time.sleep(1)
    return (True, 'Test Boot, bring device to known state')

tests = []
# tests += [test_boot]
# tests += tests_mq
# tests += tests_nrf
# tests += tests_local
tests += tests_storage

logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s %(message)s',
                    filename='rcc_test.log',
                    filemode='a')
logging.error('*** Test Start ***')


if __name__ == '__main__':
    w = 80
    for test in tests:
        result, name = test()
        if result:
            print(name + 'ok'.rjust(w - len(name), '.'))
        else:
            print(name + 'FAIL'.rjust(w - len(name), '.'))
