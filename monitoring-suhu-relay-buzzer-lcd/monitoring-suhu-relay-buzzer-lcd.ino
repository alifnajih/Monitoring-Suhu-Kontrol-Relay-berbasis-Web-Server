#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

OneWire oneWire(0);
DallasTemperature sensors(&oneWire);

// Ganti dengan kredensial WiFi Anda
const char* ssid = "aku";
const char* password = "12345678910";

// Tentukan pin untuk relay
const int relayPin = 14;
const int buzzerPin = 12; // atau bisa gunakan GPIO5

// Membuat server di port 80
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(buzzerPin, OUTPUT);

  // Inisialisasi pin relay sebagai output
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Awalnya relay mati
  
  // Menghubungkan ke WiFi
  Serial.println();
  Serial.println();
  lcd.setCursor(0, 0);
  Serial.print("Connecting to ");
  lcd.print("Connecting to ");
  lcd.setCursor(0, 1);
  lcd.print(ssid);
  Serial.println(ssid);

  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi connected.");
  delay (1000);
  Serial.println("WiFi connected.");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP address:");
  Serial.println("IP address: ");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay (2000);
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MONITRING SUHU:");
  // Memulai server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  if (tempC != DEVICE_DISCONNECTED_C) {
     Serial.print("Suhu: ");
     lcd.setCursor(0, 1);
     lcd.print("SUHU:");
     lcd.print(tempC);
     lcd.print("°C");
     Serial.print(tempC);
     Serial.println("°C");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("GAGAL MEMBACA SUHU");
    Serial.println("Gagal membaca suhu dari sensor!");
  }
  delay(1000);
  
  digitalWrite(buzzerPin, HIGH);
  delay(500); // Tunggu 1 detik
  
  // Matikan buzzer
  digitalWrite(buzzerPin, LOW);
  delay(1000); // Tunggu 1 detik

  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  Serial.println("New Client.");
  String currentLine = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      Serial.write(c);
      if (c == '\n') {
        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0) {
          // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
          // and a content-type so the client knows what's coming, then a blank line:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();
          
          // the content of the HTTP response follows the header:
          client.print("Tekan <a href=\"/H\">Tombol</a> untuk menhidupkan relay.<br>");
          client.print("Tekan <a href=\"/L\">Tombol</a> untuk mematikan relay.<br>");
          
          // The HTTP response ends with another blank line:
          client.println();
          
          // Break out of the while loop:
          break;
        } else { // if you got a newline, then clear currentLine:
          currentLine = "";
        }
      } else if (c != '\r') { // if you got anything else but a carriage return character,
        currentLine += c; // add it to the end of the currentLine
      }
      
      // Check to see if the client request was "GET /H" or "GET /L":
      if (currentLine.endsWith("GET /H")) {
        digitalWrite(relayPin, HIGH); // Relay ON
      }
      if (currentLine.endsWith("GET /L")) {
        digitalWrite(relayPin, LOW); // Relay OFF
      }
    }
  }
  // Close the connection:
  client.stop();
  Serial.println("Client Disconnected.");
}
