#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
unsigned int localUdpPort = 4210; // local port to listen on
char incomingPacket[255];         // buffer for incoming packets

int *clients = NULL;

void setup()
{
    Serial.begin(115200); // Open serial communication
    WiFi.mode(WIFI_AP);   // Set ESP8266 as Access Point
    pinMode(BUILTIN_LED, OUTPUT);
    ledOff();

    const char *ssid = "resup-network-1111"; // Name of the hotspot
    const char *password = "res2022up";      // Password for the hotspot
    int channel = 1;                         // Channel for the hotspot

    WiFi.softAP(ssid, password, channel); // Start the AP

    Udp.begin(localUdpPort);
}

void loop()
{
    delay(5000);
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
        // receive incoming UDP packets

        int len = Udp.read(incomingPacket, 255);
        if (len > 0)
        {
            incomingPacket[len] = 0;
        }

        if (clients == NULL)
        {

            int size = 0;
            client_push(&clients, &size, 10);
            delay(100);
            ledOn();
            delay(100);
            ledOff();
        }
        else
        {

            int size = sizeof(clients) / sizeof(clients[0]);

            if (!client_exists(clients, size, atoi(incomingPacket)))
            {

                client_push(&clients, &size, atoi(incomingPacket));
                delay(250);
                ledOn();
                delay(250);
                ledOff();
            }
        }
    }
}

void client_push(int **arr, int *size, int num)
{
    // increase the size of the array by 1
    (*size)++;
    *arr = (int *)realloc(*arr, (*size) * sizeof(int));

    // add the new element to the end of the array
    (*arr)[*size - 1] = num;
}

int client_exists(int *arr, int size, int item)
{
    // loop through the array and compare each element to the item
    for (int i = 0; i < size; i++)
    {
        if (arr[i] == item)
        {
            return 1; // item found
        }
    }

    return 0; // item not found
}

void ledOn()
{
    digitalWrite(BUILTIN_LED, LOW);
}
void ledOff()
{
    digitalWrite(BUILTIN_LED, HIGH);
}