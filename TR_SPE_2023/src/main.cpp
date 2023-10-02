#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>
#include <WiFi.h>
#include <time.h>

#define BUTTON_SW 2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SSD1306_I2C_ADDRESS 0x3C
Adafruit_AHTX0 aht;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
const char *ssid = "fAnus";      // please adjust
const char *password = "makanterus";  // please adjust

bool buttonState = false;
bool lastButtonState = false;
bool OLEDshow = false;

void readAHTSensor() {
  // Read sensor data
  sensors_event_t humidity, temperature;
  aht.getEvent(&humidity, &temperature);

  // Clear the display buffer
  display.clearDisplay();

  // Set text size, color, and display sensor data
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Display labels "Temp" and "Humid" above the data
  display.setCursor(0, 0); // Position for "Temp" label
  display.print(F("Temp: "));
  
  display.setCursor(70, 0); // Position for "Humid" label
  display.print(F("Humid: "));

  // Display the sensor data on the OLED screen (below the labels)
  display.setTextSize(2);
  display.setCursor(0, 15); // Position for temperature data
  display.print(temperature.temperature, 0); // Display temperature with 2 decimal places
  display.print((char)247);
  display.print(F("C"));

  display.setTextSize(2);
  display.setCursor(70, 15); // Position for humidity data
  display.print(humidity.relative_humidity, 0); // Display humidity with 2 decimal places
  display.print(F("%"));

  // Display the content on the screen
  display.display();
}

void showTime() {
  time_t currentTime = time(nullptr);
  struct tm *localTime = localtime(&currentTime);

  char formattedDate[20];
  strftime(formattedDate, sizeof(formattedDate), "%A, %d-%b-%Y", localTime); // Format for day, day-month-year

  char formattedTime[20];
  strftime(formattedTime, sizeof(formattedTime), "%H:%M:%S", localTime);

  Serial.println(formattedDate);
  Serial.println(formattedTime);

  // Clear the display buffer.
  display.clearDisplay();
  
  // Set text size, color, and center the text horizontally and vertically
  display.setTextSize(1);  // Decrease text size for date
  display.setTextColor(SSD1306_WHITE);
  int dateWidth = strlen(formattedDate) * 6;  // Adjust for the font width
  int dateX = (SCREEN_WIDTH - dateWidth) / 2;  // Center horizontally
  int dateY = 1;  // Place date at the top

  // Print the formatted date on the OLED display
  display.setCursor(dateX, dateY);
  display.println(formattedDate);

  display.setTextSize(2);  // Increase text size for time
  int textWidth = strlen(formattedTime) * 12;  // Adjust for the font width
  int x = (SCREEN_WIDTH - textWidth) / 2;  // Center horizontally
  int y = 16;  // Center vertically (assuming 16-pixel height font)

  // Print the formatted time on the OLED display
  display.setCursor(x, y);
  display.println(formattedTime);

  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  configTime(7 * 3600, 0, "pool.ntp.org"); // Set the UTC offset for Jakarta time and NTP server

  pinMode(BUTTON_SW, INPUT_PULLUP);

  // Initialize with the I2C oled
  if (!display.begin(SSD1306_I2C_ADDRESS)) { // Adjust the pins as needed
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  // Initialize AHT10 sensor
  if (!aht.begin()) {
    Serial.println(F("Couldn't find AHT10 sensor!"));
    for (;;) {}
  }
  // Clear the display buffer.
  display.clearDisplay();
  display.display();
}

void loop() {
  buttonState = digitalRead(BUTTON_SW);

  if (buttonState != lastButtonState) {
    // Button state has changed
    if (buttonState == LOW) {
      // Button is pressed
      OLEDshow = !OLEDshow;
      delay(100); // Debounce delay
    }
    lastButtonState = buttonState;
  }

  if (OLEDshow) {
    readAHTSensor();
  } else {
    showTime();
  }
}
