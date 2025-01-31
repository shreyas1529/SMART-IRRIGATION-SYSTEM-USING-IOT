#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "Meme";
const char* password = "dr.aitEC";

// OpenWeatherMap API credentials
const char* weatherAPIKey = "6f7a9e67db3068eb61719f1974f59099";
String weatherAPIUrl = "http://api.openweathermap.org/data/2.5/forecast?q=Bengaluru&units=metric&appid=" + String(weatherAPIKey);

// Define the LED and Relay pin
const int ledPin = LED_BUILTIN;
const int relayPin = D1;

// Soil moisture sensor configuration
#define SOIL_PIN A0

// DHT sensor configuration
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Variables to store sensor readings and control states
float temperature = 0.0;
float humidity = 0.0;
float soilMoisture = 0.0;
bool rainTomorrow = false;
bool autoMode = true; // Default to automatic mode

unsigned long lastWeatherUpdate = 0; // Track weather updates
const unsigned long weatherUpdateInterval = 3600000; // 1 hour

// Create an instance of the Wi-Fi server
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize pins
  pinMode(ledPin, OUTPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(ledPin, HIGH);   // Turn LED off (active low)
  digitalWrite(relayPin, HIGH); // Turn relay off (active low)

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Start the server
  server.begin();
  Serial.println("Server started");
}


// Function to display sensor values dynamically
void disp(WiFiClient &client, int line, const char* description, float value) {
  client.println("<p>Line " + String(line) + ": " + String(description) + " - " + String(value, 2) + "</p>");
}
// Function to display boolean values dynamically
void dispBool(WiFiClient &client, int line, const char* description, bool value) {
  client.println("<p>Line " + String(line) + ": " + String(description) + " - " + (value ? "True" : "False") + "</p>");
}

void sendWebPage(WiFiClient &client, float temp, float humid, float soilMoisture, bool rainTomorrow) {
  unsigned long currentMillis = millis();
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>Motor Control</h1>");
  client.println("<p><a href=\"/MANUAL_ON\"><button>Manual ON</button></a></p>");
  client.println("<p><a href=\"/MANUAL_OFF\"><button>Manual OFF</button></a></p>");
  client.println("<p><a href=\"/AUTO\"><button>Automatic</button></a></p>");
  
//  client.println("<p>Soil Moisture: " + String(soilMoisture, 2) + "%</p>");
//  client.println("<p>Temperature: " + String(temperature, 2) + "°C</p>");
//  client.println("<p>Humidity: " + String(humidity, 2) + "%</p>");
//  client.println("<p>Rain Tomorrow: " + String(rainTomorrow ? "Yes" : "No") + "</p>");

    // Display sensor values
  disp(client, 1, "temperature", temp);
  disp(client, 2, "humidity", humid);
  disp(client, 3, "soilMoisture", soilMoisture);
  dispBool(client, 3, "Rain Tomorrow", rainTomorrow);

  client.println("<script>");
  client.println("setInterval(() => { location.reload(); }, 1000);");
  client.println("</script>");
  client.println("</html>");  
}

void getWeatherForecast() {
  WiFiClient client; // Create a WiFiClient instance
  HTTPClient http;   // Create an HTTPClient instance

  // Begin the HTTP GET request with the WiFiClient and URL
  http.begin(client, weatherAPIUrl); 
  int httpCode = http.GET(); // Perform the GET request

  if (httpCode > 0) {
    Serial.printf("HTTP GET request successful, code: %d\n", httpCode);
    String payload = http.getString();
    Serial.println(payload); // Print the JSON response

    // Parse the JSON data
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    // Extract forecast for tomorrow
    bool rainTomorrowForecast = false;
    for (JsonObject forecast : doc["list"].as<JsonArray>()) {
      if (forecast.containsKey("rain")) {
        if (forecast["rain"].containsKey("3h")) {
          float rainAmount = forecast["rain"]["3h"];
          if (rainAmount > 0.0) {
            rainTomorrowForecast = true;
            break;
          }
        }
      }
    }

    rainTomorrow = rainTomorrowForecast;
    Serial.print("Rain forecast for tomorrow: ");
    Serial.println(rainTomorrow ? "Yes" : "No");
  } else {
    Serial.println("Failed to retrieve weather data");
  }

  http.end(); // End the HTTP connection
}

void loop() {
  // Handle client connections
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        if (c == '\n') {
          if (request.indexOf("GET /MANUAL_ON") >= 0) {
            autoMode = false;
            digitalWrite(relayPin, LOW); // Turn relay ON
            Serial.println("Manual ON");
          } else if (request.indexOf("GET /MANUAL_OFF") >= 0) {
            autoMode = false;
            digitalWrite(relayPin, HIGH); // Turn relay OFF
            Serial.println("Manual OFF");
          } else if (request.indexOf("GET /AUTO") >= 0) {
            autoMode = true;
            Serial.println("Automatic Mode Enabled");
          }

          sendWebPage(client, temperature, humidity, soilMoisture, rainTomorrow);
          break;
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected");
  }

  // Read sensor values
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  int soilRaw = analogRead(SOIL_PIN);
  soilMoisture = map(soilRaw, 1023, 300, 0, 100);

  // Debugging: Print sensor values
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Humidity: ");
  Serial.println(humidity);
  Serial.print("Soil Moisture: ");
  Serial.println(soilMoisture);
  Serial.print("Rain Tomorrow: ");
  Serial.println(rainTomorrow ? "Yes" : "No");

  // Automatic control logic
  if (autoMode) {
    if (rainTomorrow) {
      digitalWrite(relayPin, HIGH); // Turn OFF motor
    } else if (soilMoisture < 30) { // Adjust threshold as needed
      digitalWrite(relayPin, LOW); // Turn ON motor
    } else {
      digitalWrite(relayPin, HIGH); // Turn OFF motor
    }
  }

  // Fetch weather data periodically
  if (millis() - lastWeatherUpdate > weatherUpdateInterval) {
    getWeatherForecast();
    lastWeatherUpdate = millis();
  }

  delay(200); // Small delay for stability
}
