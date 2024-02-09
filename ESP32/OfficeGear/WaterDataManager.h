#ifndef WaterDataManager_h
#define WaterDataManager_h

#include <Preferences.h>

class WaterDataManager {
public:
    WaterDataManager();

    void begin();

    void saveDailyWaterIntake(int dailyIntake);
    int readDailyWaterIntake();
    
    void saveBarCount(int barCount);
    int readBarCount();

    void saveCurrentWaterLevel(float waterLevel);
    float readCurrentWaterLevel();
    
    void clearData(); 
    
private:
    Preferences preferences;
};

#endif
