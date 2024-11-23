/*
由四川睿频科技有限公司编写
复制及使用请保留版权所属
*/
#include <esp_now.h>
#include <WiFi.h>
#include <HTTPClient.h>


// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = {0xCC, 0xDB, 0xA7, 0x32, 0x6A, 0xE4};
int count = 1;
typedef struct test_struct {
  int x;
  int y;
  char EPC_readed[200];
} test_struct;

test_struct test;
test_struct myData;

esp_now_peer_info_t peerInfo;

/*多次读取指令*/
#define WINDOW_SIZE 2000
unsigned char ReadMulti[10] = {0XAA,0X00,0X27,0X00,0X03,0X22,0XFF,0XFF,0X4A,0XDD};
unsigned int timeSec = 0;
unsigned int timemin = 0;
unsigned int dataAdd = 0;
unsigned int incomedate = 0;
unsigned int parState = 0;
unsigned int codeState = 0;

int LED_BUILTIN = 2;
String epcBuffer[WINDOW_SIZE];
String devicesBuffer[WINDOW_SIZE];
String currentEPC;
int epcCount = 0;
int deviceCount = 0;
int Read1 = 0, Read2 = 0;


const char* ssid = "Autolab"; //"Mario";   
const char* password = "Network24";//mario2024 Reemplaza con tu contraseña


String ip_database = "192.168.4.100";

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  Serial.print("Packet to: ");
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print(macStr);
  Serial.print(" send status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//callback function that will be executed when data is received
void OnDataRecv(const esp_now_recv_info* esp_now_info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Read1 = myData.x;
  Read2 = myData.y;
  Serial.print("x: ");
  Serial.println(myData.x);
  Serial.print("y: ");
  Serial.println(myData.y);
  Serial.print("String: ");
  Serial.println(myData.EPC_readed);

}
void setup() {
  //设置串口，并设置LED
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200); //设置串口波特率115200
  // Conectar a WiFi
  //WiFi.mode(WIFI_STA);
 // WiFi.softAP("ESP32_WebServer", "12345678", 6);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a la red WiFi");
  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  Serial.println(WiFi.channel());
  Serial.println("Hello world.");// "Hello world."
  Serial.write(ReadMulti,10);

  
 
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_send_cb(OnDataSent);
   
  // register peer
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // register first peer  
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  
  /*String response = consultarElementoPorEPC("EPC"+String(count));
  if(count < 20)count++;
  else count=1;
  delay(5000);*/

  if(Read1 == 150 && Read2 == 456){
      //间隔一段时间后发生循环读取命令
  timeSec ++ ;
  if(timeSec >= 50000){
    timemin ++;
    timeSec = 0;
    if(timemin >= 20){
      timemin = 0;
      //发送循环读取指令
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.write(ReadMulti,10);
      digitalWrite(LED_BUILTIN, LOW); 
    }
  }
 
  if(Serial.available() > 0)//串口接收到数据
  {
    //收到回复,以下是读到卡片示例
    //AA 02 22 00 11 C7 30 00 E2 80 68 90 00 00 50 0E 88 C6 A4 A7 11 9B 29 DD 
    /*
    AA:帧头
    02:指令代码
    22:指令参数
    00 11:指令数据长度，17个字节
    C7：RSSI信号强度
    30 00:标签PC码：标签厂相关信息登记
    E2 80 68 90 00 00 50 0E 88 C6 A4 A7：EPC代码
    11 9B:CRC校验
    29:校验
    DD:帧尾 
    */
    incomedate = Serial.read();//获取串口接收到的数据
    //判断是否为对应指令代码
    if((incomedate == 0x02)&(parState == 0))
    {
      parState = 1;
    }
    //判断是否为对应指令参数
    else if((parState == 1)&(incomedate == 0x22)&(codeState == 0)){  
        codeState = 1;
        dataAdd = 3;
    }
    else if(codeState == 1){
      dataAdd ++;
      //获取RSSI
      if(dataAdd == 6)
      {
        Serial.print("RSSI:"); 
        Serial.println(incomedate, HEX); 
        }
       //获取PC码
       else if((dataAdd == 7)|(dataAdd == 8)){
        if(dataAdd == 7){
          Serial.print("PC:"); 
          Serial.print(incomedate, HEX);
        }
        else {
           Serial.println(incomedate, HEX);
        }
       }
       //获取EPC，如需对EPC处理，可以在该处进行
       else if((dataAdd >= 9)&(dataAdd <= 20)){
        if(dataAdd == 9){
          Serial.print("EPC:"); 
        }     
        Serial.print(incomedate, HEX);
        currentEPC += String(incomedate, HEX);
        
        if(dataAdd == 20){
            
            int flag = 0;
            for(int i=0; i<epcCount; i++){
              if(currentEPC == epcBuffer[i]){
                flag = 1;
                break;
              }              
            }
            
            if(flag == 0){
              epcBuffer[epcCount] =  currentEPC;
              epcCount++;
              //consulta BD 
              String response = consultarElementoPorEPC(currentEPC);
              delay(100);
              if (response != "" ){
                 devicesBuffer[deviceCount] = response;
                 deviceCount++;

                 response.toCharArray(test.EPC_readed, 200);

                 test.x = random(0,20);
                 test.y = random(0,20);
           
                 esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(test_struct));
                
                 if (result == ESP_OK) {
                  Serial.println("Sent with success");
                 }
                 else {
                  Serial.println("Error sending the data");
                 }

                                  
              }              
              
            }
            
            currentEPC = "";
            Serial.println();
            Serial.println("EPCs stored:");

            for(int i = 0; i<epcCount; i++)
              Serial.println( epcBuffer[i] );

            Serial.println();
            Serial.println("Devices stored:");

            for(int i = 0; i<deviceCount; i++)
              Serial.println( devicesBuffer[i] );  

        }

       }
       //位置溢出，进行重新接收
       else if(dataAdd >= 21){
        Serial.println(" "); 
        dataAdd= 0;
        parState = 0;
        codeState = 0;
        }
    }
     else{
      dataAdd= 0;
      parState = 0;
      codeState = 0;
    }
  }

  }else{
          //Se reinicia los elementos almacenados
          epcCount = 0;
          deviceCount = 0;

  }
  
}

String consultarElementoPorEPC(String epc) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "http://" + ip_database + "/autolab_api/api.php?accion=consultar_elemento&epc=" + epc;
        http.begin(url.c_str());

        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            String response = http.getString();
            response.trim(); // Elimina los espacios en blanco alrededor de la respuesta
            //Serial.println("Respuesta del servidor: " + response);

            if (response != "Elemento no encontrado") {
                Serial.println("Elemento encontrado: " + response);
                return response;  // Devuelve el elemento encontrado
            } else {
                Serial.println("Elemento no encontrado");
                return "";
            }
        } else {
            Serial.print("Error en la solicitud GET: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("Error en la conexión WiFi");
    }

    return "";
}

