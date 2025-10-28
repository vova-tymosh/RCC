

# RCC Protocol Documentation

The Remote Command and Control (RCC) system supports three communication protocols: MQTT (WiFi), NRF52 (wireless), and CLI (serial). This document describes the command structure and message formats for each protocol.

## Protocol Overview

- **MQTT**: Text-based protocol over WiFi using MQTT broker
- **NRF**: Binary protocol over 2.4GHz wireless using Nordic nRF52 chips
- **CLI**: Text-based protocol over serial connection

## Command Reference

### Throttle

Set throttle for a locomotive. The input is a value from 0 (stop) to 100 (max speed). Other numbers are ignored. The {value} for MQTT and CLI is a string representation, for NRF it is one byte binary.

```
MQTT: cab/{addr}/throttle {value}
CLI:  T{value}
NRF:  T{binary_value}
```

Example, set throttle to 37:
```
MQTT: cab/3/throttle 37
CLI:  T37
NRF:  T\x25
```

### Direction

Set direction for a locomotive. For MQTT the direction is one of REVERSE, FORWARD, STOP or NEUTRAL. For NRF/CLI the direction is represented by a number - 0, 1, 2 or 3, where 0 - REVERSE, 1 - FORWARD, 2 - STOP and 3 - NEUTRAL. This approach aligns with DCC and JMRI with exception of NEUTRAL(3). The latter is an RCC addition to support live steam locomotives.

```
MQTT: cab/{addr}/direction {REVERSE, FORWARD, STOP, NEUTRAL}
CLI:  D{value}
NRF:  D{binary_value}
```

Example, set direction forward:
```
MQTT: cab/3/direction FORWARD
CLI:  D1
NRF:  D\x01
```

### Function

Turn specific function on or off. This command has two parameters - function id ({func} below) and the {state} - ON or OFF. Function id can be a number (0-255) or a named function. The state is {ON, OFF} for MQTT and {1, 0} for NRF/CLI.

```
MQTT: cab/{addr}/function/{func} {ON, OFF}
CLI:  F{state}{func}
NRF:  F{func},{state}
```

Example, turn ON function 1 (bell) using ID:
```
MQTT: cab/3/function/1 ON
CLI:  F11
NRF:  F1,1
```

Example, turn ON function using name:
```
MQTT: cab/3/function/bell ON
NRF:  Fbell,1
```

### Get Function

Retrieve the current state of a specific function. Function can be specified by ID or name. Response is the same as Set Function command.

```
MQTT: cab/{addr}/function/get {func}
CLI:  P{func}
NRF:  P{func}
```

Example, get state of function 1:
```
MQTT: cab/3/function/get 1
CLI:  P1
NRF:  P1
```

Example, get state using function name:
```
MQTT: cab/3/function/get bell
NRF:  Pbell
```

Example response:
```
MQTT: cab/3/function/1 OFF
NRF:  F1,0
```

### Set Function Name

Assign or update a name for a function ID. This creates a mapping between function numbers and human-readable names stored in the "functionNames" file.

```
MQTT: cab/{addr}/function/name/{id} {name}
NRF:  M{id},{name}
```

Example, assign name "bell" to function 1:
```
MQTT: cab/3/function/name/1 bell
NRF:  M1,bell
```

Example, assign name "headlight" to function 0:
```
MQTT: cab/3/function/name/0 headlight
NRF:  M0,headlight
```

### List Functions

Get a list of all defined function names and their IDs.

```
MQTT: cab/{addr}/function/list/req
NRF:  U
```

Response format: `{id1},{name1},{id2},{name2},...`

Example:
```
MQTT Request:  cab/3/function/list/req
MQTT Response: cab/3/function/list 0,headlight,1,bell,2,horn,3,coupler

NRF Request:  U
NRF Response: V0,headlight,1,bell,2,horn,3,coupler
```

### Set Value

Set a configuration value. Values are key-value pairs separated by colon for CLI, or as separate topic/payload for MQTT.

```
MQTT: cab/{addr}/value/{key} {value}
CLI:  S{key}:{value}
NRF:  S{key},{value}
```

Example, set locomotive name:
```
MQTT: cab/3/value/loconame MyLoco
CLI:  Sloconame:MyLoco
NRF:  Sloconame,MyLoco
```

### Get Value

Retrieve a configuration value. Response is the same as Set Value command.

```
MQTT: cab/{addr}/value/get {key}
CLI:  G{key}
NRF:  G{key}
```

Example, get locomotive name:
```
MQTT: cab/3/value/get loconame
CLI:  Gloconame
NRF:  Gloconame
```

Example response:
```
MQTT: cab/3/value/loconame MyLoco
NRF:  Sloconame,MyLoco
```


### List Values

Get a list of all available configuration keys.

```
MQTT: cab/{addr}/value/list/req
CLI:  L
NRF:  L
```

Response format uses comma separator:
```
MQTT: cab/{addr}/value/list {key1},{key2},{key3}...
CLI:  {key1},{key2},{key3}...
NRF:  J{key1},{key2},{key3}...
```

### Introduction

Device introduction message sent when connecting or requested. Without arguments it is a request from central station, with the rguments - response from a loco. The fields are the following: 
 - type: node tyoe, can be L for locomotive and K for Pad
 - addr: locomotive unique address
 - name: locomotive name
 - version: RCC version
 - format: an Python struct format for heartbeat values, only used by NRF

```
MQTT: cab/{addr}/intro {type},{addr},{name},{version},{format}
NRF:  A{type},{addr},{name},{version},{format}
```

Example:
```
MQTT: cab/3/intro L,3,MyLoco,1.0,BIIIHBBBBBBB
NRF:  AL,3,MyLoco,1.0,BIIIHBBBBBBB
```

### Heartbeat

Periodic status update containing locomotive state information.

```
MQTT: cab/{addr}/heartbeat/values {tick},{distance},{bitstate},{speed},{lost},{throttle},{throttle_out},{battery},{temperature},{psi},{current}
NRF:  H{binary_state_struct}
```

### Heartbeat Keys

Send the list of keys included in heartbeat values. Bitstate represents the state of all functions (32bit value), Temperature and Psi are applicable for live steam only.

```
MQTT: cab/{addr}/heartbeat/keys Tick,Distance,Bitstate,Speed...
NRF:  KTick,Distance,Bitstate,Speed...
```

## CLI-Only Commands

The following commands are only available via CLI (serial interface):

### Read File
```
CLI: R{filename}
```

### Write File
```
CLI: W{filename}:{content}
```

### Append File
```
CLI: A{filename}:{size}
```
Note: After sending this command, send the binary data of specified size.

### File Size
```
CLI: Z{filename}
```

### Erase Storage
```
CLI: E
```

### Next/Select Locomotive (Pad Only)
```
CLI: N[address]
```

Cycle to next locomotive or select specific locomotive by address.

Examples:
```
CLI: N          # Cycle to next locomotive
CLI: N3         # Select locomotive with address 3
CLI: N15        # Select locomotive with address 15
```

If the specified address is not found in the known locomotives list, the command is ignored.

### Custom Command
```
CLI: C{command_char}{parameters}
```

### Reboot
```
CLI: !
```

## NRF Protocol Details

### Command Structure

NRF commands use a packed binary structure:

```c
struct Command {
    uint8_t code;           // Command character
    union {
        uint8_t value;      // Generic 8-bit value
        struct {
            uint8_t functionId : 7;  // Function ID (0-127)
            uint8_t activate   : 1;  // Function state (0/1)
        };
    };
};
```

### Packet Constraints

- Maximum packet size: 144 bytes
- Command size: 2 bytes minimum
- String parameters are null-terminated
- Multi-part messages supported for large responses

## MQTT Topic Structure

All MQTT topics follow the pattern: `cab/{locomotive_address}/{command}`

### Standard Topics

- `cab/{addr}/throttle` - Set throttle value
- `cab/{addr}/direction` - Set direction
- `cab/{addr}/function/{id}` - Set function state
- `cab/{addr}/function/get` - Get function state
- `cab/{addr}/value/{key}` - Set configuration value
- `cab/{addr}/value/get` - Get configuration value
- `cab/{addr}/value/list` - List all keys
- `cab/{addr}/intro` - Device introduction
- `cab/{addr}/introreq` - Request introduction
- `cab/{addr}/heartbeat` - Request heartbeat
- `cab/{addr}/heartbeat/values` - Heartbeat data
- `cab/{addr}/heartbeat/keys` - Heartbeat key names
- `cab/{addr}/keys` - Response to value list request

## Function Names Example

Here's a complete example of setting up and using named functions:

1. **Set up function names:**
```
MQTT: cab/3/function/name/0 headlight
MQTT: cab/3/function/name/1 bell  
MQTT: cab/3/function/name/2 horn
MQTT: cab/3/function/name/3 coupler
```

2. **Control functions by name:**
```
MQTT: cab/3/function/headlight ON
MQTT: cab/3/function/bell ON
MQTT: cab/3/function/horn OFF
```

3. **Query function state by name:**
```
MQTT: cab/3/function/get bell
Response: cab/3/function/bell ON
```

4. **Mix names and numbers:**
```
MQTT: cab/3/function/0 ON        # headlight by number
MQTT: cab/3/function/bell OFF    # bell by name
```

The function names are stored persistently in the "functionNames" file using the Storage system, so they persist across reboots.

## Error Handling

- Invalid commands are ignored
- Out-of-range values are clamped or ignored
- Malformed packets are discarded
- Connection failures trigger automatic reconnection
- Invalid function names fall back to numeric parsing
- Function name storage failures are logged but don't affect operation
