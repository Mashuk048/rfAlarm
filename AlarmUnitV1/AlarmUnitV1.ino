
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();


#define alarmPin 0 //D3
#define dataIn 2 //D4  ONBOARD LED
#define ledPin 14//D5
//#define sosButton 12//D6
const byte sosButton = 12;//D6
volatile byte interruptCounter = 0;


void ICACHE_RAM_ATTR ISR(){
  // ISR code
//  interruptCounter++;
  
  digitalWrite(alarmPin, LOW);
  digitalWrite(ledPin, LOW);
}



void setup() {
  pinMode(alarmPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
//  pinMode(sosButton, INPUT);
  pinMode(sosButton, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sosButton), ISR, FALLING);
  Serial.begin(115200);
  Serial.println("RX READY");
  delay(500);
  mySwitch.enableReceive(dataIn);  // Receiver on interrupt 0 => that is pin #2 
}

void loop() {
  if (mySwitch.available()) {
    
    Serial.print("Received ");

    unsigned long data = mySwitch.getReceivedValue();
    
    Serial.print( data );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );


    
    if(data == 5555){
      Serial.println("Matched");
      setAlarm();
    }
    else{
      Serial.println("Didn't match");
    }

    mySwitch.resetAvailable();
  }

  
    
}//end



void setAlarm(){
  Serial.println("Alarming...");
  digitalWrite(alarmPin, HIGH);
  digitalWrite(ledPin, HIGH);
}
