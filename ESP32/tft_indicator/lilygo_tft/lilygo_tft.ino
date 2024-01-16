#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#include <Button2.h>

#define BUTTON_1            35
#define BUTTON_2            0

#define LIGHT_GREEN 0x8FEA // Hexadecimal color code for light green


//display instance 
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
Button2 button(BUTTON_2);   // Create a Button2 object attached to pin 0


// Adjust these values based on your screen size and desired appearance
int barCount = 5;       // Number of total bars
int gap = 4;            // Gap between bars in pixels
int screenWidth = 240;  // Width of the screen in pixels
int screenHeight = 135; // Height of the screen in pixels

int barWidth = (screenWidth - (gap * (barCount - 1))) / barCount; // Calculate the width of each bar
int barHeight = screenHeight;                                     // Bar height will be full height of the screen

float currentLevel = 0.0;


void setup(void) {
  tft.init();
  tft.setRotation(1);
  
  button.setTapHandler(buttonClickHandler);
  drawBatteryIndicator(currentLevel);
}


void loop() {
  // Your loop code
  button.loop(); // Must call the loop function regularly
}


void buttonClickHandler(Button2& btn) {
  currentLevel += 0.5;                            // Increase level by half
  if (currentLevel > barCount) currentLevel = 0;  // Reset to 0 if it exceeds the bar count
  drawBatteryIndicator(currentLevel);
}


void drawBatteryIndicator(float chargeLevel) {
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

