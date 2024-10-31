/*
 Since my debouncing code works on interrupt on CHANGE, so whenever it was missing the second interrupt(change of level shifting), it would make the "state" variable remain stuck as TRUE and long press detection loop runs endlessly making the device unstable.
 So to solve this, here i am forcefully altering the state to false=0(Depressed) inside the long press detecton loop to stop it from running endlessly.
 Device behaviour: Now any infirm or improper button press will set off the long press detection loop and once counter reaches the threshold(after 3 seconds), it stops the siren that was mistakenly fired up.
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();



// Connect to the WiFi
const char* ssid = "DataSoft_WiFi";//DataSoft_WiFi
const char* password = "support123";
const char* mqtt_server = "broker.datasoft-bd.com";
const int mqttPort = 1883;
const char* did = "GP101";

//mqtt topic
const char* snzTopic = "dsiot/gp/snooze";
const char* resTopic = "dsiot/gp/gas";

#define DEBUG 1

#if DEBUG
#define  P_R_I_N_T(x)   Serial.println(x)
#else
#define  P_R_I_N_T(x)
#endif



//MQTT
unsigned long lastReconnectTime = 0;
WiFiClient espClient;
PubSubClient client(espClient);


#define alarmPin 0 //D3
#define dataIn 2 //D4  ONBOARD LED
#define ledPin 14//D5
const int  sosButton = 12;//D6


//for RF msg
int msgCounter=0;



// Holds the current button state.
volatile int state = 1;
//volatile bool isPRESSED = false;

// Holds the last time debounce was evaluated (in millis).
volatile long lastDebounceTime = 0;

// The delay threshold for debounce checking.
const int debounceDelay = 30;


//for long press detection
unsigned long lastPressedTime = 0;

//for alarm pause timeout detection
unsigned long lastStopTime;
volatile bool SNOOZE = false;
unsigned long stime = 300000; //in ms.  5 minutes snooze time default

//For preventing realarming
bool isAlarming = false;

// Gets called by the interrupt.
void   ICACHE_RAM_ATTR   ISR() {
  // Get the pin reading.
  int reading = digitalRead(sosButton);

  // Ignore dupe readings.
  if(reading == state) return;

  boolean debounce = false;
  
  // Check to see if the change is within a debounce delay threshold.
  if((millis() - lastDebounceTime) <= debounceDelay) {
    debounce = true;
  }

  // This update to the last debounce check is necessary regardless of debounce state.
  lastDebounceTime = millis();

  // Ignore reads within a debounce delay threshold.
  if(debounce) return;

  // All is good, persist the reading as the state.
  state = reading;
//  isPRESSED = state;// if TRUE(HIGH),  then start counting after resetting counter
//  counter = 0;

  if(!state){//if button pin is read high
    if(isAlarming){P_R_I_N_T("Already Alarming!");}else{setAlarm_and_Publish("SOS",1);}         
    lastPressedTime = millis();    
  }
  
// Work with the value now.
//  Serial.println("button: " + String(reading));
  
}//ISR ENDS




void setup() {
    pinMode(alarmPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(sosButton, INPUT);
  //  pinMode(sosButton, INPUT_PULLUP);

    pinMode(alarmPin, LOW);//to avoid making sound at startup
  
    attachInterrupt(sosButton, ISR,  CHANGE);
    Serial.begin(115200);
    P_R_I_N_T("RX READY");
    delay(500);
    
    mySwitch.enableReceive(dataIn);  // Receiver on interrupt 0 => that is pin #2 

    setup_wifi();
    setup_mqtt();
}


void loop() {

    while(!state){          
      setLongPressTimer();
      yield();
    }

    if(SNOOZE){checkResumeTimer();}else{readRadioData();};
    
    
    setMqttReconnectInterval();
    
}//LOOP ENDS







void setup_wifi() {
    // We start by connecting to a WiFi network
    Serial.print("Connecting to ");
    P_R_I_N_T(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
      delay(500);
      Serial.print(".");
    }
  randomSeed(micros());
  P_R_I_N_T("");
  P_R_I_N_T("WiFi connected");
  P_R_I_N_T("IP address: ");
  P_R_I_N_T(WiFi.localIP());
}

void setup_mqtt(){      
   client.setServer(mqtt_server, mqttPort);
   client.setCallback(callback);
}

void readRadioData(){
    if (mySwitch.available()) {
    
    Serial.print("Received ");

    unsigned long data = mySwitch.getReceivedValue();
    
    Serial.print( data );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    P_R_I_N_T( mySwitch.getReceivedProtocol() );


    msgCounter++;
    P_R_I_N_T(msgCounter);
    if(msgCounter >= 1){
        if(data == 5555){
          P_R_I_N_T("Matched");
          
          if(isAlarming){P_R_I_N_T("Already Alarming!");}else{setAlarm_and_Publish("RF",1);}         
          
        }
        else{
          P_R_I_N_T("Didn't match");
        }

        msgCounter = 0;
    }
    mySwitch.resetAvailable();    
  } 
}


void setAlarm_and_Publish(const char *type, const int state){
  
  digitalWrite(alarmPin, state);
  digitalWrite(ledPin, state);
  
  StaticJsonDocument<128> doc;
  char buffer[128];
  doc["did"] = did;
  doc["alarm"] = state;
  doc["type"] = type;
  size_t n = serializeJson(doc, buffer);
  Serial.print("Published:");
  P_R_I_N_T(client.publish(resTopic, buffer, n));
  delay(250);
  isAlarming = true;
}


void setMqttReconnectInterval(){      
    if (!client.connected()) {
      unsigned long now = millis();
      if (now - lastReconnectTime > 5000) {
          lastReconnectTime = now;
          P_R_I_N_T("Ticking every 5 seconds");

          //write your own code

//          P_R_I_N_T(WiFi.status());

          if(WiFi.status() == WL_CONNECTED){
              // Attempt to mqtt reconnect
              if (reconnect()) {
                lastReconnectTime = 0;//GOOD
              }
          }
        
      }//end of interval check
   }else{
      client.loop();
   }
  
}//setMqttReconnect


void setLongPressTimer(){

  unsigned long currentTime = millis();
  
  if(currentTime - lastPressedTime > 2000){
      
      setAlarm_and_Publish("SOS",0);
      Serial.printf("ALARM STOPPED FOR %d seconds\n",stime/1000);
      lastStopTime = millis();
      state = 1;//button state made UNPRESSED to make sure while loop does not go on if MCU missed the state change
      SNOOZE = true;
      isAlarming = false;
  }
        
}


void checkResumeTimer(){
  unsigned long now = millis();
  if(now - lastStopTime > stime){
      SNOOZE = false;//resume radio
  }
}

