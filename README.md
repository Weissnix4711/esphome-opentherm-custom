Extremely basic mock up / plan. Very much incomplete.

I don't know what's going on at this point.

- [weather comp](https://gist.github.com/Weissnix4711/60330176884b8c07db2f18d7538d2d8e)

In an ideal scenario, the following is what I would like to finally achieve:

```yaml
opentherm:

remote_reciever:
  pin: D2
  dump: opentherm
  slave_status: true # This option works similar to i2c 'scan'.
  # If enabled, runs R ID3 and dumps slave status to console.
  # Maybe also 125 (OT ver) and 127 (OEM ver), though those are not necessarily supported
  # by all devices.

remote_transmitter:
  pin: D1
  carrier_duty_percent: 100%

sensor:
  - platform: opentherm
    name: OpenTherm Sensor
    message: # message to be sent
      message_type: READ_DATA
      message_id: 3
      message_data:
        high_byte: 0
        low_byte: 0
    data: # how to inerpret returned data
      data_type: f8.8
                # or u16, s16
      # Optionally, one of:
      #high_byte_data_type: u8 or s8
      #low_byte_data_type: u8 or s8

binary_sensor:
  - platform: opentherm
    name: OpenTherm Binary Sensor
    message: # same as before
      message_type: READ_DATA
      message_id: 5
      message_data:
        high_byte: 0
        low_byte: 0
    data:
      bit: 2 # 2nd bit
    # Alternatively, have some sort of lambda templatability,
    # and allow the user to do whatever they want with the output data.

climate:
  - platform: opentherm
    # Some different approaches could be used here. Either have set configs for use cases listed below,
    # or let the user manually select the IDs for setpoint, read value (and optionally bounds).
    # This should work fine, as long as we presume temperature values are f8.8 and bounds are s8. However,
    # theoretically another data types could be used. Maybe have a lambda option as well?

    # Common configurations:
    #  - Flow (setpoint: ID1, read: ID25, bounds: ID49)
    #  - CH2 (setpoint: ID8, read: ID31)
    #  - Room setpoint (setpoint: ID16)
    #  - Room setpoint 2 (setpoint: ID23)
    #  - Room temp for OTC (setpoint: ID24) *no read so must use optimistic
    #  - DHW (setpoint: 56, read: ID26, bounds: ID48)
    #  - DHW2 (read: ID32) *apparently no setpoint, what's the deal with that?
    # Not included here is outside temp, as there is no standard command to set it, only read.
    # For boilers which support OTC, the temperature sensor should interface with the boiler directly.
    # Alternatively, run all OTC logic on the master (the ESP) and send only the flow temp back.
```