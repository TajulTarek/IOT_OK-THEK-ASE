# Smart Assistance Booth Prototype for Elderly Monitoring

## Overview
This project presents a **Smart Assistance Booth System** designed to enhance the safety and convenience of elderly individuals. The prototype integrates **ultrasonic sensors**, microcontrollers, and a simple API-based backend to detect human presence and trigger notifications in real-time. It serves as a foundational platform for **smart elder-care systems**, where minimal hardware can be used to provide immediate assistance and monitoring.

## Features
- **Real-Time Detection:** Ultrasonic sensors continuously monitor the presence of individuals in the booth.  
- **Automated Identification:** The system detects when an elderly person is in the booth and identifies their presence accurately.  
- **Modular Hardware Setup:** The prototype uses multiple booths that can function independently or in coordination.  
- **API Integration:** Detected events are sent to a backend service via HTTP requests, enabling further processing, logging, or alerting.  
- **Scalable Design:** Easy to expand for more booths or integrate with additional sensors (e.g., temperature, motion, or health monitoring).

## Hardware Components
- Microcontroller (e.g., ESP32 or Arduino)  
- Ultrasonic Distance Sensor (HC-SR04 or equivalent)  
- LEDs for status indication  
- Push buttons for manual controls  
- Power supply and connecting wires  

*Note: Placeholder images are included; replace them with actual hardware photos when available.*

## System Architecture
1. **Detection Layer:** Ultrasonic sensors monitor the booth for human presence.  
2. **Control Layer:** Microcontroller processes sensor data and determines whether assistance is required.  
3. **Notification Layer:** Relevant data is sent via API requests to a backend server for logging or alerting.  
4. **User Interaction:** LEDs and optional display provide immediate feedback inside the booth.
