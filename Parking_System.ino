#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- OLED Display Definitions ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Pin & Parking System Definitions ---
const int irEntryPin = 2;
const int irExitPin  = 3;

const int totalSlots = 5;
int availableSlots   = 5;

// Edge detection variables
bool lastEntryState = HIGH;
bool lastExitState  = HIGH;

// Function prototype for intro animation
void playStartupAnimation();
void updateOLEDDisplay();

void setup() {
  pinMode(irEntryPin, INPUT);
  pinMode(irExitPin, INPUT);

  // Initialize OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;); // Stop execution if display fails to initialize
  }

  // Run the "PARKING SYSTEM" text animation on boot
  playStartupAnimation();

  // Show live parking dashboard
  updateOLEDDisplay();
}

void loop() {
  bool currentEntryState = digitalRead(irEntryPin);
  bool currentExitState  = digitalRead(irExitPin);

  // --- ENTRY SENSOR DETECTED (Active LOW) ---
  if (lastEntryState == HIGH && currentEntryState == LOW) {
    if (availableSlots > 0) {
      availableSlots--;
      updateOLEDDisplay();
      delay(800); // Pass-through debounce delay
    } else {
      // Visual feedback when parking is full
      display.clearDisplay();
      display.setCursor(10, 20);
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.println("PARKING");
      display.setCursor(35, 40);
      display.println("FULL!");
      display.display();
      delay(1200);
      updateOLEDDisplay();
    }
  }

  // --- EXIT SENSOR DETECTED (Active LOW) ---
  if (lastExitState == HIGH && currentExitState == LOW) {
    if (availableSlots < totalSlots) {
      availableSlots++;
      updateOLEDDisplay();
      delay(800); // Pass-through debounce delay
    }
  }

  lastEntryState = currentEntryState;
  lastExitState  = currentExitState;
}

// --- Text Animation Routine ---
void playStartupAnimation() {
  // Step 1: Slide "PARKING" down from top and "SYSTEM" up from bottom
  for (int y = -15; y <= 10; y += 2) {
    display.clearDisplay();

    // Slide "PARKING" down
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(22, y);
    display.print("PARKING");

    // Slide "SYSTEM" up proportionally
    int systemY = 64 - (y + 15);
    if (systemY < 28) systemY = 28;
    display.setCursor(28, systemY);
    display.print("SYSTEM");

    display.display();
    delay(20);
  }

  // Step 2: Draw an animated loading bar beneath the text
  display.drawRoundRect(14, 50, 100, 8, 3, SSD1306_WHITE);
  display.display();

  for (int w = 0; w <= 96; w += 4) {
    display.fillRect(16, 52, w, 4, SSD1306_WHITE);
    display.display();
    delay(25);
  }

  delay(400); // Hold final frame before entering dashboard
}

// --- Main UI Dashboard ---
void updateOLEDDisplay() {
  display.clearDisplay();

  // Header Bar
  display.fillRect(0, 0, 128, 16, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(18, 4);
  display.print("PARKING SYSTEM");

  // Free Slots Count
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 24);
  display.print("FREE SLOTS:");

  display.setTextSize(3);
  display.setCursor(30, 36);
  display.print(availableSlots);
  display.setTextSize(2);
  display.print("/");
  display.print(totalSlots);

  // Status Indicator
  if (availableSlots == 0) {
    display.fillRect(0, 56, 128, 8, SSD1306_WHITE);
  } else {
    display.drawRect(0, 56, 128, 8, SSD1306_WHITE);
  }

  display.display();
}