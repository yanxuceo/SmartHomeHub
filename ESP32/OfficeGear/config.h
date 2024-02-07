#ifndef CONFIG_h
#define CONFIG_h

#include <TFT_eSPI.h>                   // Include for the display

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
const char* waterVolumeOptions[] = {"1.5L", "2L", "2.5L", "3L", "3.5L", "4L"};
const char* cupSizeOptions[] = {"200ml", "250ml", "300ml", "400ml", "500ml", "1000ml"};
static int waterVolumeOptionCount = sizeof(waterVolumeOptions) / sizeof(waterVolumeOptions[0]);
static int cupSizeOptionCount = sizeof(cupSizeOptions) / sizeof(cupSizeOptions[0]);

// long seating submenu options
const char* seatingHours[] = {"2min", "15min", "30min", "45min", "60min"};
static int seatingHoursCount = sizeof(seatingHours) / sizeof(seatingHours[0]);

// selection variables for water
static int selectedWaterVolume = 1000; // Index of the selected water volume
static int selectedCupSize = 200;      // Index of the selected cup size

// selection variables for long seating
static int selectedSeatingHour = 0;

// Track the selected option in sub-menus
static int currentOption = 0;      

State currentState = MAIN_MENU_CURRENT_BAR;
State lastMainMenuState = MAIN_MENU_CURRENT_BAR;      // Default to the first main menu item

// periodically update display
static unsigned long lastTimerUpdate = 0;
static const unsigned long timerUpdateInterval = 500; // Update the display every 1000 milliseconds (1 second)


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


unsigned int convertTimeToSeconds(const char* timeStr) {
    unsigned int timeInSeconds = 0; // Initialize time in seconds as unsigned int
    char* endPtr;

    // Check if the string contains "min" to indicate minutes
    if (strstr(timeStr, "min")) {
        // Convert the numeric part of the string to an unsigned integer
        unsigned int minutes = strtoul(timeStr, &endPtr, 10);
        // Convert minutes to seconds
        timeInSeconds = minutes * 60;
    }
    
    return timeInSeconds;
}


void waterTarget_SubmenuLayout(TFT_eSPI &_tft) {
  // First, display the "Set target" text at a fixed position
  _tft.setTextSize(2); 
  _tft.drawString("Set daily target", 5, 5, 2); 

  // Calculate the width of the text for the selected water volume option
  int optionTextWidth = _tft.textWidth(waterVolumeOptions[currentOption]);

  // Calculate the starting X position to center the text in the remaining screen space
  // The "remaining screen space" is considered to be the screen width minus any margins you want to keep
  int remainingScreenWidth = SCREENWIDTH - 20;                    // Assuming a 10-pixel margin on each side for example
  int startX = (remainingScreenWidth - optionTextWidth) / 2 + 10; // Adjust the '+ 10' if you change the margin

  // Calculate a Y position for the option text. Assuming you want to place it below the "Set target" text
  int optionTextY = 60; // This can be adjusted based on your layout

  // Set the text color for the option text if needed
  // _tft.setTextColor(TFT_WHITE, TFT_BLACK); // Example: White text with black background

  // Display the selected water volume option, centered within the remaining space
  _tft.drawString(waterVolumeOptions[currentOption], startX, optionTextY, 2);
}


void cupSize_SubmenuLayout(TFT_eSPI &_tft) {
  // First, display the "Set target" text at a fixed position
  _tft.setTextSize(2); 
  _tft.drawString("Set cup size", 5, 5, 2);

  // Calculate the width of the text for the selected water volume option
  int optionTextWidth = _tft.textWidth(cupSizeOptions[currentOption]);

  // Calculate the starting X position to center the text in the remaining screen space
  // The "remaining screen space" is considered to be the screen width minus any margins you want to keep
  int remainingScreenWidth = SCREENWIDTH - 20;                    // Assuming a 10-pixel margin on each side for example
  int startX = (remainingScreenWidth - optionTextWidth) / 2 + 10; // Adjust the '+ 10' if you change the margin

  // Calculate a Y position for the option text. Assuming you want to place it below the "Set Cup Size" text
  int optionTextY = 60; // This can be adjusted based on your layout

  // Set the text color for the option text if needed
  // _tft.setTextColor(TFT_WHITE, TFT_BLACK); // Example: White text with black background

  // Display the selected water volume option, centered within the remaining space
  _tft.drawString(cupSizeOptions[currentOption], startX, optionTextY, 2);
}


void seatingTime_SubmenuLayout(TFT_eSPI &_tft) {
  // First, display the "Set a timer" text at a fixed position
  _tft.setTextSize(2); 
  _tft.drawString("Set a timer", 5, 5, 2);

  // Calculate the width of the text for the selected water volume option
  int optionTextWidth = _tft.textWidth(seatingHours[currentOption]);

  // Calculate the starting X position to center the text in the remaining screen space
  // The "remaining screen space" is considered to be the screen width minus any margins you want to keep
  int remainingScreenWidth = SCREENWIDTH - 20;                    // Assuming a 10-pixel margin on each side for example
  int startX = (remainingScreenWidth - optionTextWidth) / 2 + 10; // Adjust the '+ 10' if you change the margin

  // Calculate a Y position for the option text. Assuming you want to place it below the "Set a timer" text
  int optionTextY = 60; // This can be adjusted based on your layout

  // Set the text color for the option text if needed
  // _tft.setTextColor(TFT_WHITE, TFT_BLACK); // Example: White text with black background

  // Display the selected water volume option, centered within the remaining space
  _tft.drawString(seatingHours[currentOption], startX, optionTextY, 2);
}


void displayTimerMenu(TFT_eSPI &_tft, OfficeTimer &_timer, bool alertReached) {
    String timerStr = _timer.getTimerString(); // The timer string, "00:00"

    // Set the desired font size for the timer display
    _tft.setTextSize(6);

    // Calculate the width and height of the text with the current font settings
    int textWidth = _tft.textWidth(timerStr);
    int textHeight = _tft.fontHeight();

    // Calculate the starting X and Y positions to center the text
    int startX = (_tft.width() - textWidth) / 2;
    int startY = (_tft.height() - textHeight) / 2;

    // Set the cursor to the calculated position
    _tft.setCursor(startX, startY);

    // Optionally, set the text color before drawing
    if(alertReached) {
      _tft.setTextColor(TFT_RED, TFT_BLACK); 
    } else {
      _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    }

    // Draw the text on the screen
    _tft.println(timerStr);
}

#endif