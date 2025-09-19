


### Throttle

Set throttle for a loco. The input is a value from 0 (stop) to 100 (max speed). Other numbers are ignored. The {value} for MQTT and CLI is a string representation, for NRF it is one byte binary.


```
Mqtt: cab/<addr>/throttle {value}
CLI:  T{value}
NRF:  T{binary_value}
```

Example, set throttle to 37:
```
Mqtt: cab/3/throttle 37
CLI:  T37
NRF:  T\x25
```

### Direction

Set direction for a loco. For MQTT the direction is one of REVERSE, FORWARD, STOP or NEUTRAL. For NRF/CLI the direction is represented by a number - 0, 1, 2 or 3, where 0 - REVERSE, 1 - FORWARD, 2 - STOP and 3 - NEUTRAL. This approach aligns with DCC and JMRI with exception of NEUTRAL(3). The later is an RCC addition to support live steam locomotives.

```
Mqtt: cab/{addr}/direction {REVERSE, FORWARD, STOP, NEUTRAL}
CLI:  D{value}
NRF:  D{binary_value}
```

Example, set direction forward:
```
Mqtt: cab/3/direction FORWARD
CLI:  D1
NRF:  D\x01
```

### Function

Turn specific function on or off. This command has two parameters - function id ({func} below) and the {state} - ON or OFF. Function id is a number (currently from 0 to 30, but the range may increase in future). The state is {ON, OFF} for MQTT and {1, 0} for NRF/CLI. In case of NRF command argument is packed into 1 byte, where state takes highets bit and the rest 7 are allocated for the function id.

```
Mqtt: cab/{addr}/function/{func} {ON, OFF}
CLI:  F{state}{func}
NRF:  F{state:1}{func:8}
```

Example, turn ON function 1 (bell):
```
Mqtt: cab/3/function/1 ON
CLI:  F11
NRF:  F\x81
```
