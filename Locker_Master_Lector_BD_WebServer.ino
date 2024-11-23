#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <esp_wifi.h>
#include <WebServer.h>
#include <esp_now.h>
#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>


#define SS_PIN 5
#define RST_PIN 22

const char* ssid = "Autolab";//"Mario";
const char* password = "Network24"; //"mario2024";

// Configuración de IP estática (usa comas en vez de puntos)
IPAddress local_IP(192, 168, 4, 200);         // La IP fija que quieres asignar
IPAddress gateway(192, 168, 4, 1);            // IP del router (puerta de enlace)
IPAddress subnet(255, 255, 0, 0);             // Máscara de subred
IPAddress primaryDNS(8, 8, 8, 8);             // DNS primario (opcional)
IPAddress secondaryDNS(8, 8, 4, 4);           // DNS secundario (opcional)

WebServer server(80); // Servidor en el puerto 80

const int relay1 = 12;  
const int relay2 = 13;
const int relay3 = 27;
const int relay4 = 14;
const int relay5 = 25;
const int relay6 = 26;  
const int relay7 = 32;
const int relay8 = 33;
const int relay9 = 2;
const int relay10 = 15;
const int relay11 = 16;  
const int relay12 = 4;

bool newCardRead = false; 
bool newCardReadTwo = false;
bool delivered = false;
bool Reading = false;
unsigned long tiempoDelay = 2000;
String ip_database ="192.168.4.100"; 
// ip ESP32: 172.20.10.3

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
String DatoHex;
const String UserReg_1 = "E30D28C5";
const String UserReg_2 = "930551F4";
const String UserReg_3 = "C3DF42C5";
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx    

  String nombre;
  String codigo_rfid;// = "EAD9D00E"; //EAD9D00C 
  String identificacion;
  String correo;
  String programa;
  String reservedBefore; 
  String deliveredDevices;

String devicesBuffer[5];
String PrestadoBuffer[5];
int deviceCount = 0;
int PrestadoCount = 0;
int reservedLocker = 0;
int reservedBeforeInt = 0;
int count = 1;
int flagRestart = 0;
int flagRead = 0;
int returnState = 0;
int lockerOn[12]={0,0,0,0,0,0,0,0,0,0,0,0};

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;
// Init array that will store new NUID
byte nuidPICC[4];

// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = {0x24, 0xDC, 0xC3, 0xA6, 0xBD, 0x28};
uint8_t broadcastAddress2[] = {0x24, 0xDC, 0xC3, 0xA6, 0xE0, 0x30};
uint8_t broadcastAddress3[] = {0x24, 0xDC, 0xC3, 0xA6, 0xE1, 0xEC};
uint8_t broadcastAddress4[] = {0x24, 0xDC, 0xC3, 0xA6, 0xDF, 0x50};
uint8_t broadcastAddress5[] = {0xFC, 0xB4, 0x67, 0xF6, 0x8A, 0x5C};

typedef struct test_struct {
  int x;
  int y;
  char EPC_readed[200];
} test_struct;

test_struct test;
test_struct myData;

esp_now_peer_info_t peerInfo;

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
  Serial.print("x: ");
  Serial.println(myData.x);
  Serial.print("y: ");
  Serial.println(myData.y);
  Serial.print("String: ");
  Serial.println(myData.EPC_readed);
  devicesBuffer[deviceCount] = myData.EPC_readed;
  deviceCount++;

}
// HTML para las diferentes pantallas
const char* welcomePage = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Bienvenida - AUTOLAB</title>
    <style>
        body { 
            text-align: center; 
            padding: 100px; 
            font-family: Arial, sans-serif; 
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #f5f5f5; /* Texto claro */
        }
        h1 {
            color: #b2945e; /* Color dorado suave para títulos */
            font-size: 5.5em;
            margin-bottom: 20px;
        }
        #status { 
            margin-top: 20px; 
            font-size: 4.2em;
           
        }
        p {
            margin: 10px 0;
            color: #f5f5f5; /* Mantener el texto en color claro */
        }
    </style>
</head>
<body>
    <h1>Bienvenido a AUTOLAB</h1>
    <p id="status">Por favor, coloque su carné en el lector.</p>
    <script>
    function checkForCard() {
      fetch('/checkCard')
       .then(response => response.text())
        .then(data => {
          if (data === '1') {
            window.location.href = '/user-info';
          } else if(data === '2'){
            window.location.href = '/register';
          } else {
            setTimeout(checkForCard, 2000);
          }
        });
    }
    checkForCard();
    </script>
</body>
</html>
)rawliteral";

const char* userInfoPage = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Información del usuario - AUTOLAB</title>
    <style>
        body { 
            text-align: center; 
            font-size: 1em;
            padding: 50px; 
            font-family: Arial, sans-serif; 
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #f5f5f5; /* Texto claro */
            margin: 0;
        }

        h1 {
            color: #b2945e; /* Color dorado suave para títulos */
            font-size: 5.5em;
            margin-bottom: 20px;
        }

        #user-info { 
            margin-top: 20px; 
            font-size: 4.2em;
        }

        p {
            margin: 10px 0;
            color: #f5f5f5; /* Mantener el texto en color claro */
        }

        button {
            padding: 10px 20px;
            margin-top: 30px;
            font-size: 36px;
            background-color: #b2945e; /* Botones dorados */
            color: #1c1c1c; /* Texto del botón en color oscuro */
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }

        button:hover {
            background-color: #a68f50; /* Efecto hover con un tono más oscuro */
        }

    </style>
</head>
<body>
    <h1>Información del usuario</h1>
    <div id="user-info">
        <p><strong>Nombre:</strong> %s</p>
        <p><strong>Identificación:</strong> %s</p>
        <p><strong>Correo:</strong> %s</p>
        <p><strong>Programa:</strong> %s</p>
    </div>
    <button onclick="finalize()">Continuar</button>
    <script>
        function finalize() {
            window.location.href = "/retirar";
        }
    </script>
    <button onclick="exitMain()">Salir</button>
    <script>
        function exitMain() {
            window.location.href = "/";
        }
    </script>
</body>
</html>
)rawliteral";

const char* visualizarPrestado = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Visualizar prestamo previo - AUTOLAB</title>
    <style>
        body { 
            text-align: center; 
            font-size: 1em;
            padding: 50px; 
            font-family: Arial, sans-serif; 
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #f5f5f5; /* Texto claro */
            margin: 0;
        }
        h1 {
            color: #b2945e; /* Color dorado suave para títulos */
            font-size: 5.5em;
            margin-bottom: 20px;
        }
        #item-list { 
            margin-top: 20px; 
            font-size: 4.2em;
        }
        p {
            margin: 10px 0;
            color: #f5f5f5; /* Mantener el texto en color claro */
        }

        button {
            padding: 10px 20px;
            margin-top: 30px;
            font-size: 36px;
            background-color: #b2945e; /* Botones dorados */
            color: #1c1c1c; /* Texto del botón en color oscuro */
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        button:hover {
            background-color: #a68f50; /* Efecto hover con un tono más oscuro */
            }
    </style>
</head>
<body>
    <h1>Prestamo vigente</h1>
    <div id="item-list">
        <p><strong>Elemento 1:</strong> %s</p>
        <p><strong>Elemento 2:</strong> %s</p>
        <p><strong>Elemento 3:</strong> %s</p>
        <p><strong>Elemento 4:</strong> %s</p>
        <p><strong>Elemento 5:</strong> %s</p>
    </div>
    <button onclick="confirmItems()">Devolver elementos</button>
    <script>
        function confirmItems() {
            window.location.href = "/previewItems";
        }
    </script>
    <button onclick="exitMain()">Salir</button>
    <script>
        function exitMain() {
            window.location.href = "/";
        }
    </script>
</body>
</html>
)rawliteral";

const char* registerPage = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Registro de Usuario - AUTOLAB</title>
    <style>
        body { 
            text-align: center; 
            padding: 50px; 
            font-family: Arial, sans-serif; 
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #d4af37; /* Color dorado */
        }
        h1 {
            color: #d4af37; /* Color dorado */
            font-size: 2.5em;
            margin-bottom: 20px;
        }
        form { 
            display: inline-block; 
            text-align: left; 
            background-color: #333333; /* Fondo más oscuro para el formulario */
            padding: 50px; 
            border-radius: 8px;
            box-shadow: 0px 0px 15px rgba(0, 0, 0, 0.5); /* Sombras doradas */
        }
        label {
            font-weight: bold;
            color: #d4af37; /* Color dorado para las etiquetas */
            display: block;
            margin: 10px 0 5px;
        }
        input, select {
            margin-bottom: 15px;
            width: 100%;
            padding: 10px;
            border: 1px solid #d4af37; /* Borde dorado */
            border-radius: 5px;
            font-size: 16px;
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #f4f4f9; /* Texto claro */
        }
        button {
            width: 100%;
            padding: 10px;
            background-color: #d4af37; /* Botón dorado */
            color: #1c1c1c; /* Texto oscuro en el botón */
            border: none;
            border-radius: 5px;
            cursor: pointer;
            font-size: 18px;
            font-weight: bold;
        }
        button:hover {
            background-color: #b8860b; /* Botón dorado más oscuro al pasar el cursor */
        }
    </style>
</head>
<body>
    <h1>REGISTRO DE USUARIO</h1>
    <form id="register-form" action="/register" method="post">
        <label for="nombre">Nombre:</label>
        <input type="text" id="nombre" name="nombre" required>
        
        <label for="identificacion">Documento de identidad:</label>
        <input type="text" id="identificacion" name="identificacion" required>
        
        <label for="correo">Correo:</label>
        <input type="email" id="correo" name="correo" required>
        
        <label for="programa">Programa:</label>
        <select id="programa" name="programa" required>
            <option value="Ing. Electrónica">Ing. Electrónica</option>
            <option value="Ing. Biomédica">Ing. Biomédica</option>
        </select>
        
        <button type="submit">Registrar</button>
    </form>
</body>
</html>
)rawliteral";

const char* confirmationRegister = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Registro Exitoso - AUTOLAB</title>
    <style>
        body { 
            text-align: center; 
            padding: 50px; 
            font-family: Arial, sans-serif; 
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #f5f5f5; /* Texto claro */
        }
        h1 {
            color: #b2945e; /* Color dorado suave para títulos */
            font-size: 5.5em;
            margin-bottom: 20px;
        }
        #confirmation { 
            margin-top: 20px; 
            font-size: 4.2em;
        }
        p {
            margin: 10px 0;
            color: #f5f5f5; /* Mantener el texto en color claro */
        }
        button {
            padding: 10px 20px;
            margin-top: 30px;
            font-size: 36px;
            background-color: #b2945e; /* Botones dorados */
            color: #1c1c1c; /* Texto del botón en color oscuro */
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        button:hover {
            background-color: #a68f50; /* Efecto hover con un tono más oscuro */
        }
    </style>
</head>
<body>
    <h1>REGISTRO EXITOSO</h1>
    <div id="confirmation">
        <p>El usuario se ha registrado correctamente.</p>
    </div>
    <button onclick="location.href='/retirar'">Continuar</button>
    <button onclick="location.href='/'">Salir</button>
</body>
</html>
)rawliteral";


const char* retirarElementosPage = R"rawliteral(

<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Retirar Elementos - AUTOLAB</title>
    <style>
        body { 
            text-align: center; 
            padding: 50px; 
            font-family: Arial, sans-serif; 
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #f5f5f5; /* Texto claro */
        }
        h1 {
            color: #b2945e; /* Color dorado suave para títulos */
            font-size: 5.5em;
            margin-bottom: 20px;
        }
        #retirar { 
            margin-top: 20px; 
            font-size: 2.5em;
        }
        ul {
            text-align: left;
            display: inline-block;
            font-size: 1.2em;
            margin: 20px 0;
            color: #f5f5f5; /* Mantener el texto en color claro */
        }
        li {
            margin: 20px 0;
        }
        button {
            padding: 10px 10px;
            margin-top: 5px;
            font-size: 26px;
            background-color: #b2945e; /* Botones dorados */
            color: #1c1c1c; /* Texto del botón en color oscuro */
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        button:hover {
            background-color: #a68f50; /* Efecto hover con un tono más oscuro */
        }
    </style>
</head>
<body>
    <h1>RETIRAR ELEMENTOS</h1>
    <div id="retirar">
        <p>NO CONTINUE hasta que:</p>
   
       <ul>
        <li> Abra el casillero asignado.</li>
        <li> Retire los elementos que requiere.</li>
        <li> Cierre el casillero.</li>
        <li> Presione el botón "Finalizar".</li>
    </ul>
</div>
    <button onclick="finalize()">Finalizar</button>
    <script>
        function finalize() {
            window.location.href = "/confirmation";
        }
    </script>
</body>
</html>
)rawliteral";

const char* returnItemsPage = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Devolución de Elementos - AUTOLAB</title>
    <style>
        body { 
            text-align: center; 
            padding: 50px; 
            font-family: Arial, sans-serif; 
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #f5f5f5; /* Texto claro */
        }
        h1 {
            color: #b2945e; /* Color dorado suave para títulos */
            font-size: 5.5em;
            margin-bottom: 20px;
        }
        #devolver { 
            margin-top: 20px; 
            font-size: 2.5em;
        }

        ul {
            text-align: left;
            display: inline-block;
            font-size: 1.2em;
            margin: 20px 0;
            color: #f5f5f5; /* Mantener el texto en color claro */
        }
        li {
            margin: 20px 0;
        }
        button {
            padding: 10px 20px;
            margin-top: 5px;
            font-size: 26px;
            background-color: #b2945e; /* Botones dorados */
            color: #1c1c1c; /* Texto del botón en color oscuro */
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        button:hover {
            background-color: #a68f50; /* Efecto hover con un tono más oscuro */
                }
    </style>
</head>
<body>
    <h1>DEVOLUCIÓN DE ELEMENTOS</h1>
    <div id="devolver">
        <p>Actualmente usted tiene un prestamo activo:</p>

       <ul>
        <li> Abra el casillero asignado.</li>
        <li> Ingrese al casillero los elementos.</li>
        <li> Cierre el casillero.</li>
        <li> Presione el botón "Finalizar devolución".</li>
    </ul>
</div>
    <button onclick="finalizeReturn()">Finalizar devolución</button>
    <script>
        function finalizeReturn() {
            window.location.href = "/confirmation";
        }
    </script>
</body>
</html>
)rawliteral";


const char* confirmationPage = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Confirmación de lectura - AUTOLAB</title>
    <style>
        body { 
            text-align: center; 
            font-size: 1em;
            padding: 50px; 
            font-family: Arial, sans-serif; 
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #f5f5f5; /* Texto claro */
            margin: 0;
        }
        h1 {
            color: #b2945e; /* Color dorado suave para títulos */
            font-size: 5.5em;
            margin-bottom: 20px;
        }
        #item-list { 
            margin-top: 20px; 
            font-size: 4.2em;
        }
        p {
            margin: 10px 0;
            color: #f5f5f5; /* Mantener el texto en color claro */
        }

        button {
            padding: 10px 20px;
            margin-top: 30px;
            font-size: 36px;
            background-color: #b2945e; /* Botones dorados */
            color: #1c1c1c; /* Texto del botón en color oscuro */
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        button:hover {
            background-color: #a68f50; /* Efecto hover con un tono más oscuro */
            }
    </style>
</head>
<body>
    <h1>Confirmación de lectura</h1>
    <div id="item-list">
        <p><strong>Elemento 1:</strong> %s</p>
        <p><strong>Elemento 2:</strong> %s</p>
        <p><strong>Elemento 3:</strong> %s</p>
        <p><strong>Elemento 4:</strong> %s</p>
        <p><strong>Elemento 5:</strong> %s</p>
    </div>
    <button onclick="confirmItems()">Confirmar elementos</button>
    <script>
        function confirmItems() {
            window.location.href = "/final";
        }
    </script>
</body>
</html>
)rawliteral";

const char* finalPage = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Notificación - AUTOLAB</title>
    <style>
        body { 
            text-align: center; 
            padding: 50px; 
            font-family: Arial, sans-serif; 
            background-color: #1c1c1c; /* Fondo oscuro */
            color: #f5f5f5; /* Texto claro */
        }
        h1 {
            color: #b2945e; /* Color dorado suave para títulos */
            font-size: 5.5em;
            margin-bottom: 20px;
        }
        p {
            font-size: 4.2em;
            margin: 10px 0;
            color: #f5f5f5; /* Mantener el texto en color claro */
        }
        button {
            padding: 10px 20px;
            margin-top: 30px;
            font-size: 36px;
            background-color: #b2945e; /* Botones dorados */
            color: #1c1c1c; /* Texto del botón en color oscuro */
            border: none;
            border-radius: 5px;
            cursor: pointer;
            transition: background-color 0.3s;
        }
        button:hover {
            background-color: #a68f50; /* Efecto hover con un tono más oscuro */
            }
    </style>
</head>
<body>
    <h1>%s</h1>
    <p>Muchas gracias por usar AUTOLAB, regrese pronto.</p>
    <button onclick="finalize()">Inicio</button>
    <script>
        function finalize() {
            window.location.href = "/";
        }
    </script>
</body>
</html>
)rawliteral";


void handleRoot() {
      for(int i = 0; i<PrestadoCount; i++ ){
      Serial.println(devicesBuffer[i]);
      PrestadoBuffer[i] = "";
    }
    flagRead = 0;
    PrestadoCount = 0;
    server.send(200, "text/html", welcomePage);
    //redirigir cuando se lea un nuevo carné
    
}

void handleCheckCard() {
  if (newCardRead) {
    server.send(200, "text/plain", "1");
    newCardRead = false; // Reset after detection
  } else if(newCardReadTwo){
    server.send(200, "text/plain", "2");
    newCardReadTwo = false; // Reset after detection
  }else {
    server.send(200, "text/plain", "0");
  }
}


void handleUserInfo() {
   char htmlBuffer[4056];  // Buffer para almacenar el HTML formateado

    // Formatear el HTML con los valores actuales de las variables
    sprintf(htmlBuffer, userInfoPage, nombre.c_str(), identificacion.c_str(), correo.c_str(), programa.c_str());

    // Enviar la página HTML al cliente
    server.send(200, "text/html", htmlBuffer);
}

void handleRegister() {
    if (server.method() == HTTP_GET) {
        server.send(200, "text/html", registerPage);
    } else if (server.method() == HTTP_POST) {
        // Aquí se manejaría el registro del usuario
        // Obtener datos del formulario:
         nombre = server.arg("nombre");
         identificacion = server.arg("identificacion");
         correo = server.arg("correo");
         programa = server.arg("programa");
         
         registrarUsuario(nombre, codigo_rfid, identificacion, correo, programa);

         server.send(200, "text/html", confirmationRegister);
    }
}
void handlePreview() {
            //envia señal a la uhf vía ESPNOW para que inicie la lectura
    
    test.x = 150;
    test.y = 456;

    esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(test_struct));
    
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }

    AccionCasillero(reservedBeforeInt);

    server.send(200, "text/html", returnItemsPage);
}

void handleRetirarElementos() {

    delay(100);

    if(delivered){
      char htmlBuffer[4056];  // Buffer para almacenar el HTML formateado
    // Formatear el HTML con los valores actuales de las variables
    sprintf(htmlBuffer, visualizarPrestado, PrestadoBuffer[0].c_str(), PrestadoBuffer[1].c_str(), PrestadoBuffer[2].c_str(), PrestadoBuffer[3].c_str(), PrestadoBuffer[4].c_str());
    
      server.send(200, "text/html", htmlBuffer);
      
    }else{
      server.send(200, "text/html", retirarElementosPage);
      asignarCasillero();
          //envia señal a la uhf vía ESPNOW para que inicie la lectura
    
    test.x = 150;
    test.y = 456;

    esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(test_struct));
    
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }

    }
     
}

void handleConfirmation() {
    //Se envía señal a la UHF vía ESPNOW para que finalice la lectura. envio de datos uno a uno.
    //test.EPC_readed = "empty";
    test.x = 0;
    test.y = 0;

    esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(test_struct));
    
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
    
    //registrarPrestamo(nombre, identificacion, id_casillero, "EPC1,EPC2,EPC3");
   
    char htmlBuffer[2048];  // Buffer para almacenar el HTML formateado
    //devicesBuffer[0] = "Cables_1";
    //devicesBuffer[1] = "Protoboar_1";
    //devicesBuffer[2] = "Multimetro_3";
    //devicesBuffer[3] = "Multimetro_4";
    //devicesBuffer[4] = "Multimetro_5";
    //deviceCount = 2;
    // Formatear el HTML con los valores actuales de las variables
    sprintf(htmlBuffer, confirmationPage, devicesBuffer[0].c_str(), devicesBuffer[1].c_str(), devicesBuffer[2].c_str(), devicesBuffer[3].c_str(), devicesBuffer[4].c_str());
    
    // Enviar la página HTML al cliente
    server.send(200, "text/html", htmlBuffer);

    if(delivered)
      AccionCasillero(reservedBeforeInt);
    else
      AccionCasillero(reservedLocker);

}

void handleFinal() {
  String finalRegister;
  String finalMessage;
  int Count = 0;
  for(int i =0; i<deviceCount-1; i++){
    finalRegister += devicesBuffer[i];
    finalRegister +=",";
  }
  finalRegister += devicesBuffer[deviceCount-1];

    if(delivered == false){
      if(finalRegister == ""){
       bool estado = actualizarDisponibilidad(reservedLocker, 1); 
       finalMessage = "LECTURA VACIA, PRESTAMO INVALIDO";
      }
        
        else{
          registrarPrestamo(nombre, identificacion, reservedLocker, finalRegister);
          finalMessage = "PRESTAMO REALIZADO CON EXITO";
          //accionpuesto
          accionEnergizarPuesto(reservedLocker);
        } 
      
    }      
    else {
          Serial.println("Proceso de devolución en BD ");
          
          for(int i = 0; i<deviceCount; i++){

              
              for(int j = 0; j < PrestadoCount; j++){
                    
                      
                  if(devicesBuffer[i] == PrestadoBuffer[j]){
                      PrestadoBuffer[j] = "0";
                      Count++;
                      break;
                  }
              }
              
            }  

          Serial.println("cuenta de elementos prestados anteriormente: " + String(PrestadoCount));
          Serial.println("cuenta de elementos leidos: " + String(deviceCount));
          Serial.println("cuenta de elementos coincididos: " + String(Count));

          if(Count == PrestadoCount){
            bool estado = actualizarDisponibilidad(reservedBeforeInt, 1);
            Serial.println("Se solicita eliminar el id: "+ identificacion);
            eliminarPrestamo(identificacion);
            finalMessage = "DEVOLUCION REALIZADA EXITOSAMENTE";
            //accionpuesto
            accionEnergizarPuesto(reservedBeforeInt);
          }else{
            int missingElements = PrestadoCount - Count;
            
            String finalRegister;
              for(int i = 0; i < PrestadoCount; i++){

                if(PrestadoBuffer[i] != "0"){
                  finalRegister += PrestadoBuffer[i];
                  
                  if(missingElements != 1)
                    finalRegister +=",";

                    missingElements--;
                }

              }
              eliminarPrestamo(identificacion);
              delay(500);
              registrarPrestamo(nombre, identificacion, reservedBeforeInt, finalRegister);
              if(Count == 0)
              finalMessage = "Devolución FALLIDA, no se detectó ningún elemento asignado a su usuario, intente nuevamente";
              else
              finalMessage = "Devolución INCOMPLETA, los elementos faltantes quedan pendientes por entregar";
          }
    }
         
    Serial.println("Listado detectado por la UHF: ");
    for(int i = 0; i<deviceCount; i++ ){
      Serial.println(devicesBuffer[i]);
      devicesBuffer[i] = "";
    }

    for(int i = 0; i<PrestadoCount; i++ ){
      Serial.println(devicesBuffer[i]);
      PrestadoBuffer[i] = "";
    }
    deviceCount = 0;
    PrestadoCount = 0;
    delivered = false;
    reservedLocker = 0;
    flagRestart = 1;

    char htmlBuffer[2048];  // Buffer para almacenar el HTML formateado

    // Formatear el HTML con los valores actuales de las variables
    sprintf(htmlBuffer, finalPage, finalMessage.c_str());

    // Enviar la página HTML al cliente
    server.send(200, "text/html", htmlBuffer);
        
}

void setup() {
  Serial.begin(115200);
  MFRC5_setup();
  SetPines();

    // Intentar conectar a la red WiFi con IP estática
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Fallo al configurar la IP estática.");
  }

  // Conectar a WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi...");
  }

  Serial.println("Conectado a la red WiFi");

  Serial.print("Station IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Wi-Fi Channel: ");
  
  server.on("/", HTTP_GET, handleRoot);
  server.on("/checkCard", handleCheckCard);
  server.on("/user-info", HTTP_GET, handleUserInfo);
  server.on("/register", HTTP_GET, handleRegister);
  server.on("/register", HTTP_POST, handleRegister);
  server.on("/retirar", HTTP_GET, handleRetirarElementos);
  server.on("/previewItems", HTTP_GET, handlePreview);
  server.on("/confirmation", HTTP_GET, handleConfirmation);
  server.on("/final", HTTP_GET, handleFinal);

  server.begin();

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
  // register second peer  
  memcpy(peerInfo.peer_addr, broadcastAddress2, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  /// register third peer
  memcpy(peerInfo.peer_addr, broadcastAddress3, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

    /// register fourth peer
  memcpy(peerInfo.peer_addr, broadcastAddress4, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

      /// register fifth peer
  memcpy(peerInfo.peer_addr, broadcastAddress5, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
  }


void loop() {
  
   server.handleClient();
   
  // String response = consultarElementoPorEPC("EPC"+String(count));
  // if(count < 20)count++;
  // else count=1;
  // delay(5000);

if(flagRestart == 1 ){
  delay(500);
  Serial.println("REINICIANDO....");
  ESP.restart();  // Reinicia el ESP32
}
if(flagRead == 0){
// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
     if ( ! rfid.PICC_IsNewCardPresent()){return;}
     
     // Verify if the NUID has been readed
     if ( ! rfid.PICC_ReadCardSerial()){return;}
     
     Serial.print(F("PICC type: "));
     MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
     Serial.println(rfid.PICC_GetTypeName(piccType));
     // Check is the PICC of Classic MIFARE type
     if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K)
     {
       Serial.println("Su Tarjeta no es del tipo MIFARE Classic.");
       return;
     }
     
     if (rfid.uid.uidByte[0] != nuidPICC[0] || rfid.uid.uidByte[1] != nuidPICC[1] || rfid.uid.uidByte[2] != nuidPICC[2] || rfid.uid.uidByte[3] != nuidPICC[3] )
     {
       Serial.println("Se ha detectado una nueva tarjeta.");
       
       // Store NUID into nuidPICC array
       for (byte i = 0; i < 4; i++) {nuidPICC[i] = rfid.uid.uidByte[i];}
    
       DatoHex = printHex(rfid.uid.uidByte, rfid.uid.size);
       
       if(UserReg_1 == DatoHex || UserReg_2 == DatoHex ){
          secuenciaToken();
          }else if(UserReg_3 == DatoHex){
            accionEnergizarModoClase();
            }else{ 
              nuevaLectura(DatoHex);
              flagRead = 1;
              }
     }
     else 
     {
      Serial.println("Tarjeta leida previamente");
     }
     // Halt PICC
     rfid.PICC_HaltA();
     // Stop encryption on PCD
     rfid.PCD_StopCrypto1();

}

}

String printHex(byte *buffer, byte bufferSize)
{  
   String DatoHexAux = "";
   for (byte i = 0; i < bufferSize; i++) 
   {
       if (buffer[i] < 0x10)
       {
        DatoHexAux = DatoHexAux + "0";
        DatoHexAux = DatoHexAux + String(buffer[i], HEX);  
       }
       else { DatoHexAux = DatoHexAux + String(buffer[i], HEX); }
   }
   
   for (int i = 0; i < DatoHexAux.length(); i++) {DatoHexAux[i] = toupper(DatoHexAux[i]);}
   return DatoHexAux;
}

void accionEnergizarPuesto(int cod_rfid){

    test.x = cod_rfid*10;
    test.y = 120-cod_rfid*10; //operación aleatoria

    esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(test_struct));
    
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
}
void accionEnergizarModoClase(){

    test.x = 999;
    //test.y = 120-cod_rfid*10; //operación aleatoria

    esp_err_t result = esp_now_send(0, (uint8_t *) &test, sizeof(test_struct));
    
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
}

void asignarCasillero(){
      //consultar disponibilidad de casillero
    int id_casillero = 1;
    while(id_casillero <= 12){
      
      bool disponible = consultarDisponibilidad(id_casillero);

      if(disponible) break;
      else id_casillero++;

      Serial.println(id_casillero);
      delay(500);
    
    }
    reservedLocker = id_casillero;

    if(actualizarDisponibilidad(id_casillero, 0) and id_casillero <= 12){
      AccionCasillero(id_casillero);
      Serial.println("Se ha reservado el casillero "+ String(id_casillero) + " correctamente");    
      } 
}


void MFRC5_setup(){

   SPI.begin(); // Init SPI bus
   rfid.PCD_Init(); // Init MFRC522
   Serial.println();
   Serial.print(F("Reader :"));
   rfid.PCD_DumpVersionToSerial();
   for (byte i = 0; i < 6; i++) {
     key.keyByte[i] = 0xFF;
   } 
   DatoHex = printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
   Serial.println();
   Serial.println();
   Serial.println("Iniciando el Programa");
   
}

void nuevaLectura(String lectura_rfid){

  codigo_rfid = lectura_rfid;

  String response = consultarUsuarioPorRFID(codigo_rfid); 
  
  if (response == "No registrado"){

    newCardReadTwo = true;

    }else {

          StaticJsonDocument<1000> doc;
          DeserializationError error = deserializeJson(doc, response);

          if (error) {
              Serial.print("Error analizando JSON: ");
              Serial.println(error.c_str());
              return;
          }

          // Acceder a los valores del JSON deserializado
          nombre = doc["Nombre"].as<String>();
          codigo_rfid = doc["Codigo_RFID"].as<String>();
          identificacion = doc["Identificacion"].as<String>();
          correo = doc["Correo"].as<String>();
          programa = doc["Programa"].as<String>();
          
          newCardRead = true;

          delay(500);  // Esperar 2 segundos antes de hacer otra solicitud

          String prestamo = consultarHistorial(identificacion);

          if(prestamo == "Sin prestamos"){
             //Registrar prestamo()
            //Leer la entrega
            //Leer prestamos de objetos adicionales del mismo casillero
          }else{
              Serial.println("El usuario " + nombre + " cuenta con un prestamo vigente.");
              delivered = true;
              StaticJsonDocument<512> doc;
              
              DeserializationError error = deserializeJson(doc, prestamo);

              if (error) {
                  Serial.print("Error analizando JSON: ");
                  Serial.println(error.c_str());
                  return;
              }
              reservedBefore = doc["Casillero_id"].as<String>();
              deliveredDevices = doc["Elementos"].as<String>();

              reservedBeforeInt = reservedBefore.toInt();
              separarValores(deliveredDevices, ',');

              Serial.println("Prestamo tratado: ");
              Serial.println(reservedBefore);
              Serial.println(deliveredDevices);
              
              delay(500);
           

    
              }


  }

}

// Función para separar la cadena en subcadenas utilizando un delimitador
void separarValores(String cadena, char delimitador) {
  int startIndex = 0;
  int endIndex = 0;
  PrestadoCount = 0;
  while ((endIndex = cadena.indexOf(delimitador, startIndex)) >= 0) {
    String valor = cadena.substring(startIndex, endIndex);
    PrestadoBuffer[PrestadoCount] = valor;
    PrestadoCount++;
    Serial.println(valor);  // Imprimir cada valor separado en el monitor serial
    startIndex = endIndex + 1;
  }
  
  // Imprimir el último valor después de la última coma
  String ultimoValor = cadena.substring(startIndex);
  PrestadoBuffer[PrestadoCount] = ultimoValor;
  PrestadoCount++;
  Serial.println(ultimoValor);
}

//########################### Consumo de la Base de Datos ##############################################

bool actualizarDisponibilidad(int casillero_id, bool disponibilidad) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://" + ip_database + "/autolab_api/api.php");
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        String postData = "accion=actualizar_disponibilidad&casillero_id=" + String(casillero_id) + "&disponibilidad=" + String(disponibilidad ? "1" : "0");

        int httpResponseCode = http.POST(postData);

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.println(response);

            if (response.indexOf("success") != -1) {
                Serial.println("Disponibilidad actualizada correctamente.");
                http.end();
                return true;
            } else {
                Serial.println("Error al actualizar disponibilidad.");
            }
        } else {
            Serial.print("Error en la solicitud POST: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("Error en la conexión WiFi");
    }
    return false;
}


String consultarUsuarioPorRFID(String codigo_rfid) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin("http://" + ip_database + "/autolab_api/api.php");  // Reemplaza con la IP de tu servidor local
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        String postData = "accion=consultar_usuario&codigo_rfid=" + codigo_rfid;

        int httpResponseCode = http.POST(postData);

        if (httpResponseCode > 0) {
            String response = http.getString();
            response.trim();
            //Serial.println(response);

            if (response.indexOf("error") == -1) {
                // Aquí puedes procesar la respuesta JSON que contiene la información del usuario
                Serial.println("Usuario encontrado:");
                Serial.println(response);
                return response;
            } else {
                Serial.println("Usuario no encontrado");
                response = "No registrado";
                return response;
            }
        } else {
            Serial.print("Error en la solicitud POST: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("Error en la conexión WiFi");
    }
}


void registrarPrestamo(String nombre_usuario, String identificacion, int casillero_id, String elementos) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("http://" + ip_database + "/autolab_api/api.php");  // Reemplaza con la IP de tu servidor local
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "accion=registrar_prestamo&nombre_usuario=" + nombre_usuario + "&identificacion=" + identificacion + "&casillero_id=" + String(casillero_id) + "&elementos=" + elementos;

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.print("Error en la solicitud POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error en la conexión WiFi");
  }
}


String consultarHistorial(String identificacion) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = "http://" + ip_database + "/autolab_api/api.php?accion=consultar_historial&identificacion=" + identificacion;
    http.begin(url.c_str());

    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      response.trim(); // Elimina los espacios en blanco alrededor de la respuesta
            
       if (response == "Sin prestamos") {
                Serial.println(response);                
                return response;             
                
            } else {
                   // Aquí puedes procesar la respuesta JSON que contiene la información del usuario
                Serial.println("Prestamo vigente: ");
                Serial.println(response);
                return response;
            }

    } else {
      Serial.print("Error en la solicitud GET: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error en la conexión WiFi");
  }
}

bool consultarDisponibilidad(int casillero_id) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construir la URL de la petición GET
    String url = "http://" + ip_database + "/autolab_api/api.php?accion=consultar_disponibilidad&casillero_id=" + String(casillero_id);
    http.begin(url.c_str());

    // Hacer la solicitud GET
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String response = http.getString();
      response.trim(); // Eliminar espacios en blanco alrededor de la respuesta

      Serial.println("Respuesta del servidor: " + response);

      // Procesar la respuesta para determinar disponibilidad
      if (response == "1") {
        Serial.println("Casillero " + String(casillero_id) + " disponible.");
        http.end();
        return true ; //Disponible
      } else if (response == "0") {
        Serial.println("Casillero " + String(casillero_id) + " NO disponible.");
        http.end();
        return false;  //No disponible
      } else {
        Serial.println("Error: Respuesta inesperada");
      }
    } else {
      Serial.print("Error en la solicitud GET: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error en la conexión WiFi");
  }

  return false; // Retorna false si hay un error en la conexión o en la respuesta
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
            Serial.println("Respuesta del servidor: " + response);

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


void registrarUsuario(String nombre, String codigo_rfid, String identificacion, String Correo, String Programa){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin("http://" + ip_database + "/autolab_api/api.php");  // Reemplaza con la IP de tu servidor local
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String postData = "accion=registrar_usuario&nombre=" + nombre + "&codigo_rfid=" + codigo_rfid + "&identificacion=" + identificacion + "&correo=" + Correo + "&programa=" + Programa;

    int httpResponseCode = http.POST(postData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(response);

      if (response.indexOf("ya existe") > -1) {
        Serial.println("El usuario ya está registrado.");
        // Aquí puedes manejar el caso, como solicitar nuevamente los datos o notificar al usuario
      } else if (response.indexOf("registrado con éxito") > -1) {
        Serial.println("Usuario registrado con éxito.");
      }
    } else {
      Serial.print("Error en la solicitud POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("Error en la conexión WiFi");
  }
}
void eliminarPrestamo(String id) {
  if (WiFi.status() == WL_CONNECTED) { // Verificar la conexión WiFi
    HTTPClient http;

    // Construir la URL de la solicitud
    String url = "http://" + ip_database + "/autolab_api/api.php?accion=eliminar_prestamo&identificacion=" + id;

    // Iniciar la solicitud HTTP DELETE
    http.begin(url.c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.sendRequest("DELETE");

    if (httpResponseCode > 0) {
      String response = http.getString();  // Obtener la respuesta del servidor
      Serial.println("Respuesta del servidor:");
      Serial.println(response);
    } else {
      Serial.print("Error en la solicitud DELETE: ");
      Serial.println(httpResponseCode);
    }

    // Finalizar la conexión
    http.end();
  } else {
    Serial.println("Error: No conectado a la red WiFi");
  }
}

void SetPines(){
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(relay6, OUTPUT);
  pinMode(relay7, OUTPUT);
  pinMode(relay8, OUTPUT);
  pinMode(relay9, OUTPUT);
  pinMode(relay10, OUTPUT);
  pinMode(relay11, OUTPUT);
  pinMode(relay12, OUTPUT);
  
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
  digitalWrite(relay5, HIGH);
  digitalWrite(relay6, HIGH);
  digitalWrite(relay7, HIGH);
  digitalWrite(relay8, HIGH);
  digitalWrite(relay9, HIGH);
  digitalWrite(relay10, HIGH);
  digitalWrite(relay11, HIGH);
  digitalWrite(relay12, HIGH);
}

void AccionCasillero(int casillero){
switch(casillero){
    case 1: if(lockerOn[0] == 0){
            digitalWrite(relay1, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[0] = 1;
            }else{
              digitalWrite(relay1, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
            }
            break;
    case 2: if(lockerOn[1] == 0){
            digitalWrite(relay2, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[1] = 1;
            }else{
              digitalWrite(relay2, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
            }
            
            break;
    case 3: if(lockerOn[2] == 0){
            digitalWrite(relay3, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[2] = 1;
            }else{
              digitalWrite(relay3, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");            
            }
            break;
    case 4: if(lockerOn[3] == 0){
            digitalWrite(relay4, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[3] = 1;
            }else{
              digitalWrite(relay4, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
            }
            
            break;
    case 5: if(lockerOn[4] == 0){
            digitalWrite(relay5, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[4] = 1;
            }else{
              digitalWrite(relay5, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
            }            
            break;
    case 6: if(lockerOn[5] == 0){
            digitalWrite(relay6, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[5] = 1;
            }else{
              digitalWrite(relay6, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
            }
            break;
    case 7: if(lockerOn[6] == 0){
            digitalWrite(relay7, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[6] = 1;
            }else{
              digitalWrite(relay7, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
            }
            break;
    case 8: if(lockerOn[7] == 0){
            digitalWrite(relay8, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[7] = 1;
            }else{
              digitalWrite(relay8, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
                 }
            break;
    case 9: if(lockerOn[8] == 0){
            digitalWrite(relay9, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[8] = 1;
            }else{
              digitalWrite(relay9, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
                 }
            break;
    case 10: if(lockerOn[9] == 0){
            digitalWrite(relay10, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[9] = 1;
            }else{
              digitalWrite(relay10, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
                }
            break;
    case 11: if(lockerOn[10] == 0){
            digitalWrite(relay11, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[10] = 1;
            }else{
              digitalWrite(relay11, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
            }
            break;
    case 12: if(lockerOn[11] == 0){
            digitalWrite(relay12, LOW);
            Serial.println("Casillero "+ String(casillero) + " abierto.");
            lockerOn[11] = 1;
            }else{
              digitalWrite(relay12, HIGH);
              Serial.println("Casillero "+ String(casillero) + " cerrado.");
                  }
            break;
    default: break;
  }

}
void secuenciaToken(){
      
            digitalWrite(relay1, LOW);
            Serial.println("Casillero 1 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay1, HIGH);
            Serial.println("Casillero 1 cerrado.");   
            
            digitalWrite(relay2, LOW);
            Serial.println("Casillero 2 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay2, HIGH);
            Serial.println("Casillero 2 cerrado.");
            
            digitalWrite(relay3, LOW);
            Serial.println("Casillero 3 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay3, HIGH);
            Serial.println("Casillero 3 cerrado.");
           
            digitalWrite(relay4, LOW);
            Serial.println("Casillero 4 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay4, HIGH);
            Serial.println("Casillero 4 cerrado.");
           
            digitalWrite(relay5, LOW);
            Serial.println("Casillero 5 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay5, HIGH);
            Serial.println("Casillero 5 cerrado.");
           
            digitalWrite(relay6, LOW);
            Serial.println("Casillero 6 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay6, HIGH);
            Serial.println("Casillero 6 cerrado.");
           
            digitalWrite(relay7, LOW);
            Serial.println("Casillero 7 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay7, HIGH);
            Serial.println("Casillero 7 cerrado.");
           
            digitalWrite(relay8, LOW);
            Serial.println("Casillero 8 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay8, HIGH);
            Serial.println("Casillero 8 cerrado.");
           
            digitalWrite(relay9, LOW);
            Serial.println("Casillero 9 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay9, HIGH);
            Serial.println("Casillero 9 cerrado.");
           
            digitalWrite(relay10, LOW);
            Serial.println("Casillero 10 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay10, HIGH);
            Serial.println("Casillero 10 cerrado.");
           
            digitalWrite(relay11, LOW);
            Serial.println("Casillero 11 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay11, HIGH);
            Serial.println("Casillero 11 cerrado.");
           
            digitalWrite(relay12, LOW);
            Serial.println("Casillero 12 abierto.");
            delay(tiempoDelay);
            digitalWrite(relay12, HIGH);
            Serial.println("Casillero 12 cerrado.");
            flagRestart = 1;
}