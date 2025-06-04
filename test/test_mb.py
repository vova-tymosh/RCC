import time
from infra import *

s = None

def test_mb_start():
    global s
    s = SerialComm.openPort()
    return (True, 'Test Local Start')

def test_f0_on():
    test_name = 'Test Function 0 ON'
    s.write('F100')
    s.read()
    yn = input('\tIs Yellow light on? (Y/n)')
    return (yn.lower() != 'n', test_name)

def test_f0_blinking():
    test_name = 'Test Function 0 Blinking'
    s.write('F000')
    print('\tIs Yellow light blinking? (Y/n)')
    start = millis()
    flip = True
    while True:
        yn = get_unblocked_input()
        if yn != None:
            break
        if millis() - start > 500:
            s.write('F100' if flip else 'F000')
            flip = not flip
            start = millis()
    s.write('F000')
    s.read()
    return (yn.strip().lower() != 'n', test_name)

def test_f1_blinking():
    test_name = 'Test Function 1 Blinking'
    s.write('F001')
    print('\tIs Blue light blinking? (Y/n)')
    start = millis()
    flip = True
    while True:
        yn = get_unblocked_input()
        if yn != None:
            break
        if millis() - start > 500:
            s.write('F101' if flip else 'F001')
            flip = not flip
            start = millis()
    # F1 is inverted, keep it off (set to 1)
    s.write('F101')
    s.read()
    return (yn.strip().lower() != 'n', test_name)

def motor_blinker():
    s.write('T0')
    s.read()    
    speed = 20
    start = millis()
    yn = False
    while True:
        yn = get_unblocked_input()
        if yn != None:
            break
        if millis() - start > 100:
            if speed >= 200:
                speed = 20
            else:
                speed += 10
            if speed < 100:
                s.write('T%03d'%(speed))
            else:
                s.write('T%03d'%(200 - speed))
            s.read()
            start = millis()
    s.write('T0')
    s.read()
    return yn

def test_motor_forward():
    test_name = 'Test Motor forward'
    print('\tIs White light fading in/out? (Y/n)')
    s.write(f'D1')
    yn = motor_blinker()
    return (yn.strip().lower() != 'n', test_name)

def test_motor_backward():
    test_name = 'Test Motor backward'
    print('\tIs Green light fading in/out? (Y/n)')
    s.write(f'D0')
    yn = motor_blinker()
    return (yn.strip().lower() != 'n', test_name)

def test_motor_bemf():
    test_name = 'Test Motor BEMF'
    s.write('CB')
    data = s.readFloat()
    print(f'\tHas to be non-zero value: {data}')
    return (data > 0, test_name)

def test_voltage():
    test_name = 'Test Battery Level'
    s.write('GBattery')
    data = s.readFloat()
    print(f'\tHas to be non-zero value: {data}V')
    return (data > 0, test_name)

def test_current():
    test_name = 'Test Power Meter Current'
    s.write('GCurrent')
    data = s.readFloat()
    print(f'\tHas to be non-zero value: {data}mA')
    return (data > 0, test_name)

def test_current_with_load():
    test_name = 'Test Current With Load'
    s.write('D1')
    s.read()
    s.write('T100')
    s.read()
    s.write('F100')
    s.read()
    s.write('F001')
    s.read()
    time.sleep(0.2)
    s.write('GCurrent')
    data = s.readFloat()
    s.write('T0')
    s.read()
    s.write('F000')
    s.read()
    s.write('F101')
    s.read()
    print(f'\tHas to be higher than 30: {data}mA')
    return (data > 30, test_name)

def test_mb_end():
    global s
    del s
    return (True, 'Test Local End')

tests_mb = [test_mb_start, 
               test_voltage, test_current, test_current_with_load, test_motor_bemf,
               test_f0_on, test_f0_blinking, test_f1_blinking, test_motor_forward, 
               test_motor_backward, 
               test_mb_end]
