#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// UDP parameters
unsigned int localPort = 4210;              // local port to listen on
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];  // buffer to hold incoming and outgoing packets
WiFiUDP udp;

// Struct to store ID and IP address
struct Pager {
  int id;
  IPAddress ipAddress;
};

// Struct to store user IP addresses
struct User {
  IPAddress ipAddress;
};

// Arrays to store pagers and users
Pager pagers[10];
User users[10];
int numPagers = 0;
int numUsers = 0;

bool userExists(IPAddress userIP) {
  for (int i = 0; i < numUsers; i++) {
    if (users[i].ipAddress == userIP) {
      return true;
    }
  }
  return false;
}

bool pagerExists(int pagerID) {
  for (int i = 0; i < numPagers; i++) {
    if (pagers[i].id == pagerID) {
      return true;
    }
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);   // Set ESP8266 as Access Point

  const char *ssid = "resup-network-1111";  // Name of the hotspot
  const char *password = "res2022up";       // Password for the hotspot
  int channel = 1;                          // Channel for the hotspot

  WiFi.softAP(ssid, password, channel);  // Start the AP

  udp.begin(localPort);
}

void loop() {
  // Check for incoming UDP packets
  int packetSize = udp.parsePacket();
  if (packetSize) {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remoteIP = udp.remoteIP();
    Serial.print(remoteIP);
    Serial.print(", port ");
    Serial.println(udp.remotePort());

    // Read the packet into the buffer
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

    // Process the received packet
    if (strcmp(packetBuffer, "join") == 0) {
      // Check if the user already exists
      if (!userExists(remoteIP) && numUsers < 10) {
        // Add the user to the users array
        users[numUsers].ipAddress = remoteIP;
        numUsers++;
        Serial.println("User joined!");

        // Send a notification to the user with all pagers data
        udp.beginPacket(remoteIP, localPort);
        for (int i = 0; i < numPagers; i++) {
          udp.print(pagers[i].id);
          udp.print(",");
          udp.print(pagers[i].ipAddress.toString());
          udp.print(";");
        }
        udp.endPacket();
      }
    } else if (strncmp(packetBuffer, "delete:", 7) == 0) {
      // Extract the ID from the packet content
      int pagerID = atoi(packetBuffer + 7);

      // Find the pager with the specified ID
      int pagerIndex = -1;
      for (int i = 0; i < numPagers; i++) {
        if (pagers[i].id == pagerID) {
          pagerIndex = i;
          break;
        }
      }

      // Delete the pager if found
      if (pagerIndex != -1) {

        // Shift the remaining pagers in the array to fill the gap
        for (int i = pagerIndex; i < numPagers - 1; i++) {
          pagers[i] = pagers[i + 1];
        }
        numPagers--;

        for (int i = 0; i < numUsers; i++) {
          udp.beginPacket(users[i].ipAddress, localPort);
          for (int i = 0; i < numPagers; i++) {
            udp.print(pagers[i].id);
            udp.print(",");
            udp.print(pagers[i].ipAddress.toString());
            udp.print(";");
          }
          udp.endPacket();
        }

        Serial.println("Pager deleted!");
      }
    } else {
      // Parse the packet content as the pager ID
      int pagerID = atoi(packetBuffer);

      // Check if the pager already exists
      if (!pagerExists(pagerID) && numPagers < 10) {
        // Add the pager to the pagers array
        pagers[numPagers].id = pagerID;
        pagers[numPagers].ipAddress = remoteIP;
        numPagers++;
        Serial.println("Pager joined!");

        // Send a notification to all saved users about the new pager
        for (int i = 0; i < numUsers; i++) {
          udp.beginPacket(users[i].ipAddress, localPort);
          for (int i = 0; i < numPagers; i++) {
            udp.print(pagers[i].id);
            udp.print(",");
            udp.print(pagers[i].ipAddress.toString());
            udp.print(";");
          }
          udp.endPacket();
        }
      }
    }
  }

  delay(5000);
}
