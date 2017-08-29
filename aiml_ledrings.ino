/*
 * AIML LED rings lighting
 * ESP8266 + WS2812 lighting control for Arup AIML exhibition
 * 
 * 2017 francesco.anselmo@arup.com
 * 
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "FastLED.h"

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    4
//#define CLK_PIN   4
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define NUM_LEDS    64
#define DELAY 10
CRGB leds[NUM_LEDS];


#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120
 
const char *ssid = "ssid";
const char *password = "password";
 
MDNSResponder mdns;
ESP8266WebServer server ( 80 );
const int led = 13;

void handleRoot() {
    digitalWrite ( led, 1 );
    String out = "<html><head><title>Wifi light</title></head>";
    out += "<body style='background-color:gray'>";
    out += "<span style='display:block; width:100%; font-size:2em; font-family:Verdana; text-align:center'>Choose color</span><br/>";
    out += "<a href='white'><span style='display:block; background-color:white; width:100%; height:6em;'></span></a><br/>";
    out += "<a href='red'><span style='display:block; background-color:red; width:100%; height:6em;'></span></a><br/>";
    out += "<a href='green'><span style='display:block; background-color:green; width:100%; height:6em;'></span></a><br/>";
    out += "<a href='blue'><span style='display:block; background-color:blue; width:100%; height:6em;'></span></a><br/>";
    out += "<a href='black'><span style='display:block; background-color:black; width:100%; height:6em;'></span></a>";
    out += "</body>";
    out += "</html>";
    server.send ( 200, "text/html", out );
    digitalWrite ( led, 0 );
}

void handleNotFound() {
    digitalWrite ( led, 1 );
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
        message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
    }
    server.send ( 404, "text/plain", message );
    digitalWrite ( led, 0 );
}

void setup ( void ) {
    pinMode ( led, OUTPUT );
    digitalWrite ( led, 0 );


    delay(3000); // 3 second delay for recovery
    setColor(2);
    delay(500);
    setColor(3);
    delay(500);
    setColor(4);
    delay(500);
    setColor(0);
  
    // tell FastLED about the LED strip configuration
    FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

    // set master brightness control
    FastLED.setBrightness(BRIGHTNESS);
 
    Serial.begin ( 115200 );
    WiFi.begin ( ssid, password );
    Serial.println ( "" );
    // Wait for connection
    while ( WiFi.status() != WL_CONNECTED ) {
        delay ( 500 );
        Serial.print ( "." );
    }
    
    Serial.println ( "" );
    Serial.print ( "Connected to " );
    Serial.println ( ssid );
    Serial.print ( "IP address: " );
    Serial.println ( WiFi.localIP() );
    if ( mdns.begin ( "esp8266", WiFi.localIP() ) ) {
        Serial.println ( "MDNS responder started" );
    }
    
    server.on ( "/", []() {handleRoot();} );
    server.on ( "/white", []() {setColor(1); handleRoot();} );
    server.on ( "/red", []() {setColor(2); handleRoot();} );
    server.on ( "/green", []() {setColor(3); handleRoot();} );
    server.on ( "/blue", []() {setColor(4); handleRoot();} );
    server.on ( "/black", []() {setColor(0); handleRoot();} );
    server.onNotFound ( handleNotFound );
    server.begin();
    Serial.println ( "HTTP server started" );
}

void loop ( void ) {
    mdns.update();
    server.handleClient();
}

void setColor(int color) {
  int i;
  Serial.println(color);
  for(int i = 0; i < NUM_LEDS; i++) {
    // set our current dot to red
    CRGB c;
    switch (color) {
      case 1:
        c = CRGB::White;
        break;
      case 2:
        c = CRGB::Red;
        break;
      case 3:
        c = CRGB::Green;
        break;
      case 4:
        c = CRGB::Blue;
        break;
      case 0:
        c = CRGB::Black;
        break;
    }
    leds[i] = c;
    FastLED.show();

    delay(DELAY);
  }

}
