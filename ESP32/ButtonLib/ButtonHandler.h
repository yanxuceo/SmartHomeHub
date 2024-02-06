// ButtonHandler.h

#ifndef ButtonHandler_h
#define ButtonHandler_h

#include <Arduino.h>

class ButtonHandler {
public:
    ButtonHandler(uint8_t pin);
    void begin();
    void update();
    void onPressed(void (*callback)());
    void onDoublePressed(void (*callback)());
    void onLongPressed(void (*callback)());

    // Setter methods for timing parameters
    void setDebounceDelay(unsigned long delay);
    void setDoubleClickDelay(unsigned long delay);
    void setLongPressDelay(unsigned long delay);

private:
    uint8_t _pin;
    bool _buttonState;
    bool _lastButtonState;
    unsigned long _lastDebounceTime;
    unsigned long _lastPressTime;
    unsigned long _lastReleaseTime;
    bool _awaitingSecondClick;

    void (*_onPressedCallback)();
    void (*_onDoublePressedCallback)();
    void (*_onLongPressedCallback)();

    // Timing parameters
    unsigned long debounceDelay;
    unsigned long doubleClickDelay;
    unsigned long longPressDelay;
};

#endif
