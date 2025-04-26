import time, re
from infra import *

MQ_PREFIX = "cab"
MQ_INTRO = "intro"
MQ_SET_THROTTLE = "throttle";
MQ_SET_DIRECTION = "direction";
MQ_HEARTBEAT_VALUES = "heartbeat/values"
MQ_GET_FUNCTION = "function/get"
MQ_SET_FUNCTION = "function/"
MQ_GET_VALUE = "value/get"
MQ_LIST_VALUE_ASK = "value/list"
MQ_LIST_VALUE_RES = "keys"
MQ_SET_VALUE = "value/"

MQ_DIRECTIONS = ["REVERSE", "FORWARD", "STOP", "NEUTRAL"]
MQ_ON = "ON"
MQ_OFF = "OFF"

MQ_SEPARATOR = ' '


ADDR = 3


mq = TransportMqtt()

def test_mqtt_start():
    mq.start()
    time.sleep(1)
    setValueMsg = f'cab/{ADDR}/value/heartbeat+10000'
    mq.write(setValueMsg)
    mq.loop(setValueMsg)
    return (True, 'Test MQ Start')

def test_throttle():
    test_name = 'Test MQ Throttle'
    setValueMsg = f'cab/{ADDR}/throttle+63'
    mq.write(setValueMsg)
    mq.loop(setValueMsg)
    getValueMsg = f'cab/{ADDR}/value/get+Throttle'
    getValueRes = f'cab/{ADDR}/value/Throttle+63'
    mq.write(getValueMsg)
    testResult = mq.loop(getValueRes)
    return (testResult, test_name)

def _do_test_direction(dircode, dirmsg):
    setDirectionMsg = f'cab/{ADDR}/direction+{dirmsg}'
    mq.write(setDirectionMsg)
    mq.loop(setDirectionMsg)
    hb = mq.waitForHearbeat(f'cab/{ADDR}/{MQ_HEARTBEAT_VALUES}')
    testResult = False
    if hb:
        hb = hb.split(MQ_SEPARATOR)
        bitstate = int(hb[2]) % (2**32)
        testResult = dircode == (bitstate >> 30)
    return testResult

def _test_direction(dircode):
    direction = MQ_DIRECTIONS[dircode]
    test_name = f'Test MQ Direction {direction}'
    testResult = _do_test_direction(dircode, dircode)
    testResult = testResult and _do_test_direction(dircode, direction)
    return (testResult, test_name)

def test_direction_start():
    setValueMsg = f'cab/{ADDR}/value/heartbeat+100'
    mq.write(setValueMsg)
    testResult = mq.loop(setValueMsg)
    return (testResult, 'Test MQ fast heartbeat')

def test_direction_0():
    return _test_direction(0)

def test_direction_1():
    return _test_direction(1)

def test_direction_2():
    return _test_direction(2)

def test_direction_3():
    return _test_direction(3)

def test_direction_end():
    setValueMsg = f'cab/{ADDR}/value/heartbeat+1000'
    mq.write(setValueMsg)
    testResult = mq.loop(setValueMsg)
    return (testResult, 'Test MQ normal heartbeat')

def test_function():
    test_name = 'Test MQ Function'
    function_id = 1
    setFunctionMsg = f'cab/{ADDR}/function/{function_id}+{MQ_ON}'
    mq.write(setFunctionMsg)
    mq.loop(setFunctionMsg)
    getFunctionMsg = f'cab/{ADDR}/function/get+{function_id}'
    getFunctionRes = f'cab/{ADDR}/function/{function_id}+{MQ_ON}'
    mq.write(getFunctionMsg)
    testResult = mq.loop(getFunctionRes)
    if not testResult:
        return (False, f"{test_name} failed for function {function_id} ON")

    setFunctionMsg = f'cab/{ADDR}/function/{function_id}+{MQ_OFF}'
    mq.write(setFunctionMsg)
    mq.loop(setFunctionMsg)
    getFunctionMsg = f'cab/{ADDR}/function/get+{function_id}'
    getFunctionRes = f'cab/{ADDR}/function/{function_id}+{MQ_OFF}'
    mq.write(getFunctionMsg)
    testResult = mq.loop(getFunctionRes)
    if not testResult:
        return (False, f"{test_name} failed for function {function_id} OFF")

    return (True, test_name)

def test_value():
    test_name = 'Test MQ Value'
    setValueMsg = f'cab/{ADDR}/value/acceleration+63'
    mq.write(setValueMsg)
    mq.loop(setValueMsg)
    getValueMsg = f'cab/{ADDR}/value/get+acceleration'
    getValueRes = f'cab/{ADDR}/value/acceleration+63'
    mq.write(getValueMsg)
    testResult = mq.loop(getValueRes)
    setValueMsg = f'cab/{ADDR}/value/acceleration+0'
    mq.write(setValueMsg)
    return (testResult, test_name)

def test_list():
    test_name = 'Test MQ List'
    setValueMsg = f'cab/{ADDR}/{MQ_LIST_VALUE_ASK}+ '
    mq.write(setValueMsg)
    mq.loop(setValueMsg)
    getValueRes = f'cab/{ADDR}/{MQ_LIST_VALUE_RES}'
    testResult = mq.loop(getValueRes)
    return (testResult, test_name)

def test_mqtt_end():
    setValueMsg = f'cab/{ADDR}/value/heartbeat+1000'
    mq.write(setValueMsg)
    mq.loop(setValueMsg)
    return (True, 'Test MQ End')

tests_mq = [test_mqtt_start, test_throttle, test_direction_start, 
            test_direction_1, test_direction_2, test_direction_3, test_direction_0, 
            test_direction_end, test_function, test_value, test_list, test_mqtt_end]

