// Adapted for the ESP32-S2-LCD-0.96 development board example from the library ESP_WiFiManager_Lite
// https://github.com/khoih-prog/ESP_WiFiManager_Lite
// https://github.com/Dammerhaft

#include "defines.h"
#include "Credentials.h"
#include "dynamicParams.h"

// Connecting the library to work with the display
#include <TFT_eSPI.h>
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI(); 

ESP_WiFiManager_Lite* ESP_WiFiManager;

// Style for the login page
#if USING_CUSTOMS_STYLE
const char NewCustomsStyle[] PROGMEM = "<style>div,input{padding:5px;font-size:1em;}input{width:95%;}body{text-align: center;}"\
"button{background-color:blue;color:white;line-height:2.4rem;font-size:1.2rem;width:100%;}fieldset{border-radius:0.3rem;margin:0px;}</style>";
#endif

// Output to the console of dynamic parameters, if enabled
#if USE_DYNAMIC_PARAMETERS
void displayCredentials()
{
  Serial.println(F("\nYour stored Credentials :"));

  for (uint16_t i = 0; i < NUM_MENU_ITEMS; i++)
  {
    Serial.print(myMenuItems[i].displayName);
    Serial.print(F(" = "));
    Serial.println(myMenuItems[i].pdata);
  }
}

// Data output in the main loop
void displayCredentialsInLoop()
{
  static bool displayedCredentials = false;

  if (!displayedCredentials)
  {
    for (int i = 0; i < NUM_MENU_ITEMS; i++)
    {
      if (!strlen(myMenuItems[i].pdata))
      {
        break;
      }

      if ( i == (NUM_MENU_ITEMS - 1) )
      {
        displayedCredentials = true;
        displayCredentials();
      }
    }
  }
}
#endif

// Display initialization
void initScreen()
{

  // Display backlight activation
  pinMode(45, OUTPUT);
  digitalWrite(45, HIGH);

  // Display initialization
  tft.init();
  tft.setRotation(1);

  // Filling the display with black
  tft.fillScreen(TFT_BLACK);
}

// Data output to the display
void drawScreen()
{

  // Set text color, background color, font size
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);

  // If wifi is connected
  if (WiFi.status() == WL_CONNECTED)
    {
      // Display fill black
      tft.fillScreen(TFT_BLACK);

      // Positioning the cursor at 0, 0 on the display, font size 2
      tft.setCursor(0, 0, 2);

      // Display text
      tft.println(" Status: Connected");
      tft.println(" " + WiFi.SSID() + " (" + WiFi.RSSI() + ")");
      tft.println(" " + ESP_WiFiManager->localIP());
      tft.println(" " + WiFi.macAddress());
    }
  else
  {

    // If the device is in hotspot mode
    if (ESP_WiFiManager->isConfigMode())
    {
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 1, 2);
      tft.println("Status: AP");
      tft.println(AP_SSID);
      tft.println(AP_PWD);
    }

    // If the device is disconnected from the Wi-Fi network
    else
    {
      tft.fillScreen(TFT_BLACK);
      tft.setCursor(0, 1, 2);
      tft.println("Status: Disonnected");
    }
  }
}

// Timers for updating data on the display
void loopTimers()
{
  static unsigned long checkstatus_timeout = 0;

  //KH
  #define HEARTBEAT_INTERVAL    20000L
  // Print hearbeat every HEARTBEAT_INTERVAL (20) seconds.
  if ((millis() > checkstatus_timeout) || (checkstatus_timeout == 0))
  {
    drawScreen();
    checkstatus_timeout = millis() + HEARTBEAT_INTERVAL;
  }
}

// Initialization
void setup()
{
  // Display initialization
  initScreen();

  // Port initialization
  Serial.begin(115200);

  delay(200);

  // Output to the console data about the device
  Serial.print(F("\nStarting ESP_WiFi using ")); Serial.print(FS_Name);
  Serial.print(F(" on ")); Serial.println(ARDUINO_BOARD);
  Serial.println(ESP_WIFI_MANAGER_LITE_VERSION);

  // Information about the ESP_MultiResetDetector library
  #if USING_MRD  
    Serial.println(ESP_MULTI_RESET_DETECTOR_VERSION);
  #else
    Serial.println(ESP_DOUBLE_RESET_DETECTOR_VERSION);
  #endif

  // Creating an instance of ESP_WiFiManager_Lite
  ESP_WiFiManager = new ESP_WiFiManager_Lite();
  
  // Set customized AP SSID and PWD
  ESP_WiFiManager->setConfigPortal(AP_SSID, AP_PWD);

  // Optional to change default AP IP(192.168.4.1) and channel(10)
  //ESP_WiFiManager->setConfigPortalIP(IPAddress(192, 168, 120, 1));
  ESP_WiFiManager->setConfigPortalChannel(0);

  // Using custom css style
  #if USING_CUSTOMS_STYLE
    ESP_WiFiManager->setCustomsStyle(NewCustomsStyle);
  #endif

  #if USING_CUSTOMS_HEAD_ELEMENT
    ESP_WiFiManager->setCustomsHeadElement(PSTR("<style>html{filter: invert(10%);}</style>"));
  #endif

  #if USING_CORS_FEATURE  
    ESP_WiFiManager->setCORSHeader(PSTR("Your Access-Control-Allow-Origin"));
  #endif

  // Set customized DHCP HostName
  ESP_WiFiManager->begin(HOST_NAME);

  drawScreen();
}

// Main loop
void loop()
{
  ESP_WiFiManager->run();
  loopTimers();

  #if USE_DYNAMIC_PARAMETERS
    displayCredentialsInLoop();
  #endif

}
