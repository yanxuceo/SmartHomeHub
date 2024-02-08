# WiFiTimeSync Class for ESP32

The `WiFiTimeSync` class simplifies managing WiFi connections and synchronizing the system time with an NTP server on ESP32 devices. It supports handling multiple WiFi networks, enabling the dynamic selection for connection, and provides time synchronization functionalities with timezone adjustment, specifically tailored for Berlin time (CET/CEST).

## Features

- Manages multiple WiFi network credentials.
- Facilitates easy connection to WiFi networks.
- Offers methods to check the WiFi connection status.
- Enables time synchronization with NTP servers.
- Configures and handles Berlin timezone (CET/CEST), adjustable for other timezones.
- Retrieves the synchronized time in a human-readable string format.

## Prerequisites

- An ESP32 development board.
- Arduino IDE or PlatformIO with ESP32 board support installed.
- Basic knowledge of programming and connectivity with ESP32.

## Installation

1. Copy `WiFiTimeSync.h` and `WiFiTimeSync.cpp` into your project directory.
2. Include the `WiFiTimeSync` class in your main sketch: `#include "WiFiTimeSync.h"`.

## Usage

### Initialization

Create an instance of the `WiFiTimeSync` class:

```cpp
WiFiTimeSync timeSync;
```

### Adding WiFi Networks
Add your WiFi networks to the instance:

```cpp
timeSync.addNetwork("YourWiFiSSID", "YourWiFiPassword");
// Repeat the above line as needed for additional networks.
```

### Connecting to a WiFi Network
Initiate connection to a network by specifying its index:

```cpp
timeSync.connectToNetwork(0); // Connects to the first added network.
```

### Synchronizing Time
To synchronize the system time with an NTP server:

```cpp
timeSync.syncTime();
```

### Getting the Synchronized Time
Retrieve the current time, synchronized and adjusted for the specified timezone:

```cpp
String currentTime = timeSync.getSynchronizedTime();
Serial.println(currentTime);
```

## Example Usage


```cpp
void setup() {
    Serial.begin(115200);
    
    // Initialize and add WiFi networks
    timeSync.addNetwork("SSID1", "password1");
    timeSync.addNetwork("SSID2", "password2");
    
    // Connect to a WiFi network
    timeSync.connectToNetwork(0);
    
    // Synchronize time
    timeSync.syncTime();
    
    // Print the synchronized time
    Serial.println(timeSync.getSynchronizedTime());
}

void loop() {
    // Periodically print the synchronized time
    delay(10000);
    Serial.println(timeSync.getSynchronizedTime());
}
```