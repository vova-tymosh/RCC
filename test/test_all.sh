#!/bin/bash

# Track if any test failed
FAILED=0

# python3 masterTestLoco.py --addr 2 --port 1 storage || FAILED=1
# python3 masterTestLoco.py --addr 3 --port 2 storage || FAILED=1
# python3 masterTestLoco.py --addr 4 --port 3 storage || FAILED=1

python3 masterTestLoco.py --addr 2 --port 1 --pad 0 mqtt nrf || FAILED=1
python3 masterTestLoco.py --addr 3 --port 2 --pad 0 mqtt nrf || FAILED=1
python3 masterTestLoco.py --addr 4 --port 3 --pad 0 mqtt nrf || FAILED=1

echo
printf -- '%.0s=' {1..80}; echo
if [ $FAILED -eq 0 ]; then
    echo "✓ All tests passed"
else
    echo "✗ Some tests failed"
fi
printf -- '%.0s=' {1..80}; echo
echo
