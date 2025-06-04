import re
import serial
import serial.tools.list_ports
import time
import queue
import select
import sys
import logging
import paho.mqtt.client as mqtt
from paho.mqtt.subscribeoptions import SubscribeOptions

MQTT_NODE_NAME = 'RCC_Test'
MQTT_BROKER = '192.168.20.61'
MQ_PREFIX = "cab"
MQ_MESSAGE = re.compile("cab/(.*?)/(.*)")

MQ_PREFIX = "cab"
MQ_INTRO = "intro"
MQ_SET_THROTTLE = "throttle";
MQ_SET_DIRECTION = "direction";
MQ_HEARTBEAT_VALUES = "heartbeat/values"
MQ_GET_FUNCTION = "function/get"
MQ_SET_FUNCTION = "function/"
MQ_GET_VALUE = "value/get"
MQ_LIST_VALUE_ASK = "value/list"
MQ_LIST_VALUE_RES = "keys"
MQ_SET_VALUE = "value/"

MQ_DIRECTIONS = ["REVERSE", "FORWARD", "STOP", "NEUTRAL"]
MQ_ON = "ON"
MQ_OFF = "OFF"
MQ_SEPARATOR = ' '

ADDR = 3


def millis():
    return round(time.time() * 1000)

def get_unblocked_input():
    output, _, _ = select.select([sys.stdin], [], [], 0)
    if output:
        return sys.stdin.readline()
    return None


class SerialComm:

    @staticmethod
    def listPorts():
        l = []
        for i in serial.tools.list_ports.comports():
            if i.description != 'n/a':
                l.append(i)
        l.sort()
        return l

    @staticmethod
    def openPort(idx = 0):
        l = SerialComm.listPorts()
        if idx < len(l):
            return SerialComm(serial.Serial(l[idx].device, 115200, timeout=1), l[idx].description)
        return None

    def __init__(self, s, description):
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
            logging.info(f"Read <: '{b}' Need: '{msg}'")
            if msg:
                if msg == b:
                    return True
            elif b:
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
    def __init__(self):
        self.mqttClient = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, MQTT_NODE_NAME)
        self.mqttClient.on_message = self.onReceive
        self.messageQueue = queue.Queue()

    def start(self):
        self.mqttClient.connect(MQTT_BROKER)
        options = SubscribeOptions(qos = 1, noLocal = True)
        self.mqttClient.subscribe(f'{MQ_PREFIX}/#', options=options)

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
