# No free vendor documentation available #

Unfortunately FrSky does not provide documentation of the FrSky S.Port protocol to everyone but only so selected developers after the sign an NDA.

We have not seen the official documentation (and of course not signed and NDA). Therefore this description is something like a "best guess" based on reversing what is going on and looking at some free source code.

Sources used:
  * OpenTX Taranis S.Port parsing code: https://code.google.com/p/opentx/source/browse/trunk/src/telemetry/frsky_sport.cpp
  * OpenXVario FrSky code https://code.google.com/p/openxvario/source/browse/branches/openxsensor/oxs_out_frsky.cpp

There is **no guarantee that the information on this page is correct**

# Basic spec #

FrSky S.Port protocol is an 3.3V inverted single wire serial protocol running at 57.600baud. To explain more carefully:

  * serial protocol, so it can be used with an UART
  * 3.3V level
  * inverted, i.e. high and low level are swapped
  * single wire, meaning that both tansmit and receive are using the same wire, so they cannot talk at the same time (half duplex)

So to correctly read the protocol you will need a serial port that can to all of the things above. Please note that the Atmel hardware UARTs used in Arduino boards do not have an inverted mode

# Logical procotol #

  * Receiver (e.g. FrSky X8R) polls sensors by sending:
    * 0x7E (8bit start-stop)
    * 8\_bit\_sensor\_id
  * If a sensor is present it answers by sending:
    * 0x10 (8bit data frame header)
    * value\_type (16 bit, e.g. voltage / speed)
    * value (32 bit, may be signed or unsigned depending on value type)
    * checksum (8 bit)
  * If no sensor is present no answer gets sent and the receiver goes on to the next sensor-id


# Sensor Ids #

The X8R receiver does not poll all possible sensor ids (0x00-0xFF) but only a few selected ones. There is no public documentation on used FrSky sensor ids available so you will have to find and use your own.

Ids known to work are: 0x00, 0xA1

If two sensors use the same id the bus will not work since both sensors will try to send at the same time. This will result in telemtry connection breaking up.


# Value types #

A value type indicates what kind of value will be transmitted, e.g. voltage / current / speed etc.

One sensor can support multiple value types: Each time it receives a poll it may send a different value type. So if a sensor supports output of voltage and current it may output voltage on the first poll, current on the second, voltage on the third, current at the fourth and so on.

Value types are described here: https://code.google.com/p/opentx/source/browse/trunk/src/telemetry/frsky_sport.cpp  (as FrSky New Data IDs)


Some types:

| **Type\_ID** | **Description** | **Unit** | **Data value** |
|:-------------|:----------------|:---------|:---------------|
| 0x0110       | vario           | 0.01m/s  | int32          |
| 0x0200       | current         | 0.01A    | uint32         |
| 0x0210       | voltage         | 0.01V    | uint32         |
| 0x0500       | RPM             | 1 rpm    | uint32         |
| 0x0600       | capacity used   | 1 mah    | uint32         |
| 0x0610       | altitude        | 1m       | int32          |
| 0x0830       | gps speed       | 1 km/h   | uint32         |

Many types do not only have one id but support a range of type-ids, enabling transfer of more than one value for any type. for details see the taranis sport code linked above.