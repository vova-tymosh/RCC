import time, re
from infra import *



mq = TransportMqtt()

class VerifyHeartbeat:
    def __init__(self, dir):
        self.dir = dir
    def __call__(self, msg):
        if msg.startswith(f'cab/{ADDR}/{MQ_HEARTBEAT_VALUES}'):
            hb = msg.split(MQ_SEPARATOR)
            if len(hb) >= 3:
                bitstate = int(hb[2]) % (2**32)
                dircode = (bitstate >> 30)
                if dircode == self.dir:
                    return True
        return False

def test_mqtt_start():
    mq.start()
    setValueMsg = f'cab/{ADDR}/value/heartbeat+300'
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    return (True, 'Test MQ Start')

def test_throttle():
    test_name = 'Test MQ Throttle'
    setValueMsg = f'cab/{ADDR}/throttle+63'
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    getValueMsg = f'cab/{ADDR}/value/get+Throttle'
    getValueRes = f'cab/{ADDR}/value/Throttle+63'
    mq.write(getValueMsg)
    testResult = mq.waitForMessage(getValueRes)
    return (testResult, test_name)

def _do_test_direction(dircode, dirmsg):
    setDirectionMsg = f'cab/{ADDR}/direction+{dirmsg}'
    mq.write(setDirectionMsg)
    mq.waitForMessage(setDirectionMsg)
    testResult = mq.waitForMessage(VerifyHeartbeat(dircode))
    return testResult

def _test_direction(dircode):
    direction = MQ_DIRECTIONS[dircode]
    test_name = f'Test MQ Direction {direction}'
    testResult = _do_test_direction(dircode, dircode)
    testResult = testResult and _do_test_direction(dircode, direction)
    return (testResult, test_name)

def test_direction_0():
    return _test_direction(0)

def test_direction_1():
    return _test_direction(1)

def test_direction_2():
    return _test_direction(2)

def test_direction_3():
    return _test_direction(3)

def test_function():
    test_name = 'Test MQ Function'
    function_id = 1
    setFunctionMsg = f'cab/{ADDR}/function/{function_id}+{MQ_ON}'
    mq.write(setFunctionMsg)
    mq.waitForMessage(setFunctionMsg)
    getFunctionMsg = f'cab/{ADDR}/function/get+{function_id}'
    getFunctionRes = f'cab/{ADDR}/function/{function_id}+{MQ_ON}'
    mq.write(getFunctionMsg)
    testResult = mq.waitForMessage(getFunctionRes)
    if not testResult:
        return (False, f"{test_name} failed for function {function_id} ON")

    setFunctionMsg = f'cab/{ADDR}/function/{function_id}+{MQ_OFF}'
    mq.write(setFunctionMsg)
    mq.waitForMessage(setFunctionMsg)
    getFunctionMsg = f'cab/{ADDR}/function/get+{function_id}'
    getFunctionRes = f'cab/{ADDR}/function/{function_id}+{MQ_OFF}'
    mq.write(getFunctionMsg)
    testResult = mq.waitForMessage(getFunctionRes)
    if not testResult:
        return (False, f"{test_name} failed for function {function_id} OFF")

    return (True, test_name)

def test_value():
    test_name = 'Test MQ Value'
    setValueMsg = f'cab/{ADDR}/value/acceleration+63'
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    getValueMsg = f'cab/{ADDR}/value/get+acceleration'
    getValueRes = f'cab/{ADDR}/value/acceleration+63'
    mq.write(getValueMsg)
    testResult = mq.waitForMessage(getValueRes)
    setValueMsg = f'cab/{ADDR}/value/acceleration+0'
    mq.write(setValueMsg)
    return (testResult, test_name)

def test_list():
    test_name = 'Test MQ List'
    setValueMsg = f'cab/{ADDR}/{MQ_LIST_VALUE_ASK}+ '
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    getValueRes = f'cab/{ADDR}/{MQ_LIST_VALUE_RES}'
    testResult = mq.waitForMessage(getValueRes)
    return (testResult, test_name)

def test_mqtt_end():
    setValueMsg = f'cab/{ADDR}/value/heartbeat+1000'
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    return (True, 'Test MQ End')

tests_mq = [test_mqtt_start, test_throttle, 
            test_direction_1, test_direction_2, test_direction_3, test_direction_0, 
            test_function, test_value, test_list, test_mqtt_end]
