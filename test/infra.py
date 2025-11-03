# 
# Copyright (c) 2024-2025 Volodymyr "Vova" Tymoshchuk
# Distributed under MIT licence, https://github.com/vova-tymosh/RCC/blob/main/LICENSE
# For more details go to https://github.com/vova-tymosh/RCC
# 
# The above copyright notice shall be included in all
# copies or substantial portions of the Software.
# 
import re
import serial
import serial.tools.list_ports
import time
import queue
import select
import sys, os
import logging
import paho.mqtt.client as mqtt
from paho.mqtt.subscribeoptions import SubscribeOptions

MQTT_NODE_NAME = 'RCC_Test'
MQTT_BROKER = '192.168.20.62'
MQ_PREFIX = "cab"
MQ_MESSAGE = re.compile("cab/(.*?)/(.*)")

MQ_PREFIX = "cab"
MQ_INTRO = "intro"
MQ_SET_THROTTLE = "throttle";
MQ_SET_DIRECTION = "direction";
MQ_HEARTBEAT = "heartbeat"
MQ_HEARTBEAT_VALUES = "heartbeat/values"
MQ_GET_FUNCTION = "function/get"
MQ_SET_FUNCTION = "function/"
MQ_GET_VALUE = "value/get"
MQ_LIST_VALUE_REQ = "value/list/req"
MQ_LIST_VALUE_RES = "value/list"
MQ_SET_VALUE = "value/"

MQ_DIRECTIONS = ["REVERSE", "FORWARD", "STOP", "NEUTRAL"]
MQ_ON = "ON"
MQ_OFF = "OFF"
MQ_SEPARATOR = ','


def millis():
    return round(time.time() * 1000)

def get_unblocked_input():
    output, _, _ = select.select([sys.stdin], [], [], 0)
    if output:
        return sys.stdin.readline()
    return None

def updateSettings(port):
    data = []
    try:
        if os.path.exists('custom.cfg'):
            file = open('custom.cfg')
        else:
            file = open('default.cfg')
        data = file.read()
        data = data.splitlines()
    except:
        pass

    rightLine = re.compile('\s*(?P<key>[a-zA-Z0-9_-]+):(?P<value>[a-zA-Z0-9_-]+\s*)')
    for i in data:
        m = rightLine.match(i)
        if m:
            line = i
            key = m.group("key")
            if key == "functionNames":
                continue
            if key == "loconame":
                line = f'loconame:RCC{LocoSetting.locoaddr}'
            elif key == "locoaddr":
                line = f'locoaddr:{LocoSetting.locoaddr}'
            port.write(f'S{line}')
            port.read(f'{line}')

class LocoSetting:
    locoaddr = 3

class SerialComm:
    locoPortIndex = 0

    @staticmethod
    def listPorts():
        l = []
        for i in serial.tools.list_ports.comports():
            if i.description != 'n/a':
                l.append(i)
        l.sort()
        return l

    @staticmethod
    def openPort():
        idx = SerialComm.locoPortIndex
        l = SerialComm.listPorts()
        if idx < len(l):
            return SerialComm(idx, serial.Serial(l[idx].device, 115200, timeout=1), l[idx].description)
        return None

    def __init__(self, idx, s, description):
        self.idx = idx
        self.s = s
        self.description = description

    def __del__(self):
        self.s.close()

    def write(self, data):
        if type(data) is not bytes:
            data = data.encode('utf-8')
        if len(data) <= 40:
            logging.info(f"Write >: {data}")
        else:
            logging.info(f"Write >:[{len(data)}] {data[:40]}...")
        self.s.write(data)
        self.s.flush()

    def read(self, msg = None):
        buffer = ''
        for i in range(5):
            b = self.s.readline().decode('utf-8').strip()
            if msg:
                if msg == b:
                    logging.info(f"Read <: '{b}'. Ack")
                    return True
                else:
                    logging.info(f"Read <: '{b}', but need: '{msg}'")
            elif b:
                logging.info(f"Read <: '{b}', no validation")
                return b
        return False

    def readFloat(self):
        data = self.read()
        try:
            data = data.split()[0]
            return float(data)
        except:
            pass
        return 0.0


class TransportMqtt:
    locoaddr = 3

    def __init__(self):
        self.mqttClient = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, MQTT_NODE_NAME)
        self.mqttClient.on_message = self.onReceive
        self.messageQueue = queue.Queue()

    def start(self):
        self.mqttClient.connect(MQTT_BROKER)
        options = SubscribeOptions(qos = 1, noLocal = True)
        self.mqttClient.subscribe(f'cab/{TransportMqtt.locoaddr}/#', options=options)

    def waitForMessage(self, message = None):
        for i in range(5):
            self.mqttClient.loop(timeout=1.0)
            if message:
                while not self.messageQueue.empty():
                    m = self.messageQueue.get()
                    if type(message) == str and m.startswith(message):
                        return m.split('+', 1)[1]
                    elif callable(message) and message(m):
                        return True
        return None

    def write(self, message, retain = False):
        logging.info(f"[MQ] >: {message}")
        topic, message = message.split('+', 1)
        self.mqttClient.publish(topic, message, retain)

    def onReceive(self, client, userdata, msg):
        topic = MQ_MESSAGE.match(msg.topic)
        if (topic is None):
            return
        message = str(msg.payload, 'UTF-8')

        logging.info(f"[MQ] <: {msg.topic}+{message}")
        self.messageQueue.put(msg.topic + '+' + message)
