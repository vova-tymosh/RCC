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


s = None
mq = TransportMqtt()


def test_nrf_start():
    global s
    mq.start()
    s = SerialComm.openPort()
    s.write('CO')
    s.read(f'[Nrf] Updated lits of nodes, count:')
    s.read(f'[Nrf] Set value: heartbeat 1000')
    s.write(f'CN{LocoSetting.locoaddr}')
    r1 = s.read(f'Selected loco address {LocoSetting.locoaddr}: RCC{LocoSetting.locoaddr}')
    r2 = mq.waitForMessage(f'cab/{LocoSetting.locoaddr}/{MQ_HEARTBEAT_VALUES}')
    return (r1 and r2, 'Test NRF Start')

def test_throttle():
    test_name = 'Test NRF Throttle'
    s.write('T65')
    setValueMsg = f'cab/{LocoSetting.locoaddr}/throttle+65'
    testResult = mq.waitForMessage(setValueMsg)
    return (testResult, test_name)

def test_direction_3():
    test_name = 'Test NRF Direction 3'
    s.write('D3')
    setValueMsg = f'cab/{LocoSetting.locoaddr}/direction+NEUTRAL'
    testResult = mq.waitForMessage(setValueMsg)
    return (testResult, test_name)

def test_direction_0():
    test_name = 'Test NRF Direction 0'
    s.write('D0')
    setValueMsg = f'cab/{LocoSetting.locoaddr}/direction+REVERSE'
    testResult = mq.waitForMessage(setValueMsg)
    return (testResult, test_name)

def test_function_set():
    test_name = 'Test NRF Function Set'
    s.write('F12')
    setValueMsg = f'cab/{LocoSetting.locoaddr}/function/2+ON'
    testResult = mq.waitForMessage(setValueMsg)
    if not testResult:
        return (testResult, test_name)
    s.write('F02')
    setValueMsg = f'cab/{LocoSetting.locoaddr}/function/2+OFF'
    testResult = mq.waitForMessage(setValueMsg)
    return (testResult, test_name)
    
def test_function_get():
    test_name = 'Test NRF Function Get'
    s.write('P2')
    setValueMsg = f'cab/{LocoSetting.locoaddr}/function/2+OFF'
    testResult = mq.waitForMessage(setValueMsg)
    return (testResult, test_name)

def test_value():
    test_name = 'Test NRF Velue'
    s.write('Sacceleration:72')
    setValueMsg = f'cab/{LocoSetting.locoaddr}/value/acceleration+72'
    mq.waitForMessage(setValueMsg)
    s.write('Gacceleration')
    getValueRes = f'cab/{LocoSetting.locoaddr}/value/acceleration+72'
    testResult = mq.waitForMessage(getValueRes)
    s.write('Sacceleration:0')
    setValueMsg = f'cab/{LocoSetting.locoaddr}/value/acceleration+0'
    mq.waitForMessage(setValueMsg)
    return (testResult, test_name)

def test_list():
    test_name = 'Test NRF List'
    s.write('L')
    getValueRes = f'cab/{LocoSetting.locoaddr}/{MQ_LIST_VALUE_RES}'
    testResult = mq.waitForMessage(getValueRes)
    logging.error(f"Test List: {testResult}")
    return (testResult, test_name)


def test_nrf_end():
    global s
    del s
    return (True, 'Test NRF End')

tests_nrf = [test_nrf_start,
             test_throttle,
             test_direction_3,
             test_direction_0,
             test_function_set,
             test_function_get,
             test_value,
             test_list,
             test_nrf_end]
