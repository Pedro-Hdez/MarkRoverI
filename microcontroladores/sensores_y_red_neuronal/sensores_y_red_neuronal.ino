
#include <BasicLinearAlgebra.h>
using namespace BLA;

// ---------- Echoes y Triggers de los sensores ultrasónicos ----------
unsigned short int sensDerTrig = 6;
unsigned short int sensDerEcho = 5;

unsigned short int sensCentroTrig = 8;
unsigned short int sensCentroEcho = 7;

unsigned short int sensIzqTrig = 12;
unsigned short int sensIzqEcho = 11;
// *********************************************************************

// ---------- Variables para almacenar las distancias calculadas por los sensores ----------
// Distancias de los sensores
float distSensDer;
float distSensCentro;
float distSensIzq;
// *********************************************************************

// ---------- Pines de los motores ----------
// Pines del motor izquierdo
unsigned short int izqA = 5;
unsigned short int izqB = 6;

// Pines del motor derecho
unsigned short int derA = 10;
unsigned short int derB = 11;
// *********************************************************************

// Mensaje de la placa de wifi
char mensajeEsp8266;

// ****************************** RED NEURONAL ******************************

#define RENSWLAYER0 3
#define COLSWLAYER0 4
BLA::Matrix<RENSWLAYER0,COLSWLAYER0> weights_layer0 = {4.088535308837891, 4.528482437133789, -5.082963466644287, 4.40722131729126,
 0.3427855372428894, -0.2766473889350891, -5.3058271408081055, 4.766732215881348,
 -0.17308340966701508, 0.23171192407608032, -5.975094795227051, 4.971473693847656};

#define TAMBLAYER0 4
BLA::Matrix<1,TAMBLAYER0> bias_layer0 = {-0.8993253111839294, -0.795426070690155, 0.29666513204574585, -0.17137248814105988};


#define RENSWLAYER1 4
#define COLSWLAYER1 3
BLA::Matrix<RENSWLAYER1,COLSWLAYER1> weights_layer1 = {-3.1578617095947266, 2.546917676925659, -3.0269694328308105,
 -3.273172378540039, 2.7451560497283936, -2.9152934551239014,
 -1.7309777736663818, 1.4869023561477661, 3.257903575897217,
 2.383434295654297, -1.1055042743682861, -2.035982370376587};

#define TAMBLAYER1 3
BLA::Matrix<1,TAMBLAYER1> bias_layer1 = {-0.8317104578018188, 0.7105112671852112, 0.5773258805274963};


#define RENSWLAYER2 3
#define COLSWLAYER2 4
BLA::Matrix<RENSWLAYER2,COLSWLAYER2> weights_layer2 = {2.209212064743042, 0.12566913664340973, -1.9509284496307373, -0.27883559465408325,
 -0.8671445846557617, 0.6196681261062622, 0.9349763989448547, -0.10505367070436478,
 -0.40017542243003845, 2.3796539306640625, -1.9373548030853271, 2.335737705230713};

#define TAMBLAYER2 4
BLA::Matrix<1,TAMBLAYER2> bias_layer2 = {0.8409974575042725, -0.6162226796150208, 0.09114780277013779, 0.1059432402253151};

// Función de activación
float logistica(float x){
  return 1.0/(exp(-x)+1.0);
}

// *************************************************************************************

// ------------ Vectores para almacenar las salidas de las capas de la red -------------
// Entrada a la red neuronal
#define TAMENTRADA 3
BLA::Matrix <1,TAMENTRADA> input;

//Salida de la primer capa
BLA::Matrix <1, COLSWLAYER0> outputLayer0;
//Salida de la capa oculta
BLA::Matrix <1, COLSWLAYER1> outputLayer1;
//Salida de la última capa
BLA::Matrix <1, COLSWLAYER2> output;

boolean inAutomatic;

void setup() {
  // Se inicializa el puerto serial
  Serial.begin(115200);

  // Se inicializan los pines de los sensores
  pinMode(sensIzqTrig, OUTPUT);
  pinMode(sensCentroTrig, OUTPUT);
  pinMode(sensDerTrig, OUTPUT);

  pinMode(sensIzqEcho, INPUT);
  pinMode(sensCentroEcho, INPUT);
  pinMode(sensDerEcho, INPUT);

  // Led para indicar si el arduino está controlando
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  mensajeEsp8266 = 0;
  inAutomatic = false;
}

// Función para las lecturas de los sensores
float readSensor(int trig, int echo){
  float duration;
  float distance;
  unsigned long currentMillis;

  // Se inicializa el sensor apagando el Trigger 2 microsegundos (.002 milisegundos)
  currentMillis = millis();
  digitalWrite(trig, LOW);
  while (millis() < currentMillis + .002);
  
  // Se envía una señal de sonido por 10 microsegundos (.01 milisegundos)y después se
  // apaga el Trigger para que no interfiera con la señal que rebota
  currentMillis = millis();
  digitalWrite(trig, HIGH);
  while (millis() < currentMillis + .01);
  digitalWrite(trig, LOW);
  
  // Se cuenta el tiempo que tarda en llegar la señal rebotada
  duration = pulseIn(echo, HIGH);
  
  // Se calcula la distancia en centímetros 
  // (de acuerdo a la fórmula del fabricante)
  distance = duration / 58;
  //return distance;
  
  if (distance >= 20 && distance <= 400) distance = 10;
  if (distance < 3 || distance > 400) distance = 0;
  
  return (20 - distance) / 20;
  
}

unsigned short int velocidad = 600; // Velocidad de los motores
unsigned short int deltaAdelante = 30; // Diferencia de velocidades
unsigned short int deltaAtras = 35; // Diferencia de velocidades

unsigned long currentMillis;
float x;
String resultado = "";
char* cString;
void loop() {
  
  if(Serial.available()){

    mensajeEsp8266 = Serial.read();
    currentMillis = millis();
    while (millis() < currentMillis + 300);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on 
    
    distSensIzq = readSensor(sensIzqTrig, sensIzqEcho);
    distSensCentro = readSensor(sensCentroTrig, sensCentroEcho);
    distSensDer = readSensor(sensDerTrig, sensDerEcho);
  
    // ----- Se calcula la acción con la red neuronal -----
    // Se inicializa la entrada
    input << distSensIzq, distSensCentro, distSensDer;
  
     // Se calcula la activación de la capa de entrada
    outputLayer0 = (input * weights_layer0) + bias_layer0;
    for(int i = 0 ; i < outputLayer0.GetColCount() ; i++){
      outputLayer0(0, i) = logistica(outputLayer0(0, i));
    }
  
    // Se calcula la activación de la capa oculta
    outputLayer1 = (outputLayer0 * weights_layer1) + bias_layer1;
    for(int i = 0 ; i < outputLayer1.GetColCount() ; i++){
      outputLayer1(0, i) = logistica(outputLayer1(0, i));
    }
  
    // Se calcula la activación de la capa de salida
    output = (outputLayer1 * weights_layer2) + bias_layer2;
    for(int i = 0 ; i < output.GetColCount() ; i++){
      output(0, i) = logistica(output(0, i));
    }
  
    // Se decide la acción a realizar de acuerdo a la salida de la red
    resultado = "";
    for(int i = 0 ; i < output.GetColCount() ; i++){
      if(output(0, i) < .55) output(0, i) = 0;
      if(output(0, i) >= .55) output(0, i) = 1;

      resultado.concat((int)output(0, i));
    }

    if(resultado == "0000" || resultado == "1100" || resultado == "0011" || resultado == "1111" || resultado == "0110" || resultado == "1010"){
      resultado = "1000";
    }
    
    
    cString = (char*) malloc(sizeof(char)*(resultado.length() + 1));
    resultado.toCharArray(cString, resultado.length() + 1);
    Serial.write(cString);
    free(cString);
    
  }
  else{
    digitalWrite(LED_BUILTIN, LOW);   // turn the LED on
  }
}
