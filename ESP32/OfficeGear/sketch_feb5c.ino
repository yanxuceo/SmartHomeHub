#include <TFT_eSPI.h>        // Include for the display

#include "ButtonHandler.h"
#include "TimerDisplay.h"


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


TFT_eSPI tft = TFT_eSPI();   // Initialize TFT


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
static int selectedCupSize = 200;     // Index of the selected cup size

// selection variables for long seating
static int selectedSeatingHour = 0;

// Track the selected option in sub-menus
static int currentOption = 0;      

State currentState = MAIN_MENU_CURRENT_BAR;
State lastMainMenuState = MAIN_MENU_CURRENT_BAR; // Default to the first main menu item


#define LIGHT_GREEN 0x8FEA // Hexadecimal color code for light green

// Adjust these values based on your screen size and desired appearance
static int barCount = selectedWaterVolume/selectedCupSize;       // Number of total bars
static int gap = 4;                         // Gap between bars in pixels
static int screenWidth = 240;               // Width of the screen in pixels
static int screenHeight = 135;              // Height of the screen in pixels
static int barHeight = screenHeight;        // Bar height will be full height of the screen

static float currentWaterLevel = 0.0;

OfficeTimer timer;  // Create an OfficeTimer instance

ButtonHandler buttonA(35); // Example button on pin 35
ButtonHandler buttonB(0);  // Another button on pin 0


void setup() {
    Serial.begin(115200);
    buttonA.begin();
    buttonB.begin();

    tft.init();
    tft.setRotation(1);
    tft.setTextSize(3);  
    tft.fillScreen(TFT_BLACK);

    displayCurrentState();

    buttonA.setDoubleClickDelay(250);
    buttonB.setDoubleClickDelay(250);

    buttonA.onPressed(handle_buttonA_singleClick);
    buttonA.onDoublePressed(handle_buttonA_doubleClick);
    buttonA.onLongPressed(handle_buttonA_longClick);

    buttonB.onPressed(handle_buttonB_singleClick);
    buttonB.onDoublePressed(handle_buttonB_doubleClick);
    buttonB.onLongPressed(handle_buttonB_longClick);

    // Start the timer
    timer.start();
}

unsigned long lastTimerUpdate = 0;
const unsigned long timerUpdateInterval = 500; // Update the display every 1000 milliseconds (1 second)

void loop() {
    buttonA.update();
    buttonB.update();

    unsigned long currentMillis = millis();

    // Check if the Timer Menu is active and if it's time to update the display
    if (currentState == MAIN_MENU_TIMER && currentMillis - lastTimerUpdate >= timerUpdateInterval) {
        displayTimerMenu(); // Update the timer display
        lastTimerUpdate = currentMillis; // Reset the last update time
    }
}

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
  Serial.println("Button B clicked, confirm!");
  switch (currentState) {
    case MAIN_MENU_CURRENT_BAR:
      Serial.println("Increasing water level...");
      increaseWaterLevel(0.5);
      displayCurrentState(); // Refresh the display to show any changes.
      break;

    case MAIN_MENU_TIMER:
      Serial.println("Resetting timer...");
      // ResetTimer();        // This would reset the timer for the long-seating reminder.
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
      updatebarCount();
      // ApplyWaterVolumeSetting();         // Apply the setting as needed
      break;

    case SUB_MENU_CUP_SIZE:
      Serial.println("Cup Size Selected: " + String(cupSizeOptions[currentOption]));
      selectedCupSize = convertVolumeToMl(cupSizeOptions[currentOption]);

      Serial.print("converted cup size value: ");
      Serial.println(selectedCupSize); 
      updatebarCount();
      break;

    case SUB_MENU_SEATING_HOUR_THRESHOLD:
      Serial.println("Cup Size Selected: " + String(seatingHours[currentOption]));
      selectedSeatingHour = currentOption; // Save the selected cup size
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
  Serial.println("Button B Long Pressed, return to main menu");
   
  currentState = lastMainMenuState; // Return to the last main menu state
  currentOption = 0; 
  displayCurrentState(); // Refresh the display to show the main menu
}


void displayCurrentState() {
  tft.fillScreen(TFT_BLACK);  // Clear the screen
  tft.setTextSize(3); // Set a default font size for most menus

  switch (currentState) {
    case MAIN_MENU_CURRENT_BAR:
      drawBatteryIndicator(currentWaterLevel, barCount);
      break;
    case MAIN_MENU_TIMER:
      displayTimerMenu();
      break;
    case MAIN_MENU_SYSTEM:
      tft.drawString("System Info", 10, 10, 2);
      break;

    // We will add sub-menu displays later
    case SUB_MENU_WATER_VOLUME:
      tft.drawString("Select Water Volume", 10, 10, 2);
      tft.drawString(waterVolumeOptions[currentOption], 10, 50, 2);
      break;
    case SUB_MENU_CUP_SIZE:
      tft.drawString("Select Cup Size", 10, 10, 2);
      tft.drawString(cupSizeOptions[currentOption], 10, 50, 2);
      break;

    case SUB_MENU_SEATING_HOUR_THRESHOLD:
      tft.drawString("Select seating time", 10, 10, 2);
      tft.drawString(seatingHours[currentOption], 10, 50, 2);
      break;

    default:
      break;
  }
}

void increaseWaterLevel(float incresing_unit) {
  currentWaterLevel += incresing_unit;    // Increase level by half
  if (currentWaterLevel > barCount)       // Reset to 0 if it exceeds the bar count
    currentWaterLevel = 0;            
}
     
void drawBatteryIndicator(float chargeLevel, int barCount) {
    int barWidth = (screenWidth - (gap * (barCount - 1))) / barCount; 

    for (int i = 0; i < barCount; i++) {
        if (i < (int)chargeLevel) {
            // Fully fill the bar
            tft.fillRect(i * (barWidth + gap), 0, barWidth, barHeight, TFT_GREEN);
        } else {
            // Clear the bar if it's not supposed to be filled
            tft.fillRect(i * (barWidth + gap), 0, barWidth, barHeight, TFT_BLACK);

            if (i < chargeLevel) {
                // Half fill the bar horizontally
                tft.fillRect(i * (barWidth + gap), 0, barWidth / 2, barHeight, TFT_GREEN);
            }
        }
        // Draw vertical line for partition, if not the last bar
        if (i < barCount - 1) {
            tft.drawLine((i + 1) * (barWidth + gap) - 1, 0, (i + 1) * (barWidth + gap) - 1, barHeight, TFT_WHITE);
        }
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

void updatebarCount() {
  barCount = selectedWaterVolume/selectedCupSize;
  
  Serial.print("updated barCount value: ");
  Serial.println(barCount); 
}

/*
void displayTimerMenu() {
  String timerStr = timer.getTimerString();

  tft.setTextSize(6);
  
  tft.setCursor(0, 0);
  tft.println(timerStr);
}
*/

void displayTimerMenu() {
    String timerStr = timer.getTimerString(); // The timer string, "00:00"

    // Set the desired font size for the timer display
    tft.setTextSize(6);

    // Calculate the width and height of the text with the current font settings
    int textWidth = tft.textWidth(timerStr);
    int textHeight = tft.fontHeight();

    // Calculate the starting X and Y positions to center the text
    int startX = (tft.width() - textWidth) / 2;
    int startY = (tft.height() - textHeight) / 2;

    // Set the cursor to the calculated position
    tft.setCursor(startX, startY);

    // Optionally, set the text color before drawing
    tft.setTextColor(TFT_WHITE, TFT_BLACK); // White text with a black background to overwrite previous text

    // Draw the text on the screen
    tft.println(timerStr);
}


