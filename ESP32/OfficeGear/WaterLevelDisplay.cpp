#include "WaterLevelDisplay.h"

WaterLevelDisplay::WaterLevelDisplay(TFT_eSPI &display, int screenWidth, int screenHeight) 
: tft(display), _screenWidth(screenWidth), _screenHeight(screenHeight), _barCount(0), _gap(4), _currentWaterLevel(0.0), _totalWaterDrinking(0.0) {}

void WaterLevelDisplay::setBarCount(int count) {
  _barCount = count;
}

int WaterLevelDisplay::getBarCount() {
  return _barCount;
}

float WaterLevelDisplay::getCurrentWaterLevel() {
  return _currentWaterLevel;
}

void WaterLevelDisplay::setCurrentWaterLevel(float waterLevel) {
  _currentWaterLevel = waterLevel;
}

void WaterLevelDisplay::setGap(int gap) {
  _gap = gap;
}

void WaterLevelDisplay::increaseWaterLevel(float increaseUnit) {
  _currentWaterLevel += increaseUnit;
  if (_currentWaterLevel > _barCount) {
    _currentWaterLevel = increaseUnit; // Reset to 0 if it exceeds the bar count
  }
}

void WaterLevelDisplay::drawBatteryIndicator() {
  int barWidth = (_screenWidth - (_gap * (_barCount - 1))) / _barCount;

  for (int i = 0; i < _barCount; i++) {
      if (i < (int)_currentWaterLevel) {
          tft.fillRect(i * (barWidth + _gap), 0, barWidth, _barHeight, LIGHT_GREEN);
      } else {
          tft.fillRect(i * (barWidth + _gap), 0, barWidth, _barHeight, TFT_BLACK);
          if (i < _currentWaterLevel) {
              tft.fillRect(i * (barWidth + _gap), 0, barWidth / 2, _barHeight, LIGHT_GREEN);
          }
      }
      if (i < _barCount - 1) {
          tft.drawLine((i + 1) * (barWidth + _gap) - 1, 0, (i + 1) * (barWidth + _gap) - 1, _barHeight, TFT_WHITE);
      }
  }
}

void WaterLevelDisplay::resetWaterLevel() {
  //TODO: 2024.02.06 intended to add currentWater drinking to total drinking, but it should be done by flashing w/r
  //_totalWaterDrinking += _currentWaterLevel;
  _currentWaterLevel = 0;
  
  drawBatteryIndicator();
}
