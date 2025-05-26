import time
from infra import *

s = None

def _test_storage(idx, test_name):
    writeSerial(s, f'CT{idx}')
    r = readSerial(s, 'ok')
    if r != 'ok':
        print(f'\t{r}')
        return (False, test_name)
    return (True, test_name)



def test_storage_start():
    global s
    s = openSerial()
    return (True, 'Test Storage Start')

def test_storage00():
    return _test_storage(0, 'Test Storage 00, write/read with extra size')

def test_storage01():
    return _test_storage(1, 'Test Storage 01, read only')

def test_storage02():
    return _test_storage(2, 'Test Storage 02, read multiple')

def test_storage03():
    return _test_storage(3, 'Test Storage 03, size and offset')

def test_storage04():
    return _test_storage(4, 'Test Storage 04, offset bigger than size')

def test_storage05():
    return _test_storage(5, 'Test Storage 05, re-write')

def test_storage06():
   return  _test_storage(6, 'Test Settings 06, setting create')

def test_storage07():
   return  _test_storage(7, 'Test Settings 07, setting read')

def test_storage08():
   return  _test_storage(8, 'Test Settings 08, settings re-write')

def test_storage09():
   return  _test_storage(9, 'Test Settings 09, reading defaults')

def test_storage10():
   return  _test_storage(10, 'Test Settings 10, cache')

def test_storage11():
   return  _test_storage(11, 'Test Storage 11, exists')

def test_storage12():
   return  _test_storage(12, 'Test Storage 12, clean and validation')

def test_storage_end():
    s.close()
    return (True, 'Test Storage End')


tests_storage = [test_storage_start, 
                 test_storage00, test_storage01, test_storage02, test_storage03, 
                 test_storage04, test_storage05, test_storage06, test_storage07, 
                 test_storage08, test_storage09, test_storage10, test_storage11,
                 test_storage12,
                 test_storage_end]

