#include <ArduinoJson.h>
#include <SD.h>
#include <SPI.h>

#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

/*
 * librerias para el RFID
*/

#define SS_PIN 4  //D2
#define RST_PIN 5 //D1
#include <MFRC522.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
int out = 0;

// Set these to run example.
#define FIREBASE_HOST "ccomercial-62041.firebaseio.com"
#define FIREBASE_AUTH "3r95O4apKqB1CkNjghlYziBx7SrTaEyZ0Cgs4YN5"
#define WIFI_SSID "TIGO11_076B4C"
#define WIFI_PASSWORD "3H4S4W7L"

void setup() {
  Serial.begin(9600);
   SPI.begin();      // Initiate  SPI bus
   mfrc522.PCD_Init();   // Initiate MFRC522

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}


void loop() {
    //cuando se registra una nueva tarjeta
    if ( ! mfrc522.PICC_IsNewCardPresent()) 
    {
      return;
      }
      
  // Selecciona una tarjeta
    if ( ! mfrc522.PICC_ReadCardSerial()) 
    {
      return;
    }
    
    Serial.println();
    Serial.print(" UID tag :");
    String content= "";
    byte letter;
    
    for (byte i = 0; i < mfrc522.uid.size; i++) 
    {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
      content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      content.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    
    content.toUpperCase();
    Serial.println();

    //inicio de firebase
    if (Firebase.failed()){
      Serial.print("ha ocurrido un error");
      delay(3000);
    }

    String nombre = Firebase.getString("users/"+content.substring(1)+"/apellido");  //realizamos la busqueda en la base de datos
    Serial.print("El nombre es"+nombre+"!");
    Serial.print("cui"+content.substring(1)+"!");

   if(nombre==""){  // si nombre esta vacio es por que el codigo no está registrado en la base de datos
    Serial.print("Usted no está registrado\n");
    Serial.print("Solicite una tarjeta válida\n");
   }else{
    Serial.print("nombre: "+nombre);
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& user = jsonBuffer.createObject();
    user["iduser"]=content.substring(1);
    user["fecha"]="23/2/2019";
    Firebase.push("asistencia",user);
    statuss = 1;
    }
}
