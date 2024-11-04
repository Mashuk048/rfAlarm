# ESP8266 Alarm System with SOS Button and MQTT Integration

## Overview

This project implements an ESP8266-based alarm system that can be triggered using an SOS button. It integrates with MQTT for remote monitoring and control, allowing users to receive alerts and manage the alarm state through a wireless connection. The system also includes debounce logic to handle button presses effectively, preventing false alarms.

## Features

- **WiFi Connectivity**: Connects to a specified WiFi network.
- **MQTT Integration**: Publishes alarm states and listens for incoming messages.
- **Long Press Detection**: Triggers an alarm if the button is pressed for a specified duration (2 seconds).
- **Debouncing Logic**: Ensures accurate button press detection using interrupts and debouncing.
- **Radio Frequency (RF) Message Handling**: Receives RF messages to trigger the alarm.
- **Snooze Functionality**: Temporarily disables the alarm for a defined period.

## Hardware Requirements

- ESP8266 board (e.g., NodeMCU, Wemos D1 Mini)
- SOS button (momentary push button)
- Buzzer or alarm device
- LED for status indication
- RF transmitter and receiver modules (if RF functionality is used)
- Breadboard and jumper wires for connections

## Wiring Diagram

- **SOS Button**: Connect to GPIO 12 (D6)
- **Buzzer**: Connect to GPIO 0 (D3)
- **LED**: Connect to GPIO 14 (D5)
- **Data In for RF**: Connect to GPIO 2 (D4)

## Installation

1. **Set Up the Hardware**:
   - Connect the ESP8266 to your computer.
   - Wire the components according to the specified GPIO pins.

2. **Install Required Libraries**:
   - Make sure you have the following libraries installed in your Arduino IDE:
     - `ESP8266WiFi`
     - `PubSubClient`
     - `ArduinoJson`
     - `RCSwitch`

3. **Configure WiFi and MQTT Settings**:
   - Update the `ssid`, `password`, and `mqtt_server` variables in the code with your network credentials and MQTT broker information.

4. **Upload the Code**:
   - Open the Arduino IDE, load the provided code, and upload it to your ESP8266 board.

## Usage

1. Once the device is powered on, it will connect to the specified WiFi network.
2. Press the SOS button to trigger the alarm. A long press (2 seconds) will activate the alarm.
3. The system publishes the alarm state to the MQTT topic, allowing remote monitoring.
4. Use an MQTT client to subscribe to the relevant topics and monitor the alarm status.

## Functions Overview

- `ISR()`: Interrupt service routine for the SOS button. Handles button state changes and triggers alarm.
- `setup_wifi()`: Connects the ESP8266 to the WiFi network.
- `setup_mqtt()`: Initializes MQTT client and sets the callback.
- `loop()`: Main loop that handles button presses, MQTT connectivity, and RF data reception.
- `setAlarm_and_Publish()`: Activates the alarm and publishes the state to the MQTT topic.
- `setLongPressTimer()`: Monitors long button presses to stop the alarm after a specified time.
- `checkResumeTimer()`: Resumes normal operation after the snooze time expires.

## Notes

- Make sure the MQTT broker is running and accessible to the ESP8266 for proper functionality.
- Adjust the `stime` variable to change the snooze duration as needed.

## Contributing

Contributions are welcome! Feel free to submit issues or pull requests for enhancements or bug fixes.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
