import time, re
from infra import *
import paho.mqtt.client as mqtt
from paho.mqtt.subscribeoptions import SubscribeOptions

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

MQTT_NODE_NAME = 'RCC_Test'
MQTT_BROKER = '192.168.20.61'

ADDR = 3


class TransportMqtt:
    def __init__(self):
        self.mqttClient = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2, MQTT_NODE_NAME)
        self.mqttClient.on_message = self.onReceive
        self.message = ""

    def start(self):
        self.mqttClient.connect(MQTT_BROKER)
        options = SubscribeOptions(qos = 1, noLocal = True)
        self.mqttClient.subscribe(f'{MQ_PREFIX}/#', options=options)

    def loop(self, message = None):
        for i in range(5):
            self.mqttClient.loop(timeout=1.0)
            if message and self.message.startswith(message):
                self.message = ""
                return True
        return False

    def waitForHearbeat(self):
        for i in range(10):
            self.mqttClient.loop(timeout=1.0)
            if self.message.startswith(f'cab/{ADDR}/{MQ_HEARTBEAT_VALUES}'):
                return self.message.split('+', 1)[1]

    def write(self, message, retain = False):
        logging.info(f"[MQ] >: {message}")
        topic, message = message.split('+', 1)
        self.mqttClient.publish(topic, message, retain)

    def onReceive(self, client, userdata, msg):
        topic = MQ_MESSAGE.match(msg.topic)
        if (topic is None):
            return
        message = str(msg.payload, 'UTF-8')

        logging.info(f"[MQ] <: {msg.topic} {message}")
        self.message = msg.topic + '+' + message


mq = TransportMqtt()

def test_mqtt_start(s):
    mq.start()
    return (True, 'Test MQ Setup')

def test_throttle(s):
    test_name = 'Test MQ Throttle'
    setValueMsg = f'cab/{ADDR}/throttle+63'
    mq.write(setValueMsg)
    mq.loop(setValueMsg)
    getValueMsg = f'cab/{ADDR}/value/get+Throttle'
    getValueRes = f'cab/{ADDR}/value/Throttle+63'
    mq.write(getValueMsg)
    testResult = mq.loop(getValueRes)
    return (testResult, test_name)

def _do_test_direction(dircode, dirmsg):
    setDirectionMsg = f'cab/{ADDR}/direction+{dirmsg}'
    mq.write(setDirectionMsg)
    mq.loop(setDirectionMsg)
    hb = mq.waitForHearbeat()
    testResult = False
    if hb:
        hb = hb.split(MQ_SEPARATOR)
        bitstate = int(hb[2]) % (2**32)
        testResult = dircode == (bitstate >> 30)
    return testResult

def _test_direction(dircode):
    direction = MQ_DIRECTIONS[dircode]
    test_name = f'Test MQ Direction {direction}'
    testResult = _do_test_direction(dircode, dircode)
    testResult = testResult and _do_test_direction(dircode, direction)
    return (testResult, test_name)

def test_direction_0(s):
    return _test_direction(0)

def test_direction_1(s):
    return _test_direction(1)

def test_direction_2(s):
    return _test_direction(2)

def test_direction_3(s):
    return _test_direction(3)


def test_function(s):
    test_name = 'Test MQ Function'
    function_id = 1
    setFunctionMsg = f'cab/{ADDR}/function/{function_id}+{MQ_ON}'
    mq.write(setFunctionMsg)
    mq.loop(setFunctionMsg)
    getFunctionMsg = f'cab/{ADDR}/function/get+{function_id}'
    getFunctionRes = f'cab/{ADDR}/function/{function_id}+{MQ_ON}'
    mq.write(getFunctionMsg)
    testResult = mq.loop(getFunctionRes)
    if not testResult:
        return (False, f"{test_name} failed for function {function_id} ON")

    setFunctionMsg = f'cab/{ADDR}/function/{function_id}+{MQ_OFF}'
    mq.write(setFunctionMsg)
    mq.loop(setFunctionMsg)
    getFunctionMsg = f'cab/{ADDR}/function/get+{function_id}'
    getFunctionRes = f'cab/{ADDR}/function/{function_id}+{MQ_OFF}'
    mq.write(getFunctionMsg)
    testResult = mq.loop(getFunctionRes)
    if not testResult:
        return (False, f"{test_name} failed for function {function_id} OFF")

    return (True, test_name)

def test_value(s):
    test_name = 'Test MQ Value'
    setValueMsg = f'cab/{ADDR}/value/acceleration+63'
    mq.write(setValueMsg)
    mq.loop(setValueMsg)
    getValueMsg = f'cab/{ADDR}/value/get+acceleration'
    getValueRes = f'cab/{ADDR}/value/acceleration+63'
    mq.write(getValueMsg)
    testResult = mq.loop(getValueRes)
    setValueMsg = f'cab/{ADDR}/value/acceleration+0'
    mq.write(setValueMsg)
    return (testResult, test_name)

def test_list(s):
    test_name = 'Test MQ List'
    setValueMsg = f'cab/{ADDR}/{MQ_LIST_VALUE_ASK}+ '
    mq.write(setValueMsg)
    mq.loop(setValueMsg)
    getValueRes = f'cab/{ADDR}/{MQ_LIST_VALUE_RES}'
    testResult = mq.loop(getValueRes)
    return (testResult, test_name)

tests_mq = [test_mqtt_start, test_throttle, test_direction_1, test_direction_2, test_direction_3, test_direction_0, 
            test_function, test_value, test_list]

# tests_mq = [test_mqtt_start, test_direction_2, test_direction_3]
