//Board Selection Verification
#if !defined(ARDUINO_linkit_7697) //Linkit 7697
#error Use this code with Linkit 7697
#endif

//DEBUG habilitation
//Comment if unecessary DEBUG
#define DEBUG

//Libraries
#include <NTPClient.h>
#include <LWiFi.h>
#include <WiFiUdp.h>

//PCB Control PINS
//#define LED_BUILTIN 7
#define DA1 17
#define DB1 16
#define DC1 15
#define DD1 14
#define DA2 13
#define DB2 12
#define DC2 11
#define DD2 10
#define DA3 9
#define DB3 8
#define DC3 6
#define DD3 0
#define DA4 2
#define DB4 3
#define DC4 4
#define DD4 5

//Control Arrays
const int BCD_N = 4;
const int BCD_arrayN = 4;
const int BCD_Controls[BCD_N][BCD_arrayN] =
{ {DA1, DB1, DC1, DD1},
  {DA2, DB2, DC2, DD2},
  {DA3, DB3, DC3, DD3},
  {DA4, DB4, DC4, DD4}
};

//Wi-Fi parameters
char ssid[] = "Castro_2g";      //  your network SSID (name)
char pass[] = "pau4ka@@";   // your network password

//NTP UDP object
WiFiUDP ntpUDP;

// You can specify the time server pool and the offset, (in seconds)
// additionally you can specify the update interval (in milliseconds).
NTPClient timeClient(ntpUDP, "br.pool.ntp.org", -10800);

//Wi-Fi connection status
int status = WL_IDLE_STATUS;

//System variables
String tempo = "";
String horas = "";
String minutos = "";
String segundos = "";
int horai = 0;
int minutoi = 0;
int segundoi = 0;
int horai1 = 0;
int horai2 = 0;
int minutoi1 = 0;
int minutoi2 = 0;
int segundoi1 = 0;
int segundoi2 = 0;

//Time control variables
unsigned long tempo_antes = 0;
const int SEGUNDO = 1000;

void setup() {

  //Initialization
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("--* Linkit 7697 NTP Server Clock *--");
#endif
  delay(1000);

  //Connection
  while ( status != WL_CONNECTED ) {
    Serial.print("Conectando a rede: ");
    Serial.print(ssid);
    Serial.print( "." );
    status = WiFi.begin(ssid, pass);
  }

  //NTP server initialization
  timeClient.begin();
#ifdef DEBUG
  Serial.println("");
  printWifiStatus();
  Serial.println("Conectado e sistema iniciado\n\n");
#endif

  //User LED initialization
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);

  //BCD Controls Configuration Startup
  for (int j = 0; j < BCD_N; j++) {
    for (int i = 0; i < BCD_arrayN; i++) {
      pinMode(BCD_Controls[j][i], OUTPUT);
      digitalWrite(BCD_Controls[j][i], !0);
    }
  }



}

void loop() {

  //Every 1 minute reads server time
  if ((millis() - tempo_antes) > SEGUNDO) {

    //Update NTP time
    timeClient.update();
    tempo = timeClient.getFormattedTime();
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

#ifdef DEBUG
    Serial.print(tempo);
    Serial.println (" - original string");
#endif

    //Separates values from server return
    horas = getValue(tempo, ':', 0);
    minutos = getValue(tempo, ':', 1);
    segundos = getValue (tempo, ':', 2);

#ifdef DEBUG
    Serial.print(horas);
    Serial.print(":");
    Serial.print(minutos);
    Serial.print(":");
    Serial.print(segundos);
    Serial.println(" - string separado");
#endif

    //Converts String to Int
    horai = horas.toInt();
    minutoi = minutos.toInt();
    segundoi = segundos.toInt();

#ifdef DEBUG
    if (horai < 10) {
      Serial.print("0");
      Serial.print(horai);
    } else {
      Serial.print(horai);
    }
    Serial.print(":");
    if (minutoi < 10) {
      Serial.print("0");
      Serial.print(minutoi);
    } else {
      Serial.print(minutoi);
    }
    Serial.print(":");
    if (segundoi < 10) {
      Serial.print("0");
      Serial.print(segundoi);
    } else {
      Serial.print(segundoi);
    }
    Serial.println(" - inteiro convertido");
#endif

    //Separates Int values
    if (horai >= 10) {
      horai1 = (horai / 10) % 10;
      horai2 = horai % 10;
    } else {
      horai1 = 0;
      horai2 = horai;
    }

    if (minutoi >= 10) {
      minutoi1 = (minutoi / 10) % 10;
      minutoi2 = minutoi % 10;
    } else {
      minutoi1 = 0;
      minutoi2 = minutoi;
    }

    if (segundoi >= 10) {
      segundoi1 = (segundoi / 10) % 10;
      segundoi2 = segundoi % 10;
    } else {
      segundoi1 = 0;
      segundoi2 = segundoi;
    }

#ifdef DEBUG
    Serial.print(horai1);
    Serial.print(horai2);
    Serial.print(":");
    Serial.print(minutoi1);
    Serial.print(minutoi2);
    Serial.print(":");
    Serial.print(segundoi1);
    Serial.print(segundoi2);
    Serial.println(" - inteiro separado");
#endif

    //Control Displays by BCDs Converters
    ControlBCD(horai1, 0);
    ControlBCD(horai2, 1);
    ControlBCD(minutoi1, 2);
    ControlBCD(minutoi2, 3);

    #ifdef DEBUG
    Serial.println("\n");
    #endif

    //Updates time counter for loop
    tempo_antes = millis();

  }
}

//Funtion to separate the server response
String getValue(String data, char separator, int index) {

  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";

}

//Wi-Fi return
void printWifiStatus() {

  //Print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("Endereco de IP: ");
  Serial.println(ip);

  //Print the received signal strength:
  Serial.print("Forca da conexao (RSSI): ");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");

}

//Function to set BCD Converter input pins logic level
void ControlBCD(int digit, int BCD) {
#ifdef DEBUG
  Serial.print("Requisitado digito: ");
  Serial.print(digit);
  Serial.print(" para o BCD");
  Serial.println(BCD + 1);
#endif
  switch (digit) {
    case 0:
    #ifdef DEBUG
    Serial.println("Case0");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !0);
      digitalWrite(BCD_Controls[BCD][1], !0);
      digitalWrite(BCD_Controls[BCD][2], !0);
      digitalWrite(BCD_Controls[BCD][3], !0);
      break;
    case 1:
    #ifdef DEBUG
    Serial.println("Case1");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !1);
      digitalWrite(BCD_Controls[BCD][1], !0);
      digitalWrite(BCD_Controls[BCD][2], !0);
      digitalWrite(BCD_Controls[BCD][3], !0);
      break;
    case 2:
    #ifdef DEBUG
    Serial.println("Case2");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !0);
      digitalWrite(BCD_Controls[BCD][1], !1);
      digitalWrite(BCD_Controls[BCD][2], !0);
      digitalWrite(BCD_Controls[BCD][3], !0);
      break;
    case 3:
    #ifdef DEBUG
    Serial.println("Case3");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !1);
      digitalWrite(BCD_Controls[BCD][1], !1);
      digitalWrite(BCD_Controls[BCD][2], !0);
      digitalWrite(BCD_Controls[BCD][3], !0);
      break;
    case 4:
    #ifdef DEBUG
    Serial.println("Case4");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !0);
      digitalWrite(BCD_Controls[BCD][1], !0);
      digitalWrite(BCD_Controls[BCD][2], !1);
      digitalWrite(BCD_Controls[BCD][3], !0);
      break;
    case 5:
    #ifdef DEBUG
    Serial.println("Case5");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !1);
      digitalWrite(BCD_Controls[BCD][1], !0);
      digitalWrite(BCD_Controls[BCD][2], !1);
      digitalWrite(BCD_Controls[BCD][3], !0);
      break;
    case 6:
    #ifdef DEBUG
    Serial.println("Case6");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !0);
      digitalWrite(BCD_Controls[BCD][1], !1);
      digitalWrite(BCD_Controls[BCD][2], !1);
      digitalWrite(BCD_Controls[BCD][3], !0);
      break;
    case 7:
    #ifdef DEBUG
    Serial.println("Case7");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !1);
      digitalWrite(BCD_Controls[BCD][1], !1);
      digitalWrite(BCD_Controls[BCD][2], !1);
      digitalWrite(BCD_Controls[BCD][3], !0);
      break;
    case 8:
    #ifdef DEBUG
    Serial.println("Case8");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !0);
      digitalWrite(BCD_Controls[BCD][1], !0);
      digitalWrite(BCD_Controls[BCD][2], !0);
      digitalWrite(BCD_Controls[BCD][3], !1);
      break;
    case 9:
    #ifdef DEBUG
    Serial.println("Case9");
    #endif
      digitalWrite(BCD_Controls[BCD][0], !1);
      digitalWrite(BCD_Controls[BCD][1], !0);
      digitalWrite(BCD_Controls[BCD][2], !0);
      digitalWrite(BCD_Controls[BCD][3], !1);
      break;
  }
}
