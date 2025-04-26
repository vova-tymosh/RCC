#!/usr/bin/python3

import time
import logging
from test_local import tests_local
from test_mqtt import tests_mq
from test_nrf_local import tests_nrf

tests = []
tests += tests_mq
# tests += tests_local
# tests += tests_nrf

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
