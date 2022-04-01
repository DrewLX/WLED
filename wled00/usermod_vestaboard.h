#pragma once

#include "wled.h"

#define buttAPin 13
#define buttBPin 14
#define buttCPin 15
#define buttStartPin 34 // has button on Olimex board

#define presetHolding 50
#define presetLoading 60
#define presetWaitingForAnswer 70
#define presetProcessingAnswers 80

enum gameState {
  holding,
  q1,
  q2,
  q3,
  coffee
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

    int random = 1;
    int laterPreset = 0;
    boolean later = false;
    int laterCounter = 0;

    int debounceTime;
    int laterCounterMultiplyer;
    int touchTimeout;

    boolean buttA = false;
    boolean buttB = false;
    boolean buttC = false;
    boolean buttStart = false;

    gameState state = holding;

    const char* serverName = "https://platform.vestaboard.com/subscriptions/7eb5110e-e974-4b19-8ead-3f3b2c1127c6/message";

    String answer1 = "";
    String answer2 = "";
    String answer3 = "";


    // set your config variables to their boot default value (this can also be done in readFromConfig() or a constructor if you prefer)
    String q1Copy;
    String q2Copy;
    String q3Copy;
    String holdingCopy;
    String coffeeCopy1;
    String coffeeCopy2;
    String coffeeCopy3;
    String coffeeCopy4;
    String coffeeCopy5;
    String coffeeCopy6;
    String coffeeCopy7;
    String coffeeCopy8;
    String coffeeCopy9;
    String coffeeCopy10;
    String coffeeCopy11;
    String coffeeCopy12;
    String coffeeCopy13;
    String coffeeCopy14;
    String coffeeCopy15;
    String coffeeCopy16;
    String coffeeCopy17;
    String coffeeCopy18;
    String coffeeCopy19;
    String coffeeCopy20;
    String coffeeCopy21;
    String coffeeCopy22;
    String coffeeCopy23;
    String coffeeCopy24;


  public:
    //Functions called by WLED

    /*
     * setup() is called once at boot. WiFi is not yet connected at this point.
     * You can use it to initialize variables, sensors or similar.
     */
    void setup() {
      Serial.print("Vestaboard Usermod Setup... ");
      Serial.println(buttStartPin);

      pinMode(buttAPin, INPUT_PULLUP);
      pinMode(buttBPin, INPUT_PULLUP);
      pinMode(buttCPin, INPUT_PULLUP);
      pinMode(buttStartPin, INPUT_PULLUP);
    }


    /*
     * connected() is called every time the WiFi is (re)connected
     * Use it to initialize network interfaces
     */
    void connected() {
      Serial.println("Connected to WiFi!");
      loadWelcomeScreen(); // on boot and connection
    }


    /*
     * loop() is called continuously. Here you can check for events, read sensors, etc.
     * 
     * Tips:
     * 1. You can use "if (WLED_CONNECTED)" to check for a successful network connection.
     */
    void loop() {
      if (millis() - lastTime > debounceTime) {

        if (WLED_CONNECTED) {

          if (later) {
            laterCounter++;
            if (laterCounter > laterCounterMultiplyer) {
              finishEventuallyApplyPreset();
            }
          } else {

            // start button press
            if (!digitalRead(buttStartPin) && !buttStart && state == holding) {
              buttStart = true;
              loadQuestion1();
            }
            if (!digitalRead(buttStartPin) && !buttStart && state == coffee) {
              loadWelcomeScreen();
            }

            // Question 1 button presses
            if (!digitalRead(buttAPin) && !buttA && state == q1) {
              buttA = true;
              answer1 = "A";
              Serial.println("Question 1 Answered: A");
              loadQuestion2();
            } else if (!digitalRead(buttBPin) && !buttB && state == q1) {
              buttB = true;
              answer1 = "B";
              Serial.println("Question 1 Answered: B");
              loadQuestion2();
            } else if (!digitalRead(buttCPin) && !buttC && state == q1) {
              buttC = true;
              answer1 = "C";
              Serial.println("Question 1 Answered: C");
              loadQuestion2();
            }

            // Question 2 presses:
            if (!digitalRead(buttAPin) && !buttA && state == q2) {
              buttA = true;
              answer2 = "A";
              Serial.println("Question 2 Answered: A");
              loadQuestion3();
            } else if (!digitalRead(buttBPin) && !buttB && state == q2) {
              buttB = true;
              answer2 = "B";
              Serial.println("Question 2 Answered: B");
              loadQuestion3();
            }

            // Question 3 presses:
            if (!digitalRead(buttAPin) && !buttA && state == q3) {
              buttA = true;
              answer3 = "A";
              Serial.println("Question 3 Answered: A");
              finishQuiz();
            } else if (!digitalRead(buttBPin) && !buttB && state == q3) {
              buttB = true;
              answer3 = "B";
              Serial.println("Question 3 Answered: B");
              finishQuiz();
            }
          }
          
          // track button ups:
          if (digitalRead(buttStartPin)) {
            buttStart = false;
          }
          if (digitalRead(buttAPin)) {
            buttA = false;
          }
          if (digitalRead(buttBPin)) {
            buttB = false;
          }
          if (digitalRead(buttCPin)) {
            buttC = false;
          }
        } else {
          // Not currently on a network!
        }

        lastTime = millis();
      }

      if (millis() - touchTime > touchTimeout && state != holding) {
        Serial.println("Timeout");
        loadWelcomeScreen();
      }
    }

    void touch() {
      touchTime = lastTime;
    }

    void finishQuiz() {
      Serial.println("Answers --  1: " + answer1 + "   2: " + answer2 + "   3: " + answer3);
      touch();
      state = coffee;

      if (answer1 == "A" && answer2 == "A" && answer3 == "A") {
        if (random == 1) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(1);
          sendMessage(coffeeCopy1);    
        } else if (random == 2) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(2);
          sendMessage(coffeeCopy2);
        } else if (random == 3) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(3);
          sendMessage(coffeeCopy3);
        }
        iterateRandom();
      }

      if (answer1 == "A" && answer2 == "B" && answer3 == "A") {
        if (random == 1) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(10);
          sendMessage(coffeeCopy10);    
        } else if (random == 2) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(11);
          sendMessage(coffeeCopy11);
        } else if (random == 3) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(12);
          sendMessage(coffeeCopy12);
        }
        iterateRandom();
      }

      if (answer1 == "A" && answer2 == "A" && answer3 == "B") {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(19);
          sendMessage(coffeeCopy19);    
      }

      if (answer1 == "A" && answer2 == "B" && answer3 == "B") {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(22);
          sendMessage(coffeeCopy22);    
      }

      if (answer1 == "B" && answer2 == "A" && answer3 == "A") {
        if (random == 1) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(4);
          sendMessage(coffeeCopy4);    
        } else if (random == 2) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(5);
          sendMessage(coffeeCopy5);
        } else if (random == 3) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(6);
          sendMessage(coffeeCopy6);
        }
        iterateRandom();
      }

      if (answer1 == "B" && answer2 == "B" && answer3 == "A") {
        if (random == 1) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(13);
          sendMessage(coffeeCopy13);    
        } else if (random == 2) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(14);
          sendMessage(coffeeCopy14);
        } else if (random == 3) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(15);
          sendMessage(coffeeCopy15);
        }
        iterateRandom();
      }

      if (answer1 == "B" && answer2 == "A" && answer3 == "B") {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(20);
          sendMessage(coffeeCopy20);    
      }

      if (answer1 == "B" && answer2 == "B" && answer3 == "B") {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(23);
          sendMessage(coffeeCopy23);    
      }
      
      if (answer1 == "C" && answer2 == "A" && answer3 == "A") {
        if (random == 1) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(7);
          sendMessage(coffeeCopy7);    
        } else if (random == 2) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(8);
          sendMessage(coffeeCopy8);
        } else if (random == 3) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(9);
          sendMessage(coffeeCopy9);
        }
        iterateRandom();
      }

      if (answer1 == "C" && answer2 == "B" && answer3 == "A") {
        if (random == 1) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(16);
          sendMessage(coffeeCopy16);    
        } else if (random == 2) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(17);
          sendMessage(coffeeCopy17);
        } else if (random == 3) {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(18);
          sendMessage(coffeeCopy18);
        }
        iterateRandom();
      }

      if (answer1 == "C" && answer2 == "A" && answer3 == "B") {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(21);
          sendMessage(coffeeCopy21);    
      }

      if (answer1 == "C" && answer2 == "B" && answer3 == "B") {
          doPreset(presetProcessingAnswers);
          eventuallyApplyPreset(24);
          sendMessage(coffeeCopy24);    
      }
      

    }

    void iterateRandom() {
      random++;
      if (random > 3) {
        random = 1;
      }
    }

    void eventuallyApplyPreset(int preset) {
      laterPreset = preset;
      later = true;
      laterCounter = 0;
    }

    void finishEventuallyApplyPreset() {
      doPreset(laterPreset);
      later = false;
    }

    void doPreset(int p) {
      applyPreset(p, NotifyUpdateMode);
      Serial.print("Loading preset: ");
      Serial.println(p);
    }

    void loadWelcomeScreen() {
      Serial.println("Load welcome screen");
      doPreset(presetHolding);
      sendMessage(holdingCopy);
      state = holding;
      touch();
    }

    void loadQuestion1() {
      Serial.println("Load question 1");
      doPreset(presetLoading);
      eventuallyApplyPreset(presetWaitingForAnswer);
      sendMessage(q1Copy);
      state = q1;
      touch();
    }

    void loadQuestion2() {
      Serial.println("Load question 2");
      doPreset(presetLoading);
      eventuallyApplyPreset(presetWaitingForAnswer);
      sendMessage(q2Copy);
      state = q2;
      touch();
    }

    void loadQuestion3() {
      Serial.println("Load question 3");
      doPreset(presetLoading);
      eventuallyApplyPreset(presetWaitingForAnswer);
      sendMessage(q3Copy);
      state = q3;
      touch();
    }

  void sendMessage(String m){
  Serial.print("sendMessage begin: ");
  m = "{\"characters\":[" + m + "]}";
  Serial.println(m);


  char *message = new char[m.length() + 1];
  strcpy(message, m.c_str());

  msg = message;

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
      Serial.println("Command sent & client disconnected.");
      vestaClient = NULL;
      delete c;
    }, NULL);

    client->onData([](void * arg, AsyncClient * c, void * data, size_t len){
      Serial.print("\r\nVestaboard Server Response Data: ");
      Serial.println(len);
      uint8_t * d = (uint8_t*)data;
      for(size_t i=0; i<len;i++)
        Serial.write(d[i]);
    }, NULL);

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

  void addToConfig(JsonObject& root)
    {
      JsonObject coffee = root.createNestedObject("Vestaboard_Coffee");
      coffee["coffee_1"] = coffeeCopy1;
      coffee["coffee_2"] = coffeeCopy2;
      coffee["coffee_3"] = coffeeCopy3;
      coffee["coffee_4"] = coffeeCopy4;
      coffee["coffee_5"] = coffeeCopy5;
      coffee["coffee_6"] = coffeeCopy6;
      coffee["coffee_7"] = coffeeCopy7;
      coffee["coffee_8"] = coffeeCopy8;
      coffee["coffee_9"] = coffeeCopy9;
      coffee["coffee_10"] = coffeeCopy10;
      coffee["coffee_11"] = coffeeCopy11;
      coffee["coffee_12"] = coffeeCopy12;
      coffee["coffee_13"] = coffeeCopy13;
      coffee["coffee_14"] = coffeeCopy14;
      coffee["coffee_15"] = coffeeCopy15;
      coffee["coffee_16"] = coffeeCopy16;
      coffee["coffee_17"] = coffeeCopy17;
      coffee["coffee_18"] = coffeeCopy18;
      coffee["coffee_19"] = coffeeCopy19;
      coffee["coffee_20"] = coffeeCopy20;
      coffee["coffee_21"] = coffeeCopy21;
      coffee["coffee_22"] = coffeeCopy22;
      coffee["coffee_23"] = coffeeCopy23;
      coffee["coffee_24"] = coffeeCopy24;

      JsonObject top = root.createNestedObject("Vestaboard_Config");
      top["q1_copy"] = q1Copy;
      top["q2_copy"] = q2Copy;
      top["q3_copy"] = q3Copy;
      top["holding_copy"] = holdingCopy;

      JsonObject timing = root.createNestedObject("Vestaboard_Timing");
      timing["debounce_time"] = debounceTime;
      timing["timeout_duration"] = touchTimeout;
      timing["preset_delay_multiplyer"] = laterCounterMultiplyer;
      
    }

  bool readFromConfig(JsonObject& root)
    {
      // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
      // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)

      JsonObject top = root["Vestaboard_Config"];
      bool configComplete = !top.isNull();

      // A 3-argument getJsonValue() assigns the 3rd argument as a default value if the Json value is missing
      configComplete &= getJsonValue(top["debounce_time"], debounceTime, 100);
      configComplete &= getJsonValue(top["timeout_duration"], touchTimeout, 45000);
      configComplete &= getJsonValue(top["preset_delay_multiplyer"], laterCounterMultiplyer, 80);


      configComplete &= getJsonValue(top["q1_copy"], q1Copy, "[23,8,1,20,0,6,12,1,22,15,21,18,0,14,15,20,5,19,0,0,0,0],[4,15,0,25,15,21,0,12,9,11,5,0,9,14,0,25,15,21,18,0,0,0],[3,15,6,6,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[18,1,0,3,8,15,3,15,12,1,20,5,18,18,18,18,18,18,18,18,18,18],[7,2,0,3,1,18,1,13,5,12,7,7,7,7,7,7,7,7,7,7,7,7],[2,3,0,6,18,21,9,20,25,2,2,2,2,2,2,2,2,2,2,2,2,2]");
      configComplete &= getJsonValue(top["q2_copy"], q2Copy, "[8,15,23,0,4,15,0,25,15,21,0,20,1,11,5,0,25,15,21,18,0,0],[3,15,6,6,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[18,1,0,13,9,12,11,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18],[7,2,0,2,12,1,3,11,7,7,7,7,7,7,7,7,7,7,7,7,7,7],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]");
      configComplete &= getJsonValue(top["q3_copy"], q3Copy, "[3,1,6,6,5,9,14,1,20,5,4,0,15,18,0,4,5,3,1,6,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[18,1,0,3,1,6,6,5,9,14,1,20,5,4,18,18,18,18,18,18,18,18],[7,2,0,4,5,3,1,6,7,7,7,7,7,7,7,7,7,7,7,7,7,7],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]");
      configComplete &= getJsonValue(top["holding_copy"], holdingCopy, "[16,18,5,19,19,0,19,20,1,18,20,0,20,15,0,1,14,19,23,5,18,0],[20,8,18,5,5,0,17,21,5,19,20,9,15,14,19,0,1,14,4,0,0,0],[23,5,52,12,12,0,8,5,12,16,0,25,15,21,0,20,15,0,0,0,0,0],[4,9,19,3,15,22,5,18,0,23,8,9,3,8,0,15,6,0,20,8,5,0],[21,11,52,19,0,2,5,19,20,0,2,5,1,14,19,0,1,18,5,0,0,0],[16,5,18,6,5,3,20,0,6,15,18,0,25,15,21,0,0,0,0,0,0,0]");

      JsonObject coffee = root["Vestaboard_Coffee"];

      configComplete &= getJsonValue(coffee["coffee_1"], coffeeCopy1, "");
      configComplete &= getJsonValue(coffee["coffee_2"], coffeeCopy2, "");
      configComplete &= getJsonValue(coffee["coffee_3"], coffeeCopy3, "");
      configComplete &= getJsonValue(coffee["coffee_4"], coffeeCopy4, "");
      configComplete &= getJsonValue(coffee["coffee_5"], coffeeCopy5, "");
      configComplete &= getJsonValue(coffee["coffee_6"], coffeeCopy6, "");
      configComplete &= getJsonValue(coffee["coffee_7"], coffeeCopy7, "");
      configComplete &= getJsonValue(coffee["coffee_8"], coffeeCopy8, "");
      configComplete &= getJsonValue(coffee["coffee_9"], coffeeCopy9, "");
      configComplete &= getJsonValue(coffee["coffee_10"], coffeeCopy10, "");
      configComplete &= getJsonValue(coffee["coffee_11"], coffeeCopy11, "");
      configComplete &= getJsonValue(coffee["coffee_12"], coffeeCopy12, "");
      configComplete &= getJsonValue(coffee["coffee_13"], coffeeCopy13, "");
      configComplete &= getJsonValue(coffee["coffee_14"], coffeeCopy14, "");
      configComplete &= getJsonValue(coffee["coffee_15"], coffeeCopy15, "");
      configComplete &= getJsonValue(coffee["coffee_16"], coffeeCopy16, "");
      configComplete &= getJsonValue(coffee["coffee_17"], coffeeCopy17, "");
      configComplete &= getJsonValue(coffee["coffee_18"], coffeeCopy18, "");
      configComplete &= getJsonValue(coffee["coffee_19"], coffeeCopy19, "");
      configComplete &= getJsonValue(coffee["coffee_20"], coffeeCopy20, "");
      configComplete &= getJsonValue(coffee["coffee_21"], coffeeCopy21, "");
      configComplete &= getJsonValue(coffee["coffee_22"], coffeeCopy22, "");
      configComplete &= getJsonValue(coffee["coffee_23"], coffeeCopy23, "");
      configComplete &= getJsonValue(coffee["coffee_24"], coffeeCopy24, "");

      return configComplete;
    }

};