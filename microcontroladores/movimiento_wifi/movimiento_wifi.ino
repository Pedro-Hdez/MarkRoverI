// Librerias auxiliares para el modulo NodeMCU ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

// ******************* PARA LA CONEXIÓN A INTERNET Y COMUNICACIÓN CON EL SERVIDOR ***********************

// ***** Informacion necesaria para conectarse a Internet*****
// ***** SSID y Contrasena del router *****
const char* ssid = "ssid";
const char* password = "password";

// *** Funcion para inicial la conexion a internet 
// en modo estacion ***
void connectToWiFiAsSTA(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

// ***** Dirección IPv4 del servidor *****
const char* host = "http://markrover.ml/";

// ***** Direccion del archivo PHP que nos enviara la información que necesitamos *****
int id = 0; // ID de la direccion del carro en la base de datos.
String archivoGet = "GetData.php"; // Archivo que nos contestara la peticion
// Dirección en el servirdor del archivo que nos contestara la petición
String direccionGet = host + archivoGet; 
String idBD = "ID=" + String(id); // Cadena que se enviara como petición al servidor.

// ***** Variables para recibir los mensajes *****
HTTPClient http; // Se declara un objeto cliente para comunicarse con el servidor
int httpCodeGet; // Para enviar la peticion
String payload; // Para obtener el mensaje recibido como cadena de caracteres

// *******************************************************************************************************


// ******************* CONTROL DE LOS MOTORES *************************
// ***** Velocidad de los motores *****
unsigned short int velocidad = 600; // Velocidad de los motores
unsigned short int deltaAdelante = 30; // Diferencia de velocidades
unsigned short int deltaAtras = 35; // Diferencia de velocidades

unsigned short int velocidad_automatico = 450; // Velocidad de los motores
unsigned short int deltaAdelante_automatico = 23; // Diferencia de velocidades
unsigned short int deltaAtras_automatico = 26; // Diferencia de velocidades

// ***** Pines asignados para el control de los motores *****
unsigned short int izqA = 5;
unsigned short int izqB = 4;
unsigned short int derA = 0;
unsigned short int derB = 2;

// ***** Para controlar los giros *****
boolean beforeWasForward;

// ***** Variables para almacenar la salida de cada pin de los motores *****
int outputIzqA, outputIzqB, outputDerA, outputDerB;
// *******************************************************************************************************


void setup() {
  // Se inicializa el monitor serial
  Serial.begin(115200);

  // Se inicia la conexión a Internet
  connectToWiFiAsSTA();
  
  // Los pines de la placa se declaran como salidas
  pinMode(izqA, OUTPUT);
  pinMode(izqB, OUTPUT);
  pinMode(derA, OUTPUT);
  pinMode(derB, OUTPUT);
}

// ***** Función para enviar una instrucción al chip L293D *****
void action(int outputIzqA, int outputIzqB, int outputDerA,int outputDerB){  
  analogWrite(izqA, outputIzqA);
  analogWrite(izqB, outputIzqB);
  analogWrite(derA, outputDerA);
  analogWrite(derB, outputDerB);
}

// ***** Variable para contar el tiempo sin usar la función "Delay"
unsigned long currentMillis;

// ***** Variable para leer el mensaje del Arduino cuando nos encontremos en piloto automático *****
byte mensaje[4];

void loop() {  

  // ******************************** COMUNICACIÓN CON EL SERVIDOR ****************************************
  // ***** Se inicia la comunicación con el archivo GetData.php *****
  http.begin(direccionGet); // Se inicia la conexión con el servidor
  http.addHeader("Content-Type", "application/x-www-form-urlencoded"); // Se especifica el tipo de mensaje

  // ***** Se realiza la petición y se obtiene la respuesta *****
  httpCodeGet = http.POST(idBD); // Se envía la petición
  payload = http.getString(); // Se convierte a cadena de caracteres la respuesta del servidor  

  // *******************************************************************************************************

  // ***** Se decide qué hacer con los motores de acuerdo a la respuesta del servidor *****

  // **************************** COMUNICACIÓN CON EL ARDUINO ********************************************
  if (payload=="atm"){
    // Se le dice al Arduino que necesitamos su servicio
    Serial.write(0);
    currentMillis = millis();

    // Se hace una pequeña pausa para que el Arduino lea el mensaje y vacíe el buffer,
    // así evitamos conflictos con el siguiente While
    while (millis() < currentMillis + 50);
    while(!Serial.available());

    // Se lee la salida de la red neuronal y se pasan esos valores a nuestras variables
    Serial.readBytes(mensaje, 4);
    
    if (mensaje[0] - '0' == 1) outputIzqA = velocidad_automatico-deltaAtras_automatico;
    else outputIzqA = 0;
    
    if(mensaje[1] - '0'== 1) outputIzqB = velocidad_automatico - deltaAdelante_automatico;
    else outputIzqB = 0;

    if(mensaje[2] - '0' == 1) outputDerA = velocidad_automatico;
    else outputDerA = 0;

    if(mensaje[3] - '0'== 1) outputDerB = velocidad_automatico;
    else outputDerB = 0;

    // Se ejecuta la acción en los motores
    action(outputIzqA,outputIzqB,outputDerA,outputDerB);
    http.end(); //--> Cierra la conexión con el servidor
  }

  // ************************************ CASOS DE CONDUCCIÓN MANUAL ******************************
  else if (payload == "forward") {
    if (!beforeWasForward) beforeWasForward = true;
    outputIzqA = 0;
    outputIzqB = velocidad-deltaAdelante;
    outputDerA = 0;
    outputDerB = velocidad;

    action(outputIzqA,outputIzqB,outputDerA,outputDerB);
    http.end(); //--> Close connection
  }
  else if (payload == "reverse") {
    if(beforeWasForward)beforeWasForward = false;
    
    outputIzqA = velocidad-deltaAtras;
    outputIzqB = 0;
    outputDerA = velocidad;
    outputDerB  = 0;

    action(outputIzqA,outputIzqB,outputDerA,outputDerB);
    http.end(); //--> Close connection
  }
  else if (payload == "left") {
    outputIzqA = 0;
    outputIzqB = 0;
    
    if (beforeWasForward){
      outputDerA = 0;
      outputDerB = velocidad;
    }
    else{
      outputDerA = velocidad;
      outputDerB = 0;
    }

    action(outputIzqA,outputIzqB,outputDerA,outputDerB);
    http.end(); //--> Close connection
  }
  else if (payload == "right") {
    
    if(beforeWasForward){
      outputIzqA = 0;
      outputIzqB = velocidad-deltaAdelante;
    }
    else {
      outputIzqA = velocidad-deltaAdelante;
      outputIzqB = 0;
    }
    
    outputDerA = 0;
    outputDerB = 0;

    action(outputIzqA,outputIzqB,outputDerA,outputDerB);
    http.end(); //--> Close connection
  }
  else if (payload == "stop") {
    if(!beforeWasForward) beforeWasForward = true;
    
    outputIzqA = 0;
    outputIzqB = 0;
    outputDerA = 0;
    outputDerB = 0;

    action(outputIzqA,outputIzqB,outputDerA,outputDerB);
    http.end(); //--> Close connection
  }
}
