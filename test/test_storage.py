import time
from infra import *

s = None

def _test_storage(idx, test_name):
    writeSerial(s, f'CT{idx}')
    r = readSerial(s).strip()
    if r != 'ok':
        print(f'\t{r}')
        return (False, test_name)
    return (True, test_name)



def test_storage_start():
    global s
    s = openSerial()
    return (True, 'Test Storage Start')

def test_storage00():
    return _test_storage(0, 'Test Storage, write/read with extra size')

def test_storage01():
    return _test_storage(1, 'Test Storage, read only')

def test_storage02():
    return _test_storage(2, 'Test Storage, read multiple')

def test_storage03():
    return _test_storage(3, 'Test Storage, size and offset')

def test_storage04():
    return _test_storage(4, 'Test Settings, offset bigger than size')

def test_storage05():
    return _test_storage(5, 'Test Settings, re-write')

def test_storage06():
   return  _test_storage(6, 'Test Settings, setting create')

def test_storage07():
   return  _test_storage(7, 'Test Settings, setting read')

def test_storage08():
   return  _test_storage(8, 'Test Settings, settings re-write')

def test_storage09():
   return  _test_storage(9, 'Test Settings, reading defaults')

def test_storage10():
   return  _test_storage(10, 'Test Settings, cache')

def test_storage_end():
    s.close()
    return (True, 'Test Storage End')


tests_storage = [test_storage_start, 
                 test_storage00, test_storage01, test_storage02, test_storage03, 
                 test_storage04, test_storage05, test_storage06, test_storage07, 
                 test_storage08, test_storage09, test_storage10,
                 test_storage_end]

