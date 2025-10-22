#!/usr/bin/python3
#
# Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
# Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
# For more details go to https://github.com/vova-tymosh/RCC
#
# The above copyright notice shall be included in all
# copies or substantial portions of the Software.
#

import sys
import time
import logging
import argparse
from infra import *
from test_mb import tests_mb
from test_storage import tests_storage
from test_mqtt import tests_mq
from test_nrf import tests_nrf

# Available test suites
AVAILABLE_TESTS = {
    'mb': tests_mb,
    'storage': tests_storage,
    'mqtt': tests_mq,
    'nrf': tests_nrf
}

def print_usage():
    """Print detailed usage information"""
    print("RCC Test Suite - Remote Command and Control Testing Framework")
    print()
    print("USAGE:")
    print("  python3 masterTestLoco.py --addr <loco_address> [OPTIONS] [TESTS...]")
    print()
    print("REQUIRED ARGUMENTS:")
    print("  --addr <address>     Locomotive address")
    print()
    print("OPTIONAL ARGUMENTS:")
    print("  --port <port_index>  USB port index for locomotive (default: 0)")
    print("  --pad <port_index>   USB port index for pad (enables NRF tests)")
    print("  --help, -h          Show this help message")
    print()
    print("AVAILABLE TESTS:")
    print("  mb                  Motherboard tests")
    print("  storage             Storage system tests")
    print("  mqtt                MQTT protocol tests (WiFi)")
    print("  nrf                 NRF protocol tests (wireless)")
    print()
    print("NOTES:")
    print("  - If no test names are specified, all available tests will run")
    print("  - NRF tests require --pad argument to specify pad port")
    print("  - Debug logs are captured in 'rcc_test.log'")

def runTests(listOfTests, test_name=""):
    """Run a list of tests with formatted output"""
    if test_name:
        print(f"\n=== Running {test_name.upper()} Tests ===")

    w = 80
    passed = 0
    failed = 0
    for test in listOfTests:
        try:
            result, name = test()
            if result:
                print(name + 'OK'.rjust(w - len(name), '.'))
                passed += 1
            else:
                print(name + 'FAIL'.rjust(w - len(name), '.'))
                failed += 1
        except Exception as e:
            print(f"ERROR in {test.__name__}: {e}")
            failed += 1

    print(f"\nResults: {passed} passed, {failed} failed")
    return failed == 0

def get_test_suites(test_names):
    """Get test suites based on provided names"""
    if not test_names:
        # Return all tests if none specified
        return list(AVAILABLE_TESTS.keys())

    # Validate test names
    invalid_tests = [name for name in test_names if name not in AVAILABLE_TESTS]
    if invalid_tests:
        print(f"ERROR: Unknown test(s): {', '.join(invalid_tests)}")
        print(f"Available tests: {', '.join(AVAILABLE_TESTS.keys())}")
        return None

    return test_names

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s %(message)s',
                        filename='rcc_test.log',
                        filemode='a')
    logging.error('*** Test Start ***')

    parser = argparse.ArgumentParser(
        description='RCC Test Suite - Remote Command and Control Testing Framework',
        add_help=False
    )
    parser.add_argument('--addr', type=int, help='Locomotive address (required)')
    parser.add_argument('--port', type=int, help='USB port index for locomotive (default: 0)')
    parser.add_argument('--pad', type=int, help='USB port index for pad (enables NRF tests)')
    parser.add_argument('--help', '-h', action='store_true', help='Show help message')
    parser.add_argument('tests', nargs='*', help='Test names to run (mb, storage, mqtt, nrf)')

    args = parser.parse_args()
    if args.help:
        print_usage()
        sys.exit(0)

    l = SerialComm.listPorts()
    if len(l) == 0:
        print('ERROR: No serial ports found')
        sys.exit(1)

    if not args.addr:
        print("ERROR: --addr argument is required")
        print("Use --help for usage information")
        print()
        print("Available USB ports:")
        for i, p in enumerate(l):
            print(f"  #{i} {p.device} - {p.description}")
        sys.exit(1)

    LocoSetting.locoaddr = args.addr
    TransportMqtt.locoaddr = LocoSetting.locoaddr

    port = args.port if args.port is not None else 0
    if port >= len(l):
        print(f"WARNING: Port {port} not available, using port 0")
        port = 0
    SerialComm.locoPortIndex = port

    test_suites = get_test_suites(args.tests)
    if test_suites is None:
        sys.exit(1)

    if args.pad is not None:
        if args.pad >= len(l):
            print(f"ERROR: Pad port {args.pad} not available")
            sys.exit(1)

    print()

    # Run tests
    all_passed = True
    for test_name in test_suites:
        if test_name == 'nrf' and args.pad is None:
            print(f"SKIPPING {test_name.upper()} tests: --pad argument required for NRF tests")
            continue
        if test_name == 'nrf' and args.pad is not None:
            SerialComm.locoPortIndex = args.pad
        else:
            SerialComm.locoPortIndex = port

        test_suite = AVAILABLE_TESTS[test_name]
        result = runTests(test_suite, test_name)
        all_passed = all_passed and result

    print("\n" + "="*80)
    if all_passed:
        print("ALL TESTS PASSED!")
    else:
        print("SOME TESTS FAILED - Check output above for details")
    print("="*80)

