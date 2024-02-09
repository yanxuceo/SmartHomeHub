#include "WaterDataManager.h"

WaterDataManager::WaterDataManager() {}

void WaterDataManager::begin() {
    preferences.begin("waterData", false);
}

void WaterDataManager::saveDailyWaterIntake(int dailyIntake) {
    preferences.putInt("dailyIntake", dailyIntake);
}

int WaterDataManager::readDailyWaterIntake() {
    return preferences.getInt("dailyIntake", -1); // Return 0 if key does not exist
}

void WaterDataManager::saveBarCount(int barCount) {
    preferences.putInt("barCount", barCount);
}

int WaterDataManager::readBarCount() {
    return preferences.getInt("barCount", 5);
}

void WaterDataManager::saveCurrentWaterLevel(float waterLevel) {
    preferences.putFloat("curLevel", waterLevel);
}


float WaterDataManager::readCurrentWaterLevel() {
    return preferences.getFloat("curLevel", 0.0f);
}


void WaterDataManager::clearData() {
    preferences.clear();            // Clear all keys in the "waterData" namespace
}

