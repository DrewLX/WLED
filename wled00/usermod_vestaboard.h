#pragma once

#include "wled.h"

#define buttAPin 37
#define buttBPin 35
#define buttCPin 36
#define buttStartPin 12

enum gameState {
  holding,
  q1,
  q2,
  q3,
  q4,
  thanks
};

static AsyncClient * vestaClient = NULL;
char lstr[16];
const char* msg;

byte NotifyUpdateMode  = CALL_MODE_NO_NOTIFY; // notification mode for colorUpdated(): CALL_MODE_NO_NOTIFY or CALL_MODE_DIRECT_CHANGE

//class name. Use something descriptive and leave the ": public Usermod" part :)
class Vestaboard : public Usermod {
  private:
    //Private class members. You can declare variables and functions only accessible to your usermod here
    unsigned long lastTime = 0;
    unsigned long touchTime = 0;

    boolean buttA = false;
    boolean buttB = false;
    boolean buttC = false;
    boolean buttStart = false;

    gameState state = holding;

    const char* serverName = "https://platform.vestaboard.com/subscriptions/7eb5110e-e974-4b19-8ead-3f3b2c1127c6/message";


    // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
    String q1Copy;
    String q2Copy;
    String q3Copy;
    String holdingCopy;


  public:
    //Functions called by WLED

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() {
      Serial.println("Hello from my usermod!");

      // pinMode(buttAPin, INPUT_PULLUP);
      // pinMode(buttBPin, INPUT_PULLUP);
      // pinMode(buttCPin, INPUT_PULLUP);
      pinMode(buttStartPin, INPUT_PULLUP);
    }


    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    // void connected() {
    //   Serial.println("Connected to WiFi!");
    // }


    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     * 
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     */
    void loop() {
      if (millis() - lastTime > 100) {
        // Serial.println(digitalRead(buttStartPin));

        if (WLED_CONNECTED) {
          if (!digitalRead(buttStartPin) && !buttStart && state == holding) {
            buttStart = true;
            loadQuestion1();
          }

          
          if (digitalRead(buttStartPin)) {
            buttStart = false;
          }

        } else {
          // Not currently on a network!
        }

        lastTime = millis();
      }
      if (millis() - touchTime > 60000 && state != holding) {
        loadWelcomeScreen();
      }
    }

    void touch() {
      touchTime = lastTime;
    }

    void loadWelcomeScreen() {
      Serial.println("Load welcome screen");
      applyPreset(99, NotifyUpdateMode);
      sendMessage(holdingCopy);
      state = holding;
      touch();
    }

    void loadQuestion1() {
      Serial.println("Load initial question");
      applyPreset(91, NotifyUpdateMode);
      sendMessage(q1Copy);
      state = q1;
      touch();
    }

void sendMessage(String m){
  Serial.print("sendMessage begin: ");
  Serial.println(m);

  m = "{\"characters\":[" + m + "]}";

  char *message = new char[m.length() + 1];
  strcpy(message, m.c_str());

  // char *lines[5]; // our five lines of text
  // char *ptr = NULL;

  // byte index = 0;
  // ptr = strtok(message, "|");
  // while (ptr != NULL) {
  //   lines[index] = ptr;
  //   index++;
  //   ptr = strtok(NULL, "|");
  // }
  // Serial.println(lines[0]);


  msg = message;

  // // typedef struct keyAndValue_ {
  // //   char key;
  // //   String value;
  // // } vestaCharacters;

  // char vc[][2] = {{' ', '0'}, {'A', '1'}, {'B', '2'}, {'C', '3'}, {'D', '4'}, {'E', '5'}, {'F', '6'}, {'G', '7'}, {'H', '8'}, {'I', '9'}, {'J', '10'}, {'K', '11'}, {'L', '12'}, {'M', '13'}, {'N', '14'}, {'O', '15'}, {'P', '16'}, {'Q', '17'}, {'R', '18'}, {'S', '19'}, {'T', '20'}, {'U', '21'}, {'V', '22'}, {'W', '23'}, {'X', '24'}, {'Y', '25'}, {'Z', '26'}, {'1', '27'}, {'2', '28'}, {'3', '29'}, {'4', '30'}, {'5', '31'}, {'6', '32'}, {'7', '33'}, {'8', '34'}, {'9', '35'}, {'0', '36'}, {'!', '37'}, {'@', '38'}, {'#', '39'}, {'$', '40'}, {'(', '41'}, {')', '42'}, {'-', '44'}, {'+', '45'}, {'&', '47'}, {'=', '48'}, {';', '49'}, {'"', '52'}, {'\'', '53'}, {'%', '54'}, {',', '55'}, {'.', '56'}, {'/', '59'}, {'?', '60'}, {'r', '63'}, {'o', '64'}, {'y', '65'}, {'g', '66'}, {'b', '67'}, {'p', '68'}, {'w', '69'}};  

  // for (int i = 0; i < 5; i++) {
  //   String line = "[";
  //   line += "Hello";
  //   for (int j=0; j< 22; j++) {
  //     unsigned char source = lines[i][j];
      
  //     switch (source) {
  //       case 'A':
  //         line += "1,";
  //         break;

  //       default:
  //         line += "0";
  //     }
  //     Serial.print("  --  ");
  //     Serial.println(vc[source]);
  //   }
  //   // line += "]";
  //   // if (i<5) {
  //   //   line += ",";
  //   // }
  //   // Serial.println(line);
  // }

  // msg = "{\"characters\":[[63,64,65,66,67,68,69,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]]}";
  // Serial.println(msg);
  itoa(strlen(msg), lstr, 10);


  if(vestaClient) {
    Serial.println("Client already exists");
    return;
  }

  vestaClient = new AsyncClient();
  if(!vestaClient) {
    Serial.println("could not allocate a client");
    return;
  }

  vestaClient->onError([](void * arg, AsyncClient * client, int error){
    Serial.println("Connect Error");
    vestaClient = NULL;
    delete client;
  }, NULL);

  vestaClient->onConnect([](void * arg, AsyncClient * client){
    Serial.println("Connected");
    vestaClient->onError(NULL, NULL);

    client->onDisconnect([](void * arg, AsyncClient * c){
      Serial.println("Disconnected");
      vestaClient = NULL;
      delete c;
    }, NULL);

    client->onData([](void * arg, AsyncClient * c, void * data, size_t len){
      Serial.print("\r\nData: ");
      Serial.println(len);
      uint8_t * d = (uint8_t*)data;
      for(size_t i=0; i<len;i++)
        Serial.write(d[i]);
    }, NULL);

    //send the request


    client->write("POST /subscriptions/7eb5110e-e974-4b19-8ead-3f3b2c1127c6/message HTTP/1.1\r\n");
    client->write("Host: platform.vestaboard.com\r\n");
    client->write("X-Vestaboard-Api-Key: 23ec8147-cd58-4c5e-8ab9-a5deb5f99d97\r\n");
    client->write("X-Vestaboard-Api-Secret: ZGRjOWE4ZWYtNjk5OC00ZDQwLTllMDMtNmIzOTE4YzQ2MzFj\r\n");
    client->write("Connection: close\r\n");
    client->write("Content-Type: application/json\r\n");
    client->write("Content-Length: ");
    client->write(lstr);
    client->write("\r\n\r\n");
    client->write(msg);
    client->write("\r\n\r\n"); 

  }, NULL);

  if(!vestaClient->connect("platform.vestaboard.com", 80)){
    Serial.println("Connect Fail");
    AsyncClient * client = vestaClient;
    vestaClient = NULL;
    delete client;
  }
}


    /*
     * addToConfig() can be used to add custom persistent settings to the cfg.json file in the "um" (usermod) object.
     * It will be called by WLED when settings are actually saved (for example, LED settings are saved)
     * If you want to force saving the current state, use serializeConfig() in your loop().
     * 
     * CAUTION: serializeConfig() will initiate a filesystem write operation.
     * It might cause the LEDs to stutter and will cause flash wear if called too often.
     * Use it sparingly and always in the loop, never in network callbacks!
     * 
     * addToConfig() will make your settings editable through the Usermod Settings page automatically.
     *
     * Usermod Settings Overview:
     * - Numeric values are treated as floats in the browser.
     *   - If the numeric value entered into the browser contains a decimal point, it will be parsed as a C float
     *     before being returned to the Usermod.  The float data type has only 6-7 decimal digits of precision, and
     *     doubles are not supported, numbers will be rounded to the nearest float value when being parsed.
     *     The range accepted by the input field is +/- 1.175494351e-38 to +/- 3.402823466e+38.
     *   - If the numeric value entered into the browser doesn't contain a decimal point, it will be parsed as a
     *     C int32_t (range: -2147483648 to 2147483647) before being returned to the usermod.
     *     Overflows or underflows are truncated to the max/min value for an int32_t, and again truncated to the type
     *     used in the Usermod when reading the value from ArduinoJson.
     * - Pin values can be treated differently from an integer value by using the key name "pin"
     *   - "pin" can contain a single or array of integer values
     *   - On the Usermod Settings page there is simple checking for pin conflicts and warnings for special pins
     *     - Red color indicates a conflict.  Yellow color indicates a pin with a warning (e.g. an input-only pin)
     *   - Tip: use int8_t to store the pin value in the Usermod, so a -1 value (pin not set) can be used
     *
     * See usermod_v2_auto_save.h for an example that saves Flash space by reusing ArduinoJson key name strings
     * 
     * If you need a dedicated settings page with custom layout for your Usermod, that takes a lot more work.  
     * You will have to add the setting to the HTML, xml.cpp and set.cpp manually.
     * See the WLED Soundreactive fork (code and wiki) for reference.  https://github.com/atuline/WLED
     * 
     * I highly recommend checking out the basics of ArduinoJson serialization and deserialization in order to use custom settings!
     */
    void addToConfig(JsonObject& root)
    {
      JsonObject top = root.createNestedObject("Vestaboard_Config");
      // top["great"] = userVar0; //save these vars persistently whenever settings are saved
      top["q1_copy"] = q1Copy;
      top["q2_copy"] = q2Copy;
      top["q3_copy"] = q3Copy;
      top["holding_copy"] = holdingCopy;
    }


    /*
     * readFromConfig() can be used to read back the custom settings you added with addToConfig().
     * This is called by WLED when settings are loaded (currently this only happens immediately after boot, or after saving on the Usermod Settings page)
     * 
     * readFromConfig() is called BEFORE setup(). This means you can use your persistent values in setup() (e.g. pin assignments, buffer sizes),
     * but also that if you want to write persistent values to a dynamic buffer, you'd need to allocate it here instead of in setup.
     * If you don't know what that is, don't fret. It most likely doesn't affect your use case :)
     * 
     * Return true in case the config values returned from Usermod Settings were complete, or false if you'd like WLED to save your defaults to disk (so any missing values are editable in Usermod Settings)
     * 
     * getJsonValue() returns false if the value is missing, or copies the value into the variable provided and returns true if the value is present
     * The configComplete variable is true only if the "exampleUsermod" object and all values are present.  If any values are missing, WLED will know to call addToConfig() to save them
     * 
     * This function is guaranteed to be called on boot, but could also be called every time settings are updated
     */
    bool readFromConfig(JsonObject& root)
    {
      // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
      // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)

      JsonObject top = root["Vestaboard_Config"];

      bool configComplete = !top.isNull();

      // A 3-argument getJsonValue() assigns the 3rd argument as a default value if the Json value is missing
      configComplete &= getJsonValue(top["q1_copy"], q1Copy, "[17,27,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]");
      configComplete &= getJsonValue(top["q2_copy"], q2Copy, "[17,28,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]");
      configComplete &= getJsonValue(top["q3_copy"], q3Copy, "[17,29,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]");
      configComplete &= getJsonValue(top["holding_copy"], holdingCopy, "[8,15,12,4,9,14,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]");

      return configComplete;
    }

};