# 
# Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
# Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
# For more details go to https://github.com/vova-tymosh/RCC
# 
# The above copyright notice shall be included in all
# copies or substantial portions of the Software.
# 
import time, re
from infra import *



mq = TransportMqtt()

class VerifyHeartbeat:
    def __init__(self, dir):
        self.dir = dir
    def __call__(self, msg):
        if msg.startswith(f'cab/{mq.locoaddr}/{MQ_HEARTBEAT_VALUES}'):
            hb = msg.split(MQ_SEPARATOR)
            if len(hb) >= 3:
                bitstate = int(hb[2]) % (2**32)
                dircode = (bitstate >> 30)
                if dircode == self.dir:
                    return True
        return False

def test_mqtt_start():
    mq.start()
    setValueMsg = f'cab/{mq.locoaddr}/value/heartbeat+300'
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    return (True, 'Test MQ Start')

def test_throttle():
    test_name = 'Test MQ Throttle'
    setValueMsg = f'cab/{mq.locoaddr}/throttle+63'
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    getValueMsg = f'cab/{mq.locoaddr}/value/get+Throttle'
    getValueRes = f'cab/{mq.locoaddr}/value/Throttle+63'
    mq.write(getValueMsg)
    testResult = mq.waitForMessage(getValueRes)
    return (testResult, test_name)

def _do_test_direction(dircode, dirmsg):
    setDirectionMsg = f'cab/{mq.locoaddr}/direction+{dirmsg}'
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
    setFunctionMsg = f'cab/{mq.locoaddr}/function/{function_id}+{MQ_ON}'
    mq.write(setFunctionMsg)
    mq.waitForMessage(setFunctionMsg)
    getFunctionMsg = f'cab/{mq.locoaddr}/function/get+{function_id}'
    getFunctionRes = f'cab/{mq.locoaddr}/function/{function_id}+{MQ_ON}'
    mq.write(getFunctionMsg)
    testResult = mq.waitForMessage(getFunctionRes)
    if not testResult:
        return (False, f"{test_name} failed for function {function_id} ON")

    setFunctionMsg = f'cab/{mq.locoaddr}/function/{function_id}+{MQ_OFF}'
    mq.write(setFunctionMsg)
    mq.waitForMessage(setFunctionMsg)
    getFunctionMsg = f'cab/{mq.locoaddr}/function/get+{function_id}'
    getFunctionRes = f'cab/{mq.locoaddr}/function/{function_id}+{MQ_OFF}'
    mq.write(getFunctionMsg)
    testResult = mq.waitForMessage(getFunctionRes)
    if not testResult:
        return (False, f"{test_name} failed for function {function_id} OFF")

    return (True, test_name)

def test_value():
    test_name = 'Test MQ Value'
    setValueMsg = f'cab/{mq.locoaddr}/value/acceleration+63'
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    getValueMsg = f'cab/{mq.locoaddr}/value/get+acceleration'
    getValueRes = f'cab/{mq.locoaddr}/value/acceleration+63'
    mq.write(getValueMsg)
    testResult = mq.waitForMessage(getValueRes)
    setValueMsg = f'cab/{mq.locoaddr}/value/acceleration+0'
    mq.write(setValueMsg)
    return (testResult, test_name)

def test_list():
    test_name = 'Test MQ List'
    setValueMsg = f'cab/{mq.locoaddr}/{MQ_LIST_VALUE_REQ}+ '
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    getValueRes = f'cab/{mq.locoaddr}/{MQ_LIST_VALUE_RES}'
    testResult = mq.waitForMessage(getValueRes)
    return (testResult, test_name)

def test_function_name():
    test_name = 'Test MQ Function Name'
    function_id = 20
    function_name = 'testUniq22'
    
    # Set function name
    setNameMsg = f'cab/{mq.locoaddr}/function/name/{function_id}+{function_name}'
    mq.write(setNameMsg)
    mq.waitForMessage(setNameMsg)
    
    # Test function control by name
    setFunctionMsg = f'cab/{mq.locoaddr}/function/{function_name}+{MQ_ON}'
    mq.write(setFunctionMsg)
    mq.waitForMessage(setFunctionMsg)
    
    # Verify function state by ID
    getFunctionMsg = f'cab/{mq.locoaddr}/function/get+{function_id}'
    getFunctionRes = f'cab/{mq.locoaddr}/function/{function_id}+{MQ_ON}'
    mq.write(getFunctionMsg)
    testResult = mq.waitForMessage(getFunctionRes)
    
    if not testResult:
        return (False, f"{test_name} failed - function by name not working")
    
    # Test function control by name OFF
    setFunctionMsg = f'cab/{mq.locoaddr}/function/{function_name}+{MQ_OFF}'
    mq.write(setFunctionMsg)
    mq.waitForMessage(setFunctionMsg)
    
    # Verify function state by name
    getFunctionMsg = f'cab/{mq.locoaddr}/function/get+{function_name}'
    getFunctionRes = f'cab/{mq.locoaddr}/function/{function_name}+{MQ_OFF}'
    mq.write(getFunctionMsg)
    testResult = mq.waitForMessage(getFunctionRes)
    
    return (testResult, test_name)

def test_function_list():
    test_name = 'Test MQ Function List'
    
    # Set up some function names first
    function_names = [(0, 'headlight'), (1, 'bell'), (2, 'horn')]
    
    for func_id, func_name in function_names:
        setNameMsg = f'cab/{mq.locoaddr}/function/name/{func_id}+{func_name}'
        mq.write(setNameMsg)
        mq.waitForMessage(setNameMsg)
    
    # Request function list
    listReqMsg = f'cab/{mq.locoaddr}/function/list/req+ '
    mq.write(listReqMsg)
    mq.waitForMessage(listReqMsg)
    
    # Wait for function list response
    listResMsg = f'cab/{mq.locoaddr}/function/list'
    testResult = mq.waitForMessage(listResMsg)
    
    return (testResult, test_name)

def test_mqtt_end():
    setValueMsg = f'cab/{mq.locoaddr}/value/heartbeat+1000'
    mq.write(setValueMsg)
    mq.waitForMessage(setValueMsg)
    return (True, 'Test MQ End')

tests_mq = [test_mqtt_start, 
            test_throttle,
            test_direction_1,
            test_direction_2,
            test_direction_3,
            test_direction_0,
            test_function,
            test_function_name,
            test_function_list,
            test_value,
            test_list,
            test_mqtt_end]
