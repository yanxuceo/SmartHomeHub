// ButtonHandler.cpp

#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(uint8_t pin) : _pin(pin), _buttonState(false), _lastButtonState(false), _awaitingSecondClick(false),
                                            debounceDelay(50), doubleClickDelay(500), longPressDelay(1000) {
    _onPressedCallback = nullptr;
    _onDoublePressedCallback = nullptr;
    _onLongPressedCallback = nullptr;
}

void ButtonHandler::begin() {
    pinMode(_pin, INPUT_PULLUP);
    _lastDebounceTime = millis();
}

void ButtonHandler::update() {
    bool reading = !digitalRead(_pin);
    if (reading != _lastButtonState) {
        _lastDebounceTime = millis();
    }
    if ((millis() - _lastDebounceTime) > debounceDelay && reading != _buttonState) {
        _buttonState = reading;
        if (_buttonState) {
            _lastPressTime = millis();
        } else {
            unsigned long pressDuration = millis() - _lastPressTime;
            if (pressDuration >= longPressDelay) {
                if (_onLongPressedCallback) _onLongPressedCallback();
            } else if (millis() - _lastReleaseTime <= doubleClickDelay) {
                if (_onDoublePressedCallback) _onDoublePressedCallback();
                _lastReleaseTime = 0;
            } else {
                _lastReleaseTime = millis();
            }
        }
    }
    if (!_buttonState && _lastReleaseTime != 0 && (millis() - _lastReleaseTime) > doubleClickDelay) {
        if (_onPressedCallback) _onPressedCallback();
        _lastReleaseTime = 0;
    }
    _lastButtonState = reading;
}

void ButtonHandler::onPressed(void (*callback)()) {
    _onPressedCallback = callback;
}

void ButtonHandler::onDoublePressed(void (*callback)()) {
    _onDoublePressedCallback = callback;
}

void ButtonHandler::onLongPressed(void (*callback)()) {
    _onLongPressedCallback = callback;
}

// Setter methods for timing parameters
void ButtonHandler::setDebounceDelay(unsigned long delay) {
    debounceDelay = delay;
}

void ButtonHandler::setDoubleClickDelay(unsigned long delay) {
    doubleClickDelay = delay;
}

void ButtonHandler::setLongPressDelay(unsigned long delay) {
    longPressDelay = delay;
}
