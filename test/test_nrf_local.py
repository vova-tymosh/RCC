import time, re
from infra import *



ADDR = 3
s = None
mq = TransportMqtt()


def test_nrf_start():
    global s
    s = openSerial(1)
    mq.start()
    mq.loop()
    return (True, 'Test NRF Start')

def test_throttle():
    test_name = 'Test NRF Throttle'
    writeSerial(s, 'T65')
    printSerial(s)
    setValueMsg = f'cab/{ADDR}/throttle+65'
    testResult = mq.loop(setValueMsg)
    return (testResult, test_name)

def test_direction_3():
    test_name = 'Test NRF Direction 3'
    writeSerial(s, 'D3')
    setValueMsg = f'cab/{ADDR}/direction+NEUTRAL'
    testResult = mq.loop(setValueMsg)
    return (testResult, test_name)

def test_direction_0():
    test_name = 'Test NRF Direction 0'
    writeSerial(s, 'D0')
    setValueMsg = f'cab/{ADDR}/direction+REVERSE'
    testResult = mq.loop(setValueMsg)
    return (testResult, test_name)

def test_function():
    test_name = 'Test NRF Function'
    writeSerial(s, 'F12')
    setValueMsg = f'cab/{ADDR}/function/2+ON'
    testResult = mq.loop(setValueMsg)
    if not testResult:
        return (testResult, test_name)
    writeSerial(s, 'F02')
    setValueMsg = f'cab/{ADDR}/function/2+OFF'
    testResult = mq.loop(setValueMsg)
    return (testResult, test_name)
    
def test_function_get():
    test_name = 'Test NRF Function Get'
    writeSerial(s, 'P2')
    setValueMsg = f'cab/{ADDR}/function/2+OFF'
    testResult = mq.loop(setValueMsg)
    return (testResult, test_name)

def test_nrf_end():
    global s
    s.close()
    return (True, 'Test NRF End')

tests_nrf = [test_nrf_start, test_throttle, test_direction_3, test_direction_0, test_nrf_end]

