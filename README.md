# Smart HVAC Safety and Monitoring System

## Project Description
This project is a smart HVAC monitoring and protection system designed using an ESP32 microcontroller in the Wokwi simulation environment. The system continuously monitors environmental temperature, allows real-time adjustment of the desired temperature, and automatically controls an HVAC relay output.

The project focuses on real-time embedded system design, combining automation, multitasking, fault detection, and emergency safety handling using FreeRTOS.

---

## Main Features

- Real-time temperature monitoring using a DHT22 sensor
- Adjustable HVAC temperature setpoint using a potentiometer
- Automatic relay control for HVAC operation
- Emergency stop system using hardware interrupts
- Sensor anomaly and fault detection
- LED and buzzer alarm indicators
- FreeRTOS multitasking support
- Watchdog timer protection for system reliability

---

## Hardware Components

| Component | Description |
|---|---|
| ESP32 Dev Board | Main microcontroller |
| DHT22 Sensor | Measures ambient temperature |
| Potentiometer | Adjusts target temperature |
| Relay Module | Controls HVAC output |
| Push Button | Emergency stop trigger |
| LED | Visual warning indicator |
| Buzzer | Audible alarm |

---

## Pin Connections

| ESP32 Pin | Connected Device |
|---|---|
| GPIO 4 | DHT22 Data Pin |
| GPIO 15 | Emergency Button |
| GPIO 23 | Relay Module |
| GPIO 19 | Buzzer |
| GPIO 16 | LED |
| GPIO 34 | Potentiometer |

---

## System Operation

### Normal Mode
- The DHT22 sensor reads the current temperature.
- The potentiometer sets the desired HVAC temperature.
- If the measured temperature falls below the setpoint, the relay activates the HVAC system.
- High temperatures activate the warning LED and buzzer.

### Emergency Mode
The system enters emergency shutdown if:

- The emergency stop button is pressed
- Invalid sensor readings are detected
- Sudden abnormal temperature spikes occur

During emergency mode:

- HVAC relay is disabled
- Alarm buzzer is activated
- Warning LED remains ON
- Error messages are displayed in the Serial Monitor

---

## Sensor Safety Validation
To improve system reliability, the software validates all sensor readings.

Emergency shutdown is triggered when:

- Temperature values are outside the valid range
- The sensor returns invalid (NaN) values
- Temperature changes too rapidly between readings

---

## FreeRTOS Task Structure

| Task Name | Function |
|---|---|
| Sensor Task | Reads and filters sensor data |
| Control Task | Handles HVAC and safety logic |
| Logging Task | Sends status data to Serial Monitor |

Mutex synchronization is used to safely share data between tasks.

---

## Example Serial Output

```text
Temp: 27.3 °C | Setpoint: 30.0 °C | HVAC: ON
````

Emergency example:

```text
WARNING: Sensor anomaly detected
EMERGENCY STOP ACTIVATED
```

---

## Technologies Used

* Arduino IDE / ESP32 Framework
* FreeRTOS
* DHT Sensor Library
* ESP32 Watchdog Timer
* Wokwi Online Simulator

---

## How to Run

1. Open the project in Wokwi.
2. Start the simulation.
3. Change the potentiometer value to adjust the setpoint.
4. Modify the DHT22 temperature value.
5. Press the emergency button to test shutdown mode.
6. Observe the relay, LED, buzzer, and Serial Monitor outputs.

---

## Learning Objectives

This project demonstrates:

* Embedded systems programming
* Real-time multitasking
* Interrupt handling
* Fault-tolerant system design
* Sensor data validation
* Safety-critical control systems
* Hardware and software integration

---

## Future Improvements

Possible future enhancements include:

* OLED or LCD display integration
* Wi-Fi and IoT connectivity
* MQTT remote monitoring
* Mobile application support
* Cloud data logging
* PID-based HVAC control
* OTA firmware updates

---

## Author

Created as an embedded systems and real-time control simulation project using ESP32 and Wokwi.

```text
Temp: 26.40 °C | Setpoint: 30.15 °C | ADC: 2475 | E-Stop: OFF
```

Emergency condition example:

```text
⚠️ Sensor anomaly detected! Emergency stop triggered.
EMERGENCY STOP ACTIVATED
```

---

## How to Run the Simulation

1. Open the project in Wokwi.
2. Start the simulation.
3. Adjust the potentiometer to change the setpoint.
4. Modify the DHT22 temperature value.
5. Press the emergency button to trigger shutdown.
6. Observe relay, LED, buzzer, and Serial Monitor behavior.

---

## Learning Outcomes

This project demonstrates:

* Embedded systems programming
* Real-time multitasking with FreeRTOS
* Interrupt-driven design
* Sensor validation techniques
* Safety-critical system behavior
* Watchdog timer integration
* Shared-resource synchronization using mutexes

---

## Future Improvements

Possible extensions for the project include:

* OLED/LCD temperature display
* Wi-Fi or MQTT connectivity
* Remote monitoring dashboard
* Data logging to cloud storage
* Mobile app notifications
* Automatic HVAC PID control
* Over-the-air firmware updates

---

## Author

Developed as a Wokwi ESP32 simulation project for embedded systems and real-time control system experimentation.

```
```
