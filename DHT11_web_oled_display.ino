#include <WiFi.h>
#include <DHTesp.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 oled(128,64,&Wire,-1);
DHTesp dht;
char *ssid = "YOUR_WIFI_SSID";
char *password = "YOUR_WIFI_PASSWORD";

WiFiServer server(80);

void setup() {
  oled.begin(SSD1306_SWITCHCAPVCC,0x3C);
  oled.setTextColor(WHITE);
  oled.clearDisplay();
  oled.setTextSize(1);
  dht.setup(12,DHTesp::DHT11);
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println("WIFI CONNECTED");
  Serial.print("IP ADDRESS: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  float d = dht.getTemperature();
  float r = dht.getHumidity();
  String ds = String(d);
  String rs = String(r);
  Serial.println("Temperature: "+ ds);
  Serial.println("Humidity: "+rs);

  oled.setCursor(0,0);
  oled.print("DHT11 Monitor");
  oled.setCursor(0,10);
  oled.print("Ali Ahmadi");
  oled.setCursor(0,30);
  oled.print("Temperature: "+ ds);
  oled.setCursor(0,40);
  oled.print("Humidity:"+ rs);
  oled.display();
  delay(200);
  oled.clearDisplay();
  WiFiClient client = server.available();
  if (client){
    Serial.println("NEW Client");
    String currentline = "";
    while(client.connected()){
      if(client.available()){
        char c = client.read();
        Serial.print(c);
        if (c == '\n'){
          if(currentline.length() == 0){
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("");
            client.print("<html><meta http-equiv=\"refresh\" content=\"5\"/><title>DHT11 Monitoring</title>");
            client.print("<style>html{background:green;color:white;}h2{color:yellow;}h1{color:#17a8b3;}</style>");
            client.print("<center><h1>DHT11 Monitor</h1><h2>Electroic.with.ali</h2>");
            client.print("<p>Temperature: " + ds + "</p>");
            client.print("<p>Humidity: " + rs + "</p>");
            client.print("<p>ESP32</p>");
            client.print("</center></html>");
            break;
          }
          else{
            currentline = "";
          }
        }
        else if(c != '\r'){
          currentline += c;
        }


      }
    }
    client.stop();
    Serial.println("client Disconnected");
  }
  delay(500);
}
