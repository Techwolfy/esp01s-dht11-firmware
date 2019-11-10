// ESP-01S WiFi Temperature Sensor

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include "SimpleDHT.h"
#include "index.h"
 
//
// Constants
//

#define GPIO_TEMP 2

#define SSID "ssid"
#define PASS "password"
#define HOST "esp01s-temp"
 
//
// Function prototypes
//

void handleRoot();
void handleStatus();
void handleNotFound();

//
// Global variables
//

ESP8266WebServer server(80);
SimpleDHT11 dht(GPIO_TEMP);

//
// Setup function, called once at startup
//
void setup()
{
    //
    // Start serial output
    //

    Serial.begin(115200);
    //Serial.setDebugOutput(true);

    //
    // Set up GPIO pins
    //

    dht.setPinInputMode(INPUT_PULLUP);
   
    //
    // Connect to WiFi network
    //
   
    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.hostname(HOST);
    WiFi.mode(WIFI_STA);
    WiFi.setPhyMode(WIFI_PHY_MODE_11N);
    WiFi.begin(SSID, PASS);
    
    int count = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");

        count++;
        if (count == 80) {
            Serial.println();
            WiFi.printDiag(Serial);
            Serial.print("Status: ");
            Serial.println(WiFi.status());

            count = 0;
        }
    }
   
    Serial.println();
    Serial.println("WiFi connected!");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Netmask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());

    //
    // Start mDNS responder
    //
  
    Serial.println();
    if (MDNS.begin(HOST)) {
        Serial.println("mDNS responder started.");
    } else {
        Serial.println("Error setting up MDNS responder!");
    }

    //
    // Start HTTP server
    //
  
    server.on("/", handleRoot);
    server.on("/status", handleStatus);
    server.onNotFound(handleNotFound);
    server.begin();

    Serial.println();
    Serial.println("HTTP server started.");
    Serial.println();
}

//
// Loop function, called in infinite loop after setup
//
void loop()
{
    //
    // Listen for HTTP requests
    //

    server.handleClient();
}

//
// Print the URI and query string of the current request
//
void printRequest()
{
    Serial.print("Received HTTP request for: ");
    Serial.print(server.uri());

    for(int i = 0; i < server.args(); i++)
    {
        if (i == 0)
        {
            Serial.print("?");
        }
        else
        {
            Serial.print("&");
        }

        Serial.print(server.argName(i));
        Serial.print("=");
        Serial.print(server.arg(i));
    }

    Serial.println();
}

//
// Callback for root web page
//
void handleRoot()
{
    printRequest();

    //
    // Display root page
    //

    server.send(200, "text/html", HTML);
}

//
// Callback for status endpoint
//
void handleStatus()
{
    int error = 0;
    float temperature = 0;
    float humidity = 0;
    char buf[256] = {0};

    printRequest();

    //
    // Retrieve current temperature
    //

    error = dht.read2(&temperature, &humidity, NULL);
    if (error != 0)
    {
        Serial.println("Error reading from DHT-11 sensor: ");
        Serial.println(error);
        temperature = -1;
        humidity = -1;
    }
    else
    {
        Serial.print("Read DHT-11 sensor data: temperature = ");
        Serial.print(temperature);
        Serial.print(" C, humidity = ");
        Serial.print(humidity);
        Serial.println("%RH");
    }

    //
    // Send JSON-formatted GPIO status
    //

    sprintf(buf,
            "{\"temperature\":%f, \"humidity\":%f, \"error\":%d, \"hostName\":\"%s\", \"macAddress\":\"%s\", \"networkSSID\":\"%s\", \"ipAddress\":\"%s\", \"subnetMask\":\"%s\", \"gatewayAddress\":\"%s\"}",
            temperature,
            humidity,
            error,
            HOST,
            WiFi.macAddress().c_str(),
            SSID,
            WiFi.localIP().toString().c_str(),
            WiFi.subnetMask().toString().c_str(),
            WiFi.gatewayIP().toString().c_str());

    server.send(200, "application/json", buf);
}

//
// Callback for unrecognized web page
//
void handleNotFound()
{
    printRequest();

    //
    // Send 404 error message
    //

    server.send(404, "text/plain", "404 Page Not Found");
}
