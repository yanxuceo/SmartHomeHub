#ifndef WaterLevelDisplay_h
#define WaterLevelDisplay_h

#include <TFT_eSPI.h> // Ensure TFT_eSPI library is included in your project

class WaterLevelDisplay {
public:
    WaterLevelDisplay(TFT_eSPI &display, int screenWidth, int screenHeight);
    void setBarCount(int count);
    void setGap(int gap);
    void setCurrentWaterLevel(float waterLevel);

    void increaseWaterLevel(float increaseUnit);
    void drawBatteryIndicator();
    void resetWaterLevel();

    int getBarCount();
    float getCurrentWaterLevel();


private:
    TFT_eSPI &tft;
    int _screenWidth;
    int _screenHeight;
    int _barCount;
    int _gap;
    
    float _currentWaterLevel;
    float _totalWaterDrinking;

    static constexpr int _barHeight = 135; // Assuming full height for each bar
    static constexpr uint16_t LIGHT_GREEN = 0x8FEA; // Hexadecimal color code for light green
};

#endif
