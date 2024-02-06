# ButtonHandler Library for ESP32/Arduino

The ButtonHandler library provides an easy and efficient way to handle button inputs on Arduino platforms. It supports detecting single clicks, double clicks, and long press events without the common issue of misinterpreting double clicks as two single clicks. This library is designed to be flexible, allowing users to set custom debounce, double click, and long press timings for each button instance.

## Features

- **Debounce Handling:** Ensures stable button state changes by eliminating noise.
- **Single Click Detection:** Detects single button presses.
- **Double Click Detection:** Identifies quick, successive button presses as a double click.
- **Long Press Detection:** Recognizes when a button is held down for a longer duration.
- **Custom Timing Parameters:** Allows setting custom timings for debounce, double click, and long press thresholds.

## Installation

1. Download the ButtonHandler library as a ZIP file.
2. Open the Arduino IDE, navigate to `Sketch` > `Include Library` > `Add .ZIP Library...`.
3. Choose the downloaded ZIP file and click "Open" to install.
4. Once installed, you can include the ButtonHandler library in your sketches using `#include <ButtonHandler.h>`.

## Usage

To use the ButtonHandler library in your project, follow these steps:

### Basic Setup

```cpp
#include <ButtonHandler.h>

// Initialize the ButtonHandler with the pin number
ButtonHandler button1(2); // Example: button connected to pin 2

void setup() {
  Serial.begin(115200);
  button1.begin();

  // Set event callbacks
  button1.onPressed([]() { Serial.println("Button 1 Single Click"); });
  button1.onDoublePressed([]() { Serial.println("Button 1 Double Click"); });
  button1.onLongPressed([]() { Serial.println("Button 1 Long Press"); });
}

void loop() {
  // Continuously update button state
  button1.update();
}
```


### Customizing Timings
Adjust debounce, double click, and long press timings as needed:

```cpp
button1.setDebounceDelay(100);       // Debounce delay in milliseconds
button1.setDoubleClickDelay(400);    // Maximum interval between clicks for a double click
button1.setLongPressDelay(800);      // Minimum press duration for a long press
```

### API Reference

```cpp
ButtonHandler(uint8_t pin): Constructor. Initializes a button handler on a specific pin.
void begin(): Initializes the button hardware, setting the pin mode.
void update(): Updates the button state. Should be called in the loop.
void onPressed(void (*callback)()): Sets the callback function for single click events.
void onDoublePressed(void (*callback)()): Sets the callback function for double click events.
void onLongPressed(void (*callback)()): Sets the callback function for long press events.
void setDebounceDelay(unsigned long delay): Sets the debounce delay.
void setDoubleClickDelay(unsigned long delay): Sets the delay for detecting double clicks.
void setLongPressDelay(unsigned long delay): Sets the delay for detecting long presses.
```