#ifndef CONFIG_h
#define CONFIG_h

#define SCREENWIDTH     240             // Width of the screen in pixels
#define SCREENHEIGHT    135             // Height of the screen in pixels

enum State {
  MAIN_MENU_CURRENT_BAR,
  MAIN_MENU_TIMER,
  MAIN_MENU_SYSTEM,

  SUB_MENU_WATER_VOLUME,
  SUB_MENU_CUP_SIZE,
  LAST_SUB_MENU_WATER,

  SUB_MENU_SEATING_HOUR_THRESHOLD,
  LAST_SUB_MENU_LONG_SEATING,

  SUB_MENU_UPDATE_DATA,
  LAST_SUB_MENU_SYSTEM,

  NUM_STATES  // Helper to keep track of the total number of states
};


// water tracking submemu options
const char* waterVolumeOptions[] = {"1L", "1.5L", "2L", "2.5L", "3L"};
const char* cupSizeOptions[] = {"200ml", "300ml","300ml", "500ml"};
static int waterVolumeOptionCount = sizeof(waterVolumeOptions) / sizeof(waterVolumeOptions[0]);
static int cupSizeOptionCount = sizeof(cupSizeOptions) / sizeof(cupSizeOptions[0]);

// long seating submenu options
const char* seatingHours[] = {"15min", "30min", "45min", "60min"};
static int seatingHoursCount = sizeof(seatingHours) / sizeof(seatingHours[0]);

// selection variables for water
static int selectedWaterVolume = 1000; // Index of the selected water volume
static int selectedCupSize = 200;      // Index of the selected cup size

// selection variables for long seating
static int selectedSeatingHour = 0;

// Track the selected option in sub-menus
static int currentOption = 0;      

State currentState = MAIN_MENU_CURRENT_BAR;
State lastMainMenuState = MAIN_MENU_CURRENT_BAR; // Default to the first main menu item


int getOptionCount(State state) {
  switch (state) {
    case SUB_MENU_WATER_VOLUME:
      return waterVolumeOptionCount;
    case SUB_MENU_CUP_SIZE:
      return cupSizeOptionCount;
    // Add cases for other sub-menus
    case SUB_MENU_SEATING_HOUR_THRESHOLD:
      return seatingHoursCount;

    default:
      return 0;
  }
}


int convertVolumeToMl(const char* volumeStr) {
    double volume = 0.0; // Use double to handle fractional values
    char* endPtr;

    if (strstr(volumeStr, "L")) {
        // Found "L" in the string, indicating liters. Convert to double then to milliliters.
        volume = strtod(volumeStr, &endPtr) * 1000;
    } else if (strstr(volumeStr, "ml")) {
        // Found "ml" in the string, indicating milliliters. Direct conversion to double.
        volume = strtod(volumeStr, &endPtr);
    }
    
    return static_cast<int>(volume);
}


#endif