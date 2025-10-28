# python3 masterTestLoco.py --addr 2 --port 1 --pad 0 storage mqtt nrf
# python3 masterTestLoco.py --addr 3 --port 2 --pad 0 storage mqtt nrf
# python3 masterTestLoco.py --addr 4 --port 3 --pad 0 storage mqtt nrf

python3 masterTestLoco.py --addr 2 --port 1 --pad 0 mqtt nrf
sleep 1
python3 masterTestLoco.py --addr 3 --port 2 --pad 0 mqtt nrf
sleep 1
python3 masterTestLoco.py --addr 4 --port 3 --pad 0 mqtt nrf

