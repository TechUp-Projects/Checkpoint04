#include <ArduinoJson.h>
const int TAMANHO = 200;

volatile unsigned long cont = 0;
unsigned long tempo = 0;
long VeloAngu = 0;
long VeloMs = 0;
long VeloKmh = 0;
long Vlc;

int ExposiUv = A1;
int Vout;
float R3 = 10000;
float Alpha=0.85;
float Rdark = 127410;
float Rldr, uv, L;

int TermistorPin = A0;
int Vo;
float R1 = 10000;
float logR2, R2, T, Tc, Tf, temp;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07; //valores constante para calculo

void setup() {
  Serial.begin(115200);
  pinMode(TermistorPin, INPUT);
  pinMode(ExposiUv, INPUT);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), interrupcao, RISING);
}

void loop() {
  temp = readTemp(TermistorPin);
  uv = readUv(ExposiUv);
  //Verificando se o tempo passou
  if( (millis() - tempo) > 999){
    tempo = millis();

    VeloAngu = 2 * 3.14 * cont;
    VeloMs = VeloAngu * 0.3;
    VeloKmh = VeloMs * 3.6;
    
    cont = 0;
  }
  //formato de leitura do node-red
  StaticJsonDocument <TAMANHO> json;
  json["topic"] = "Sensor";
  json["temp"] = Tc;
  json["lux"] = L;
  json["velo"] = VeloKmh;
  serializeJson( json, Serial);
  Serial.println();
}

//função que faz leitura da temperatura e retorna o valor em graus celcius
float readTemp(int ThermistorPin){
  
  Vo = analogRead(ThermistorPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0); //calculo R2, demonstração no arquivo pdf da aula
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));// Equação de Steinhart–Hart 
  Tc = T - 273.15;
  return Tc;
}
//função que faz a leitura dos raios em UV
float readUv (int ExposiUv){
  Vout = analogRead(ExposiUv);
  Rldr = R3 *( 1023.0 / (float) Vout - 1.0);
  L = pow(Rdark/Rldr, 1/Alpha);
  return L;
}
void interrupcao(){
  cont++;
}