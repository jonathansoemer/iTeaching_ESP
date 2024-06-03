#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>

// Wi-Fi credentials REPLACE HERE
const char* ssid = "WLan-AABJ";
const char* password = "AAbj26201413!!!!";

// Web server URL
const char* serverName = "https://i-teaching-next.vercel.app/api/data";

// DHT sensor configuration
#define DHTPIN 4       // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22  // DHT 22 (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud

  // Initialize the DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Wait 5min (=300000ms) between measurements.
  delay(300000);

  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println(t);  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print temperature and humidity
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.println("°C ");

  // If WiFi is connected, send data
  if(WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Specify request destination
    http.begin(serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/json");

    // Prepare JSON payload
    String httpRequestData = "{\"data\": {\"temperature\": \"" + String(t) + "\", \"humidity\": \"" + String(h) + "\"}}";
    Serial.print("HTTP Request data: ");
    Serial.println(httpRequestData);

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    // Print HTTP response code
    if (httpResponseCode > 0) {
      String response = http.getString();  // Get the response to the request
      Serial.println(httpResponseCode);    // Print return code
      Serial.println(response);            // Print request answer
    }
    else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
}