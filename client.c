#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// resup resturant pager : client
const int clientId = 1;

const char *ssid = "resup-network-1111";
const char *password = "res2022up";
WiFiUDP Udp;
unsigned int localUdpPort = 4210; // local port to listen on
char incomingPacket[255];         // buffer for incoming packets

void setup()
{

    Serial.begin(115200);
    pinMode(BUILTIN_LED, OUTPUT);
    ledOff();

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(250);
        ledOn();
        delay(250);
        ledOff();
    }

    Udp.begin(localUdpPort);

    Udp.beginPacket("192.168.4.1", 4210);
    Udp.write(clientId);
    Udp.endPacket();
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {

        while (WiFi.status() != WL_CONNECTED)
        {
            delay(1000);
            WiFi.begin(ssid, password);
            while (WiFi.status() != WL_CONNECTED)
            {
                delay(250);
                ledOn();
                delay(250);
                ledOff();
            }
        }
    }
    else
    {

        delay(20000);
        int packetSize = Udp.parsePacket();
        if (packetSize)
        {
            // receive incoming UDP packets

            int len = Udp.read(incomingPacket, 255);
            if (len > 0)
            {
                incomingPacket[len] = 0;
            }

            if (strcmp(incomingPacket, "page") == 0)
            {
                ledOn();
                delay(1000000);
            }
        }
    }
}

void ledOn()
{
    digitalWrite(BUILTIN_LED, LOW);
}
void ledOff()
{
    digitalWrite(BUILTIN_LED, HIGH);
}
