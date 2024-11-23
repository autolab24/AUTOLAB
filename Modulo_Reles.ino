/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-one-to-many-esp32-esp8266/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

#include <esp_now.h>
#include <WiFi.h>

//Structure example to receive data
//Must match the sender structure
typedef struct test_struct {
  int x;
  int y;
} test_struct;

//Create a struct_message called myData
test_struct myData;
 int state[12] = {0,0,0,0,0,0,0,0,0,0,0,0};
 int ClassMode = 0;
//callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info* esp_now_info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("x: ");
  Serial.println(myData.x);
  Serial.print("y: ");
  Serial.println(myData.y);
  Serial.println();
}
 
void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  pinMode(25, OUTPUT);
  //digitalWrite(25, LOW);
  pinMode(26, OUTPUT);
  //digitalWrite(26, LOW);
  pinMode(32, OUTPUT);
  //digitalWrite(32, LOW);
  pinMode(33, OUTPUT);
  //digitalWrite(33, LOW);
  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

  switch(myData.x){

    //Modulo 1
    /*
    case 10: if(state[0] == 0){
              if(ClassMode == 0)
                digitalWrite(32, HIGH);
              state[0] = 1;
              }
              else{ 
                if(ClassMode == 0)           
                  digitalWrite(32, LOW);
                state[0] = 0;
              }
            myData.x = 0;       
            break;
    case 20: if(state[1] == 0){
              if(ClassMode == 0)
                digitalWrite(33, HIGH);
              state[1] = 1;
              }
             else{            
              if(ClassMode == 0)
                digitalWrite(33, LOW);
                state[1] = 0;
              }
            myData.x = 0;    
            break;
    case 30: if(state[2] == 0){
              if(ClassMode == 0)
                digitalWrite(25, HIGH);
              state[2] = 1;
              }
              else{            
                if(ClassMode == 0)
                 digitalWrite(25, LOW);
                state[2] = 0;
              }
            myData.x = 0;      
            break;
*/
    //Modulo 2

    case 40: if(state[3] == 0){
              if(ClassMode == 0)
                digitalWrite(32, HIGH);
              state[3] = 1;
              }
             else{            
               if(ClassMode == 0)
                digitalWrite(32, LOW);
              state[3] = 0;
              }
            myData.x = 0;     
            break;  
    case 50: if(state[4] == 0){
              if(ClassMode == 0)
               digitalWrite(33, HIGH);
              state[4] = 1;
              }
             else{            
              if(ClassMode == 0)
                digitalWrite(33, LOW);
                state[4] = 0;
              }
            myData.x = 0;       
            break;
    case 60: if(state[5] == 0){
              if(ClassMode == 0)
               digitalWrite(25, HIGH);
              state[5] = 1;
              }
             else{
              if(ClassMode == 0)            
               digitalWrite(25, LOW);
              state[5] = 0;
              }
            myData.x = 0;    
            break;

    //Modulo 3
/*
    case 70: if(state[6] == 0){
              if(ClassMode == 0)
               digitalWrite(32, HIGH);
              state[6] = 1;
              }
              else{            
                if(ClassMode == 0)
                 digitalWrite(32, LOW);
                state[6] = 0;
              }
            myData.x = 0;      
            break;
    case 80: if(state[7] == 0){
              if(ClassMode == 0)
              digitalWrite(33, HIGH);
              state[7] = 1;
              }
              else{            
                if(ClassMode == 0)
                 digitalWrite(33, LOW);
                state[7] = 0;
              }
            myData.x = 0;     
            break;     
    case 90: if(state[8] == 0){
              if(ClassMode == 0)
               digitalWrite(25, HIGH);
              state[8] = 1;
              }
              else{            
               if(ClassMode == 0)
                 digitalWrite(25, LOW);
                state[8] = 0;
              }
            myData.x = 0;       
            break;

    //modulo 4
    case 100: if(state[9] == 0){
                if(ClassMode == 0)
                 digitalWrite(32, HIGH);
                state[9] = 1;
              }
              else{            
                if(ClassMode == 0)
                 digitalWrite(32, LOW);
                state[9] = 0;
              }
            myData.x = 0;    
            break;
    case 110: if(state[10] == 0){
                if(ClassMode == 0)
                 digitalWrite(33, HIGH);
                state[10] = 1;
              }
              else{            
                if(ClassMode == 0)
                 digitalWrite(33, LOW);
                state[10] = 0;
              }
            myData.x = 0;      
            break;
    case 120: if(state[11] == 0){
                if(ClassMode == 0)
                 digitalWrite(25, HIGH);
                state[11] = 1;
              }
              else{            
                if(ClassMode == 0)
                 digitalWrite(25, LOW);
                state[11] = 0;
              }
            myData.x = 0;     
            break;
            */
    case 999: if(ClassMode == 0){
                digitalWrite(32, HIGH);
                digitalWrite(33, HIGH);
                digitalWrite(25, HIGH);
                ClassMode = 1;
              }
              else{            
                digitalWrite(32, LOW);
                digitalWrite(33, LOW);
                digitalWrite(25, LOW);
                ClassMode = 0;
              }
            myData.x = 0;   
            break;
   
    default:break;
  }
  delay(2000);
  
 

}