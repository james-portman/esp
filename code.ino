#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char *ssid = "";
const char *password = "";
const char *managementHost = "";

void setup() {
  Serial.begin ( 115200 );
  Serial.println("I am starting up, in setup()");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  pinMode(D0, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin ( ssid, password );
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);
  WiFi.waitForConnectResult();
}

void loop() {

  if (registerWithServer()) {
    digitalWrite(D0, HIGH);
  } else {
    digitalWrite(D0, LOW);
  }
  
  delay(20000);
}


bool registerWithServer() {
  bool on = false;
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(managementHost, httpPort)) {
    Serial.println("connection failed");
    return false;
  }

  String url = "/";
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url);
  client.print(" HTTP/1.1\r\n" +
               String("Host: ") + managementHost + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print("["+line+"]");
    if (line.indexOf("LIGHTSON") >= 0) { on = true; }
  }
  return on;
}
