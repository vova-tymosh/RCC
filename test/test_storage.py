import time
from infra import *

s = None

def _test_storage(idx, test_name):
    writeSerial(s, f'CT{idx}')
    r = readSerial(s, 'ok')
    return (r, test_name)

testStr1 = "1234567890abcd"
testStr2 = "ABCDEFGHIJKLMNO"

def test_storage_start():
    global s
    s = openSerial()
    return (True, 'Test Storage Start')

def test_storage_normal():
    test_name = 'Test Storage, write/read'
    writeSerial(s, f'W/test01:{testStr1}')
    r1 = readSerial(s, 'Write: /test01')
    writeSerial(s, f'R/test01')
    r2 = readSerial(s, f'/test01:{testStr1}')
    return (r1 and r2, test_name)

def test_storage_read_only():
    test_name = 'Test Storage, read only'
    writeSerial(s, f'R/test01')
    r2 = readSerial(s, f'/test01:{testStr1}')
    return (r2, test_name)

def test_storage_re_write():
    test_name = 'Test Storage, re-write'
    writeSerial(s, f'W/test01:{testStr2}')
    r1 = readSerial(s, 'Write: /test01')
    writeSerial(s, f'R/test01')
    r2 = readSerial(s, f'/test01:{testStr2}')
    return (r1 and r2, test_name)

def test_setting_create():
    test_name = 'Test Settings, create'
    writeSerial(s, f'CCtest05:{testStr1}')
    r1 = readSerial(s, f'test05:{testStr1}')
    writeSerial(s, f'Gtest05')
    r2 = readSerial(s, f'test05:{testStr1}')
    return (r1 and r2, test_name)

def test_setting_read_only():
    test_name = 'Test Settings, read'
    writeSerial(s, f'Gtest05')
    r2 = readSerial(s, f'test05:{testStr1}')
    return (r2, test_name)

def test_setting_re_write():
    test_name = 'Test Settings, re-write'
    writeSerial(s, f'Stest05:{testStr2}')
    r1 = readSerial(s, f'test05:{testStr2}')
    writeSerial(s, f'Gtest05')
    r2 = readSerial(s, f'test05:{testStr2}')
    return (r1 and r2, test_name)

def test_setting_defaults():
    test_name = 'Test Settings, reading defaults'
    writeSerial(s, f'Gloconame')
    r2 = readSerial(s, f'loconame:RCC')
    return (r2, test_name)

def test_storage_version():
    test_name = 'Test Storage, version missmatch'
    writeSerial(s, 'Sloconame:something')
    r1 = readSerial(s, 'loconame:something')
    writeSerial(s, 'Gloconame')
    r2 = readSerial(s, 'loconame:something') 
    writeSerial(s, 'CD')
    r3 = readSerial(s, 'Clear')    
    writeSerial(s, f'Gloconame')
    r4 = readSerial(s, f'loconame:RCC')
    r = r1 and r2 and r3 and r4  
    return (r, test_name)

def test_file_append():
    test_name = 'Test Stoarge, append 0.5kB'
    buffer = b'01234567'*32
    l = len(buffer)
    t = 0
    r = True
    for i in range(2):
        writeSerial(s, f'A/test10:{l}')
        time.sleep(0.1)
        writeSerial(s, buffer)
        t += l
        r = r and readSerial(s, f'Append bytes: {l}')
    writeSerial(s, f'Z/test10')
    r = r and readSerial(s, f'Size of /test10: {t}')
    return (r, test_name)


def test_storage00():
    return _test_storage(0, 'Test Storage 00, read multiple')

def test_storage01():
    return _test_storage(1, 'Test Storage 01, size and offset')

def test_storage02():
    return _test_storage(2, 'Test Storage 02, offset bigger than size')

def test_storage03():
    return _test_storage(3, 'Test Storage 03, cache')

def test_storage04():
    return _test_storage(4, 'Test Storage 04, exists')


def test_storage_end():
    s.close()
    return (True, 'Test Storage End')


tests_storage = [test_storage_start,
    test_storage_normal,
    test_storage_read_only,
    test_storage_re_write,
    # test_file_append,
    test_setting_create,
    test_setting_read_only,
    test_setting_re_write,
    test_setting_defaults,
    test_storage_version,
    test_storage00,
    test_storage01,
    test_storage02,
    test_storage03,
    test_storage04,
    test_storage_end]

