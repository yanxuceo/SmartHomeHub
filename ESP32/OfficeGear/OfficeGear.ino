#include "ButtonHandler.h"
#include "TimerDisplay.h"
#include "WaterLevelDisplay.h"
#include "WiFiTimeSync.h"
#include "WaterDataManager.h"

#include "config.h"


TFT_eSPI tft = TFT_eSPI();   // Initialize TFT
TFT_eSprite image = TFT_eSprite(&tft); //TODO: needs a test

OfficeTimer timer;           // Create an OfficeTimer instance
WiFiTimeSync timeSync;
WaterDataManager waterDataManager;

WaterLevelDisplay waterDisplay(tft, SCREENWIDTH, SCREENHEIGHT);


ButtonHandler buttonA(35);   // button A on pin 35
ButtonHandler buttonB(0);    // button B on pin 0


void setup() {
  Serial.begin(115200);
  
  tft_init();
  button_init();

  waterDataManager.begin();

  int barCount_InMem =  waterDataManager.readBarCount();
  float waterLevel_InMem = waterDataManager.readCurrentWaterLevel();

  Serial.print("read barCount from memory: ");
  Serial.println(barCount_InMem); 

  Serial.print("read current waterLevel from memory: ");
  Serial.println(waterLevel_InMem); 

  //timeSync.addNetwork("YOUT WiFi SSID", "WiFi Password");
  waterDisplay.setCurrentWaterLevel(waterLevel_InMem);
  waterDisplay.setBarCount(barCount_InMem); // Set the total number of bars, default 5
  waterDisplay.setGap(4);      // Set the gap between bars

  timer.setThreshold(1800);    //seconds, default 30min
  timer.start();

  displayCurrentState();
}


void loop() {
  buttonA.update();
  buttonB.update();

  unsigned long currentMillis = millis();
  // Check if the Timer Menu is active and if it's time to update the display
  if (currentState == MAIN_MENU_TIMER && currentMillis - lastTimerUpdate >= timerUpdateInterval) {
      displayTimerMenu(tft, timer, timer.checkThreshold());       // Update the timer display
      lastTimerUpdate = currentMillis;                            // Reset the last update time
  }
}


void navigateToSubmenu(State submenu) {
    lastMainMenuState = currentState; // Store the current main menu state
    currentState = submenu;           // Navigate to the submenu
}

void handle_buttonA_singleClick() {
  // Check if in a submenu
  if ((currentState >= SUB_MENU_WATER_VOLUME && currentState < LAST_SUB_MENU_WATER) ||
      (currentState >= SUB_MENU_SEATING_HOUR_THRESHOLD && currentState < LAST_SUB_MENU_LONG_SEATING) ||
      (currentState >= SUB_MENU_UPDATE_DATA && currentState < LAST_SUB_MENU_SYSTEM)) {
      // Cycling through options within a specific submenu
      currentOption = (currentOption + 1) % getOptionCount(currentState);
  } else if (currentState < MAIN_MENU_SYSTEM) {
    // Cycling through main menu items
    currentState = static_cast<State>(currentState + 1);
  } else {
    // Loop back to the first main menu item
    currentState = MAIN_MENU_CURRENT_BAR;
  }

  displayCurrentState();  // Update the display according to the new state
}

void handle_buttonA_doubleClick() {
  Serial.println("Button A Double Pressed - Navigating sub-menus");

  // Example for one first-class menu category; you need to replicate similar logic for others
  if (currentState >= SUB_MENU_WATER_VOLUME && currentState < LAST_SUB_MENU_WATER) {
    // Ensure we stay within the water volume sub-menu range
    currentState = static_cast<State>(currentState + 1);
    if (currentState == LAST_SUB_MENU_WATER) {
      // If we've reached the last sub-menu for water, wrap to the first sub-menu under water
      currentState = SUB_MENU_WATER_VOLUME;
    }
    currentOption = 0;   // Reset the option index for the new sub-menu
  }
  // Replicate similar conditional blocks for other first-class menu categories and their sub-menus
  displayCurrentState(); // Update the display to show the new sub-menu
}

void handle_buttonA_longClick() {
  Serial.println("Button A Long Pressed - Entering sub-menu");

  switch (currentState) {
    case MAIN_MENU_CURRENT_BAR:
      navigateToSubmenu(SUB_MENU_WATER_VOLUME);
      break;

    // Add cases for other main menus as needed
    case MAIN_MENU_TIMER:
      navigateToSubmenu(SUB_MENU_SEATING_HOUR_THRESHOLD);
      break;
  }
  displayCurrentState();  // Update the display for the sub-menu
}

void handle_buttonB_singleClick() {
  int temp_barCount = 0;
  Serial.println("Button B clicked, confirm!");

  switch (currentState) {
    case MAIN_MENU_CURRENT_BAR:
      Serial.println("Increasing water level...");
      waterDisplay.increaseWaterLevel(0.5);
      waterDisplay.drawBatteryIndicator();

      Serial.print("current water level: ");
      Serial.println(waterDisplay.getCurrentWaterLevel()); 

      waterDataManager.saveCurrentWaterLevel(waterDisplay.getCurrentWaterLevel());
      break;

    case MAIN_MENU_TIMER:
      Serial.println("Resetting timer...");
      timer.reset();
      displayCurrentState(); // Refresh the display to show any changes.
      break;

    case MAIN_MENU_SYSTEM:
      // Here, Button B might not have an action, or you could use it to refresh system info, for example.
      Serial.println("Refreshing system info...");
      displayCurrentState(); // Refresh the display to show any changes.
      // RefreshSystemInfo(); // This would refresh or display system information.
      break;

    // Sub-menu actions will be handled in a different section.
    case SUB_MENU_WATER_VOLUME:
      Serial.println("Water Volume Selected: " + String(waterVolumeOptions[currentOption]));
      selectedWaterVolume = convertVolumeToMl(waterVolumeOptions[currentOption]);

      Serial.print("converted volumn value: ");
      Serial.println(selectedWaterVolume); 

      temp_barCount = selectedWaterVolume/selectedCupSize;

      waterDisplay.setBarCount(temp_barCount);
      waterDataManager.saveBarCount(temp_barCount); //save to flash
      break;

    case SUB_MENU_CUP_SIZE:
      Serial.println("Cup Size Selected: " + String(cupSizeOptions[currentOption]));
      selectedCupSize = convertVolumeToMl(cupSizeOptions[currentOption]);

      Serial.print("converted cup size value: ");
      Serial.println(selectedCupSize); 

      temp_barCount = selectedWaterVolume/selectedCupSize;  
      waterDisplay.setBarCount(temp_barCount);
      waterDataManager.saveBarCount(temp_barCount);
      break;

    case SUB_MENU_SEATING_HOUR_THRESHOLD:
      Serial.println("Seating time Selected: " + String(seatingHours[currentOption]));
      selectedSeatingHour = currentOption; // Save the selected cup size

      timer.setThreshold(convertTimeToSeconds(seatingHours[currentOption]));
      break;

    case SUB_MENU_UPDATE_DATA:
      Serial.println("Updating/Syncing Data...");
      // UpdateSyncData();                // Function to initiate data sync/update
      break;
      // Add other sub-menu cases as needed
  }
}

void handle_buttonB_doubleClick() {
  Serial.println("Button B Double Pressed, to be implemented.");
}

void handle_buttonB_longClick() {
  switch (currentState) {
    case MAIN_MENU_CURRENT_BAR:
      waterDisplay.resetWaterLevel();
         waterDataManager.saveCurrentWaterLevel(0.0f);
    break;

    case SUB_MENU_WATER_VOLUME:
    case SUB_MENU_CUP_SIZE:
    case SUB_MENU_SEATING_HOUR_THRESHOLD:
    case SUB_MENU_UPDATE_DATA:
      Serial.println("Button B Long Pressed, return to main menu");
   
      currentState = lastMainMenuState; // Return to the last main menu state
      currentOption = 0; 
      displayCurrentState(); // Refresh the display to show the main menu
    break;
  }
}


void displayCurrentState() {
  tft.fillScreen(TFT_BLACK);  // Clear the screen
  tft.setTextSize(3); // Set a default font size for most menus

  switch (currentState) {
    case MAIN_MENU_CURRENT_BAR:
      waterDisplay.drawBatteryIndicator();
      break;
    case MAIN_MENU_TIMER:
      displayTimerMenu(tft, timer, false);
      break;
    case MAIN_MENU_SYSTEM:
      tft.setTextSize(3); 
      tft.setTextColor(TFT_WHITE, TFT_BLACK); 
      tft.drawString("Data Center", 5, 5, 2);
      break;

    // We will add sub-menu displays later
    case SUB_MENU_WATER_VOLUME:
      waterTarget_SubmenuLayout(tft);
      break;
    case SUB_MENU_CUP_SIZE:
      cupSize_SubmenuLayout(tft);
      break;

    case SUB_MENU_SEATING_HOUR_THRESHOLD:
      seatingTime_SubmenuLayout(tft);
      break;

    default:
      break;
  }
}


void tft_init() {
  tft.init();
  tft.setRotation(1);
  tft.setTextSize(3);  
  tft.fillScreen(TFT_BLACK);
}

void button_init() {
  buttonA.begin();
  buttonB.begin();

  buttonA.setDoubleClickDelay(250);
  buttonB.setDoubleClickDelay(250);

  buttonA.onPressed(handle_buttonA_singleClick);
  buttonA.onDoublePressed(handle_buttonA_doubleClick);
  buttonA.onLongPressed(handle_buttonA_longClick);

  buttonB.onPressed(handle_buttonB_singleClick);
  buttonB.onDoublePressed(handle_buttonB_doubleClick);
  buttonB.onLongPressed(handle_buttonB_longClick);
}

