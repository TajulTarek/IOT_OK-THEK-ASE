#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <TinyGPSPlus.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <WebSocketsClient.h>

#define I2C_ADDRESS 0x3C

#define BUTTON_IDLE 47
#define BUTTON_CNCL 48
#define BUTTON_OK 45
// NOTE: moved LED_YELLOW off GPIO1 (TX0) to a safe pin (e.g. 15)
#define LED_RED 2
#define LED_YELLOW 41
#define LED_GREEN 42

TinyGPSPlus gps;
HTTPClient https;
JsonDocument doc;
SSD1306AsciiWire oled;
WebSocketsClient webSocket;

const char *ssid = "Rj Avro Personal";
const char *password = "11111111";
static const uint32_t GPSBaud = 9600;

bool rideActive = false, offerActive = false;
uint32_t rideStartTime = 0, offerStartTime = 0, lastGPSLogTime = 0;
// String dest, pickup;
float latitude = 0.0, longitude = 0.0;

void onWebSocketEvent(WStype_t type, uint8_t *payload, size_t length);

void setup()
{
    // pins
    pinMode(BUTTON_IDLE, INPUT_PULLDOWN);
    pinMode(BUTTON_CNCL, INPUT_PULLDOWN);
    pinMode(BUTTON_OK, INPUT_PULLDOWN);

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);

    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_GREEN, HIGH);

    // serial
    Serial.begin(115200);
    delay(50);
    Serial1.begin(GPSBaud);

    // OLED init
    Wire.begin();
    Wire.setClock(400000L);
    oled.begin(&SH1106_128x64, I2C_ADDRESS);
    oled.setFont(Adafruit5x7);

    home_screen();
    oled.set1X();
    oled.setCursor(0, 6);
    oled.println("No Active Ride/Offer!");
    delay(1200);

    // connect wifi
    Serial.print("WiFi connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    unsigned long wifiStart = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(300);
        Serial.print(".");
        if (millis() - wifiStart > 20000)
        { // 20s timeout
            Serial.println();
            Serial.println("WiFi connect TIMEOUT - rebooting in 5s");
            delay(5000);
            ESP.restart();
        }
    }
    Serial.println();
    Serial.print("WiFi connected, IP: ");
    Serial.println(WiFi.localIP());

    webSocket.beginSSL("accessible-e-rickshaw-automation-system.onrender.com", 443, "/ws/iot");
    webSocket.onEvent(onWebSocketEvent);
    webSocket.setReconnectInterval(5000); // reconnect automatically
    delay(200);
}

void loop()
{
    webSocket.loop();

    if (millis() - offerStartTime > 20000)
    {
        offerActive = false;
        if (!rideActive)
        {
            home_screen();
            oled.set1X();
            oled.setCursor(0, 6);
            oled.println("No Active Ride/Offer!");
        }
        else
        {
            home_screen();
            oled.set1X();
            oled.setCursor(0, 6);
            oled.print("     Ride Active!    ");
            oled.setCursor(0, 7);
            oled.print(String(doc["requestId"]));
        }
    }
    // read GPS
    if (Serial1.available() > 0)
    {
        if (gps.encode(Serial1.read()))
        {
            // custom function call kora lagbe
            latitude = gps.location.lat();
            longitude = gps.location.lng();
        }
        else
        {
            // GPS data not valid yet, using dummy values
            latitude = 24.9048;
            longitude = 91.8600;
        }
        if (millis() - lastGPSLogTime > 2000)
        {
            lastGPSLogTime = millis();
            if (rideActive)
            {
                http_send_log(latitude, longitude, "inride");
            }
            else
            {
                http_send_log(latitude, longitude, "idle");
            }
        }
    }

    if (offerActive)
    {
        handleOffer(doc["pickup"], doc["destination"]);
    }
}

// WebSocket event callback
void onWebSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
    switch (type)
    {
    case WStype_CONNECTED:
        Serial.println("\n*** WSS Connected! ***");
        webSocket.sendTXT("{\"msg\":\"hello from Rj Avro Biswas (Developer)\"}");
        break;
    case WStype_DISCONNECTED:
        Serial.println("WebSocket Disconnected!");
        break;
    case WStype_TEXT:
    {
        Serial.print("Message from server: ");
        Serial.println((const char *)payload);
        deserializeJson(doc, String((const char *)payload));
        String dest = doc["destination"];
        String pickup = doc["pickup"];
        Serial.print("Pickup: ");
        Serial.println(pickup);
        Serial.print("Destination: ");
        Serial.println(dest);

        if (dest.length() > 5 && pickup.length() > 5)
        {
            offerActive = true;
            offerStartTime = millis();
        }
        else
        {
            offerActive = false;
        }
        break;
    }
    case WStype_ERROR:
        Serial.println("WebSocket ERROR");
        break;
    case WStype_BIN:
        Serial.println("WStype_BIN received");
        break;
    default:
        break;
    }
}

void home_screen()
{
    oled.set2X();
    oled.setCursor(0, 3);
    oled.println("Rickshaw-01");
}

void http_send_log(float latitude, float longitude, const char *status)
{
    https.begin("https://accessible-e-rickshaw-automation-system.onrender.com/api/rider/heartbeat");

    https.addHeader("Content-Type", "application/json");
    String json = "{\"riderId\":\"RIDER-001\", \"latitude\":" + String(latitude, 6) + ", \"longitude\":" + String(longitude, 6) + ", \"status\":\"" + String(status) + "\"}";
    int httpCode = https.POST(json);

    Serial.print("POST Response Code: ");
    Serial.println(httpCode);

    if (httpCode > 0)
    {
        String payload = https.getString();
        Serial.println("Response:");
        Serial.println(payload);
    }
    else
    {
        Serial.print("POST failed, error: ");
        Serial.println(https.errorToString(httpCode).c_str());
    }

    https.end();
}

void http_send_accept()
{
    https.begin("https://accessible-e-rickshaw-automation-system.onrender.com/api/rider/accept");

    https.addHeader("Content-Type", "application/json");
    String json = "{\"requestId\": \"" + String(doc["requestId"]) + "\", \"riderId\": \"RIDER-001\"}";

    int httpCode = https.POST(json);

    Serial.print("POST Response Code: ");
    Serial.println(httpCode);

    if (httpCode > 0)
    {
        String payload = https.getString();
        Serial.println("Response:");
        Serial.println(payload);
    }
    else
    {
        Serial.print("POST failed, error: ");
        Serial.println(https.errorToString(httpCode).c_str());
    }

    https.end();
}

void handleOffer(String pickup, String destination)
{
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_GREEN, HIGH);
    oled.clear();
    oled.set2X();
    oled.setCursor(0, 1);
    oled.println("New Offer!");
    oled.set1X();
    oled.setCursor(0, 4);
    oled.println("From: " + pickup + "         ");
    oled.setCursor(0, 5);
    oled.println("To: " + destination + "         ");
    oled.setCursor(0, 7);
    oled.print(" Reject       Accept ");

    if (digitalRead(BUTTON_CNCL) == HIGH)
    {
        offerActive = false;
        oled.clear();
        oled.set2X();
        oled.setCursor(10, 3);
        oled.println("Rejected!");
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_YELLOW, HIGH);
        digitalWrite(LED_GREEN, HIGH);
        delay(1200);
        oled.clear();
        home_screen();
        oled.set1X();
        oled.setCursor(0, 6);
        oled.print("No Active Ride/Offer!");
    }
    else if (digitalRead(BUTTON_OK) == HIGH)
    {
        offerActive = false;
        rideActive = true;
        rideStartTime = millis();
        http_send_accept();
        oled.clear();
        oled.set2X();
        oled.setCursor(10, 3);
        oled.print("Accepted!");
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_YELLOW, HIGH);
        digitalWrite(LED_GREEN, LOW);
        delay(1200);
        oled.clear();
        home_screen();
        oled.set1X();
        oled.setCursor(0, 6);
        oled.print("     Ride Active!    ");
        oled.setCursor(0, 7);
        oled.print(String(doc["requestId"]));
    }
}