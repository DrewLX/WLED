#pragma once

#include "wled.h"
#include <U8x8lib.h> // from https://github.com/olikraus/u8g2/

// Extra char (+1) for null
#define LINE_BUFFER_SIZE            16+1

class EE_UIPANEL : public Usermod {

  private:
    bool initDone = false;
    unsigned long lastTime = 0;

    U8X8 *u8x8 = nullptr;           // pointer to U8X8 display object
    int8_t ioPin[5] = {15, 14, -1, -1, -1};        // I2C pins: SCL, SDA
    uint32_t ioFrequency = 400000;  // in Hz (minimum is 100000, baseline is 400000 and maximum should be 3400000)

    uint8_t lineHeight = 1;         // 1 row or 2 rows
    uint32_t refreshRate = 1000; // in ms

    // Next variables hold the previous known values to determine if redraw is
    // required.
    String knownSsid = "";
    IPAddress knownIp;
    uint8_t knownBrightness = 0;
    uint8_t knownMinute = 99;
    uint8_t knownHour = 99;
    uint8_t knownPreset = -1;

    bool displayTurnedOff = false;
    unsigned long lastUpdate = 0;
    unsigned long lastRedraw = 0;
    unsigned long overlayUntil = 0;

    // strings to reduce flash memory usage (used more than twice)
    static const char _name[];
    static const char _refreshRate[];
    static const char _busClkFrequency[];


  public:

    // gets called once at boot. Do all initialization that doesn't depend on
    // network here
    void setup() {
      DEBUG_PRINTLN(F("Setting up Garlic Bread UI Panel"));

      PinOwner po = PinOwner::UM_FourLineDisplay;
      PinManagerPinType pins[2] = { { ioPin[0], true }, { ioPin[1], true } };
      if (ioPin[0]==15 && ioPin[1]==14) po = PinOwner::HW_I2C;  // allow multiple allocations of HW I2C bus pins
      if (!pinManager.allocateMultiplePins(pins, 2, po)) { return; }
      
      DEBUG_PRINTLN(F("Allocating display."));
          
      u8x8 = (U8X8 *) new U8X8_SSD1306_128X64_NONAME_HW_I2C(U8X8_PIN_NONE, ioPin[0], ioPin[1]); // Pins are Reset, SCL, SDA

      if (nullptr == u8x8) {
          DEBUG_PRINTLN(F("Display init failed."));
          pinManager.deallocateMultiplePins((const uint8_t*)ioPin, 2, po);
          return;
      }

      initDone = true;
      DEBUG_PRINTLN(F("Starting display."));
      u8x8->setBusClock(ioFrequency);  // can be used for SPI too
      u8x8->begin();
      setPowerSave(0);
      drawString(0, 0, "Loading...");
    }

    // gets called every time WiFi is (re-)connected. Initialize own network
    // interfaces here
    void connected() {}

    /**
     * Da loop.
     */
    void loop() {
      if ((millis() - lastUpdate) < refreshRate || strip.isUpdating()) return;
      lastUpdate = millis();
      redraw(false);
      showTime();
    }

    /**
     * Wrappers for screen drawing
     */
    void drawString(uint8_t col, uint8_t row, const char *string, bool ignoreLH=false) {
      u8x8->setFont(u8x8_font_chroma48medium8_r);
      if (!ignoreLH && lineHeight==2) u8x8->draw1x2String(col, row, string);
      else                            u8x8->drawString(col, row, string);
    }
    void draw2x2String(uint8_t col, uint8_t row, const char *string) {
      u8x8->setFont(u8x8_font_chroma48medium8_r);
      u8x8->draw2x2String(col, row, string);
    }
    void drawGlyph(uint8_t col, uint8_t row, char glyph, const uint8_t *font, bool ignoreLH=false) {
      u8x8->setFont(font);
      if (!ignoreLH && lineHeight==2) u8x8->draw1x2Glyph(col, row, glyph);
      else                            u8x8->drawGlyph(col, row, glyph);
    }
    uint8_t getCols() {
      return u8x8->getCols();
    }
    void clear() {
      u8x8->clear();
    }
    void setPowerSave(uint8_t save) {
      u8x8->setPowerSave(save);
    }

    void center(String &line, uint8_t width) {
      int len = line.length();
      if (len<width) for (byte i=(width-len)/2; i>0; i--) line = ' ' + line;
      for (byte i=line.length(); i<width; i++) line += ' ';
    }

    /**
     * Redraw the screen (but only if things have changed
     * or if forceRedraw).
     */
    void redraw(bool forceRedraw) {
      static bool showName = false;
      unsigned long now = millis();

      if (overlayUntil > 0) {
        if (now >= overlayUntil) {
          // Time to display the overlay has elapsed.
          overlayUntil = 0;
          forceRedraw = true;
        } else {
          // We are still displaying the overlay
          // Don't redraw.
          return;
        }
      }

      // Check if values which are shown on display changed from the last time.
      if (forceRedraw ||
          (((apActive) ? String(apSSID) : WiFi.SSID()) != knownSsid) ||
          (knownIp != (apActive ? IPAddress(4, 3, 2, 1) : Network.localIP())) ||
          (knownBrightness != bri) ||
          knownPreset != currentPreset) {
        knownHour = 99;   // force time update
        lastRedraw = now; // update lastRedraw marker
      } else {
        return;
      }

      DEBUG_PRINTLN(F("Drawing display!"));

      // Update last known values.
      knownSsid = apActive ? WiFi.softAPSSID() : WiFi.SSID();
      knownIp = apActive ? IPAddress(4, 3, 2, 1) : Network.localIP();
      knownBrightness = bri;

      char lineBuffer[LINE_BUFFER_SIZE];

      u8x8->setFont(u8x8_font_torussansbold8_r);
      u8x8->setInverseFont(true);
      drawString(0, 0, "  GARLIC BREAD  ");
      u8x8->setInverseFont(false);
      u8x8->setFont(u8x8_font_chroma48medium8_r);

      // Do the actual drawing
      String line;
      // First row with Wifi name
      drawGlyph(0, 1, 80, u8x8_font_open_iconic_embedded_1x1); // home icon
      line = knownSsid.substring(0, getCols() > 1 ? getCols() - 2 : 0);
      center(line, getCols()-2);
      drawString(1, 1, line.c_str());
      // Print `~` char to indicate that SSID is longer, than our display
      if (knownSsid.length() > (int)getCols()-1) {
        drawString(getCols() - 1, 0, "~");
      }

      // Second row with IP or Psssword
      drawGlyph(0, 2, 68, u8x8_font_open_iconic_embedded_1x1); // wifi icon
      // Print password in AP mode and if led is OFF.
      if (apActive) {
        sprintf_P(lineBuffer, PSTR(" Pass: %s"), apPass);
        drawString(1, 2, lineBuffer);
      } else {
        // alternate IP address and server name
        line = knownIp.toString();
        if (showName && strcmp(serverDescription, "WLED") != 0) {
          line = serverDescription;
        }
        center(line, getCols()-1);
        drawString(1, 2, line.c_str());
      }

      if (bri > 1) {
        drawGlyph(0, 4, 69, u8x8_font_open_iconic_weather_1x1); // sun icon
        sprintf_P(lineBuffer, PSTR("Brightness %3d"), bri*100/255);
      } else {
        drawGlyph(0, 4, 66, u8x8_font_open_iconic_weather_1x1); // moon icon
        sprintf_P(lineBuffer, PSTR("Output Off    "));
      }
        drawString(2, 4, lineBuffer);

      knownPreset = currentPreset;
      if (currentPreset > 0) {
        sprintf_P(lineBuffer, PSTR("Preset  # %d    "), currentPreset);
        drawString(2, 5, lineBuffer);
        drawGlyph(0, 5, 68, u8x8_font_open_iconic_weather_1x1);  
      } else {
        sprintf_P(lineBuffer, PSTR("! Unsaved Preset"));
        drawString(0, 5, lineBuffer);
      }


      #ifdef WLED_ENABLE_DMX
        drawGlyph(0, 7, 64, u8x8_font_open_iconic_check_1x1);
        drawString(2, 7, "DMX");
      #endif
    }



    /**
     * Allows you to show up to two lines as overlay for a
     * period of time.
     * Clears the screen and prints on the middle two lines.
     */
    void overlay(const char* line1, const char *line2, long showHowLong) {
      clear();

      // Print the overlay
      if (line1) {
        String buf = line1;
        center(buf, getCols());
        drawString(0, 1*lineHeight, buf.c_str());
      }
      if (line2) {
        String buf = line2;
        center(buf, getCols());
        drawString(0, 2*lineHeight, buf.c_str());
      }
      overlayUntil = millis() + showHowLong;
    }

   

// draws the time in the bottom right corner of the screen.
    void showTime() {
      if (ntpLastSyncTime == 999000000) {
        return;
      }
      updateLocalTime();
      byte minuteCurrent = minute(localTime);
      byte hourCurrent   = hour(localTime);
      if (knownMinute == minuteCurrent && knownHour == hourCurrent) {
        return;
      }
      char lineBuffer[LINE_BUFFER_SIZE];
      knownMinute = minuteCurrent;
      knownHour = hourCurrent;

      sprintf_P(lineBuffer, PSTR("%2d:%02d"), hourCurrent, minuteCurrent);
      drawString(11, 7, lineBuffer);
    }

    /*
     * addToJsonInfo() can be used to add custom entries to the /json/info part of the JSON API.
     * Creating an "u" object allows you to add custom key/value pairs to the Info section of the WLED web UI.
     * Below it is shown how this could be used for e.g. a light sensor
     */
    //void addToJsonInfo(JsonObject& root) {
      //JsonObject user = root["u"];
      //if (user.isNull()) user = root.createNestedObject("u");
      //JsonArray data = user.createNestedArray(F("4LineDisplay"));
      //data.add(F("Loaded."));
    //}

    /*
     * addToJsonState() can be used to add custom entries to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    //void addToJsonState(JsonObject& root) {
    //}

    /*
     * readFromJsonState() can be used to receive data clients send to the /json/state part of the JSON API (state object).
     * Values in the state object may be modified by connected clients
     */
    //void readFromJsonState(JsonObject& root) {
    //  if (!initDone) return;  // prevent crash on boot applyPreset()
    //}

    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     * 
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     * 
     * addToConfig() will also not yet add your setting to one of the settings pages automatically.
     * To make that work you still have to add the setting to the HTML, xml.cpp and set.cpp manually.
     * 
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    // void addToConfig(JsonObject& root) {
    //   JsonObject top   = root.createNestedObject(FPSTR(_name));
    //   JsonArray io_pin = top.createNestedArray("pin");
    //   for (byte i=0; i<5; i++) io_pin.add(ioPin[i]);
    //   top["help4Pins"]           = F("Clk,Data,CS,DC,RST"); // help for Settings page
    //   top["help4Type"]           = F("1=SSD1306,2=SH1106,3=SSD1306_128x64,4=SSD1305,5=SSD1305_128x64,6=SSD1306_SPI,7=SSD1306_SPI_128x64"); // help for Settings pag
    
    //   top[FPSTR(_refreshRate)]   = refreshRate/1000;
    //   top[FPSTR(_busClkFrequency)] = ioFrequency/1000;
    //   DEBUG_PRINTLN(F("4 Line Display config saved."));
    // }

    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens once immediately after boot)
     * 
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     */
    // bool readFromConfig(JsonObject& root) {
    //   bool needsRedraw    = false;
    //   int8_t newPin[5]; for (byte i=0; i<5; i++) newPin[i] = ioPin[i];

    //   JsonObject top = root[FPSTR(_name)];
    //   if (top.isNull()) {
    //     DEBUG_PRINT(FPSTR(_name));
    //     DEBUG_PRINTLN(F(": No config found. (Using defaults.)"));
    //     return false;
    //   }

    //   for (byte i=0; i<5; i++) newPin[i] = top["pin"][i] | ioPin[i];
    
    
    //   refreshRate   = (top[FPSTR(_refreshRate)] | refreshRate/1000) * 1000;


    //   ioFrequency = min(3400, max(100, (int)(top[FPSTR(_busClkFrequency)] | ioFrequency/1000))) * 1000;  // limit frequency

    //   DEBUG_PRINT(FPSTR(_name));
    //   if (!initDone) {
    //     // first run: reading from cfg.json
    //     for (byte i=0; i<5; i++) ioPin[i] = newPin[i];
    //     DEBUG_PRINTLN(F(" config loaded."));
    //   } else {
    //     DEBUG_PRINTLN(F(" config (re)loaded."));
    //     // changing parameters from settings page
    //     bool pinsChanged = false;
    //     for (byte i=0; i<5; i++) if (ioPin[i] != newPin[i]) { pinsChanged = true; break; }
    //     if (pinsChanged) {
    //       delete u8x8;
    //       PinOwner po = PinOwner::UM_FourLineDisplay;
    
    //       pinManager.deallocateMultiplePins((const uint8_t *)ioPin, 2, po);
    //       for (byte i=0; i<5; i++) ioPin[i] = newPin[i];
    //       if (ioPin[0]<0 || ioPin[1]<0) { // data & clock must be > -1
    //         return true;
    //       }
    //       setup();
    //       needsRedraw |= true;
    //     }
    //     if (!(false)) u8x8->setBusClock(ioFrequency); // can be used for SPI too
    
    
    //     if (needsRedraw && !wakeDisplay()) redraw(true);
    //   }
    //   // use "return !top["newestParameter"].isNull();" when updating Usermod with new features
    // }

    /*
     * getId() allows you to optionally give your V2 usermod an unique ID (please define it in const.h!).
     * This could be used in the future for the system to determine whether your usermod is installed.
     */
    uint16_t getId() {
      return USERMOD_ID_FOUR_LINE_DISP;
    }
};

// strings to reduce flash memory usage (used more than twice)
const char EE_UIPANEL::_name[]            PROGMEM = "4LineDisplay";
const char EE_UIPANEL::_refreshRate[]     PROGMEM = "refreshRateSec";
const char EE_UIPANEL::_busClkFrequency[] PROGMEM = "i2c-freq-kHz";
