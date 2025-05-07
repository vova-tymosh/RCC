#ifndef PROTOCOL_H
#define PROTOCOL_H

const char SEPARATOR = ' ';

// NRF Protocol Message codes
const char NRF_INTRO = 'A';
const char NRF_SUB = 'B';
const char NRF_LIST_CAB = 'C';
const char NRF_HEARTBEAT = 'H';

const char NRF_THROTTLE = 'T';
const char NRF_DIRECTION = 'D';
const char NRF_SET_FUNCTION = 'F';
const char NRF_GET_FUNCTION = 'P';
const char NRF_SET_VALUE = 'S';
const char NRF_GET_VALUE = 'G';
const char NRF_LIST_VALUE_ASK = 'L';
const char NRF_LIST_VALUE_RES = 'J';

const char NRF_TYPE_LOCO = 'L';
const char NRF_TYPE_KEYPAD = 'K';

const char NRF_SEPARATOR = SEPARATOR;

// MQTT Protocol Message codes
const char *MQ_PREFIX = "cab";
const char *MQ_INTRO = "intro";
const char *MQ_HEARTBEAT_VALUES = "heartbeat/values";

const char *MQ_SET_THROTTLE = "throttle";
const char *MQ_SET_DIRECTION = "direction";
const char *MQ_GET_FUNCTION = "function/get";
const char *MQ_SET_FUNCTION = "function/";
const char *MQ_GET_VALUE = "value/get";
const char *MQ_SET_VALUE = "value/";
const char *MQ_LIST_VALUE_ASK = "value/list";
const char *MQ_LIST_VALUE_RES = "keys";

const char *MQ_DIRECTIONS[4] = {"REVERSE", "FORWARD", "STOP", "NEUTRAL"};
const char *MQ_ON = "ON";
const char *MQ_OFF = "OFF";

const char MQ_SEPARATOR = SEPARATOR;

// Command Line Interface (CLI) Protocol Message codes
const char CMD_THROTTLE = 'T';
const char CMD_DIRECTION = 'D';
const char CMD_GET_FUNCTION = 'P';
const char CMD_SET_FUNCTION = 'F';
const char CMD_GET_VALUE = 'G';
const char CMD_SET_VALUE = 'S';
const char CMD_LIST_VALUE = 'L';

const char CMD_CMD = 'C';
const char CMD_ERASE = 'E';
const char CMD_REBOOT = '!';

#endif // PROTOCOL_H
