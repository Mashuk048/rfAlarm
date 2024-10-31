//-----------------------Callback function-------------------------------------//

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  
//-------------------------------------Getting config data---------------------//
StaticJsonDocument<128> myJsonDoc;
deserializeJson( myJsonDoc,payload,length );
const char* newdid =  myJsonDoc["did"];
unsigned long newstime = myJsonDoc["stime"];

Serial.printf("newstime = %d\n", newstime);


if(strcmp(did,newdid) == 0){
  if(newstime != NULL){
    stime = newstime*1000;//in ms
    Serial.printf("Snooze time updated by %d seconds\n", newstime);
    myJsonDoc["status"] = "success"; 
    char buffer[128];   
    size_t n = serializeJson(myJsonDoc, buffer);
    Serial.print("Published:");
    P_R_I_N_T(client.publish(resTopic, buffer, n));    
  }
  else{
    P_R_I_N_T("Invalid time");
//    StaticJsonDocument<128> doc;

//    doc["did"] = did;
    myJsonDoc["status"] = "failed";   
    char buffer[128]; 
    size_t n = serializeJson(myJsonDoc, buffer);
    Serial.print("Published:");
    P_R_I_N_T(client.publish(resTopic, buffer, n));
  }
  
}
else{
    myJsonDoc["status"] = "failed";   
    char buffer[128];
    size_t n = serializeJson(myJsonDoc, buffer);
    Serial.print("Published:");
    P_R_I_N_T(client.publish(resTopic, buffer, n));
}

     
}//Callback ends
