/*
*   
*    Copyright (C) 2017  CS.NOL  https://github.com/csnol/STM32-OTA
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, 
*    and You have to keep below webserver code 
*    "<h2>Version 1.0 by <a style=\"color:white\" href=\"https://github.com/csnol/STM32-OTA\">CSNOL" 
*    in your sketch.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*    
*   It is assumed that the STM32 MCU is connected to the following pins with NodeMCU or ESP8266/8285. 
*   Tested and supported MCU : STM32F03xF/K/C/，F05xF/K/C,F10xx8/B
*
*   连接ESP8266模块和STM32系列MCU.    Connect ESP8266 to STM32 MCU
*
*   ESP-01 Pin       STM32 MCU      NodeMCU Pin(ESP8266 based)
*   RXD                  	PA9             RXD
*   TXD                  	PA10            TXD
*   GPIO2                 BOOT0           D2
*   GPIO0                 RST             D1
*   Vcc                  	3.3V            3.3V
*   GND                  	GND             GND
*   En -> 10K -> 3.3V
*
*
* /home/flozzone/.arduino15/packages/esp8266/hardware/esp8266/2.6.3/tools/upload.py --chip esp8266 --port /dev/ttyUSB0 --baud 115200 --before default_reset --after hard_reset write_flash 0x0 /tmp/arduino_build_793505/STM32-OTA-ESP8266.ino.bin
*/

#define WITH_MDNS
#define WITH_GWOTA
#undef WITH_LOGGING
#define WITH_GATEWAY
#undef WITH_GATEWAY_TX

#ifndef STASSID 
#define STASSID "UPC6968727"
#define STAPSK  "aeZsvskmsex2"
#endif

#ifdef WITH_GATEWAY
#define SERIAL_BAUD 115200
#define SERIAL_MODE SERIAL_8N1
#define RXBUFFERSIZE 1024

#define STACK_PROTECTOR  128 // bytes

//how many clients should be able to telnet to this ESP8266
#define MAX_SRV_CLIENTS 2
#endif /* WITH_GATEWAY */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
// #include "spiffs/spiffs.h"            // Delete for ESP8266-Arduino 2.4.2 version
#include <FS.h>

#ifdef WITH_MDNS
#include <ESP8266mDNS.h>
#endif /* WITH_MDNS */

#include <ESP8266HTTPUpdateServer.h>
#include "stm32ota.h"

#include <algorithm> // std::min

#define NRST 0
#define BOOT0 2


const String STM32_CHIPNAME[8] = {
  "Unknown Chip",
  "STM32F03xx4/6",
  "STM32F030x8/05x",
  "STM32F030xC",
  "STM32F103x4/6",
  "STM32F103x8/B",
  "STM32F103xC/D/E",
  "STM32F105/107"
};

const char* host = "stm32-gateway";
const char* upload_filename = "/stm32fw.bin";

const char* ssid = STASSID;
const char* password = STAPSK;

IPAddress local_IP(192, 168, 0, 66);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

const int port = 23;

WiFiServer telnetServer(port);
WiFiClient telnetServerClients[MAX_SRV_CLIENTS];

ESP8266WebServer webServer(80);

#ifdef WITH_GWOTA
ESP8266HTTPUpdateServer httpUpdater;
#endif /* WITH_GWOTA */

File fsUploadFile;
uint8_t binread[256];
int bini = 0;
String stringtmp;
int rdtmp;
int stm32ver;
bool initflag = 0;

bool disableGwLoop = false;


void sendJsonResponse(int code, const char *msg) {
  webServer.send(code, "application/json", "{\"msg\":\"" + String(msg) + "\"}");
}

void sendJsonOK(const char *msg) {
  sendJsonResponse(200, msg);
}

void sendJsonFAIL(const char *msg) {
  sendJsonResponse(400, msg);
}

void handleFlash()
{
  bool success = false;
  int lastbuf = 0;
  uint8_t cflag, fnum = 256;

  fsUploadFile = SPIFFS.open(upload_filename, "r");
  
  if (!fsUploadFile) {
    sendJsonFAIL("Could not open firmware");
  } else {
    bini = fsUploadFile.size() / 256;
    lastbuf = fsUploadFile.size() % 256;
    for (int i = 0; i < bini; i++) {
      fsUploadFile.read(binread, 256);
      stm32SendCommand(STM32WR);
      while (!Serial.available()) ;
      cflag = Serial.read();
      if (cflag == STM32ACK)
        if (stm32Address(STM32STADDR + (256 * i)) == STM32ACK) {
          if (stm32SendData(binread, 255) == STM32ACK) {
            success = true;
          } else {
            success = false;
          }
        }
    }
    fsUploadFile.read(binread, lastbuf);
    stm32SendCommand(STM32WR);
    while (!Serial.available()) ;
    cflag = Serial.read();
    if (cflag == STM32ACK) {
      if (stm32Address(STM32STADDR + (256 * bini)) == STM32ACK) {
        if (stm32SendData(binread, lastbuf) == STM32ACK) {
          success = true;
        } else {
          success = false;
        }
      }
    }
    fsUploadFile.close();
    if (success) {
      sendJsonOK("Programming");
    } else {
      sendJsonFAIL("Failed flashing");
    }
  }
}

void setGwSerial() {
  delay(10);
  Serial.flush();  
  Serial.begin(SERIAL_BAUD, SERIAL_MODE);
  Serial.setRxBufferSize(RXBUFFERSIZE);
  delay(100);
}

void setup(void)
{
  SPIFFS.begin();
  
#ifdef WITH_GATEWAY
  setGwSerial();
#endif /* WITH_GATEWAY */
  
  pinMode(BOOT0, OUTPUT);
  pinMode(NRST, OUTPUT);

  digitalWrite(BOOT0, HIGH);
  digitalWrite(NRST, HIGH);

  WiFi.mode(WIFI_STA);
  WiFi.config(local_IP, gateway, subnet);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  webServer.on("/list", HTTP_GET, []() {
    if (SPIFFS.exists(upload_filename)) {
      sendJsonOK("Ready to /erase, /programm, /delete");
    } else {
      sendJsonFAIL("No firmware uploaded");
    }
  });
  
  webServer.on("/programm", HTTP_GET, handleFlash);
  
  webServer.on("/run", HTTP_GET, []() {
    RunMode();
    setGwSerial();
    disableGwLoop = false;
    sendJsonOK("Run");
  });
  
  webServer.on("/erase", HTTP_GET, []() {
    if (stm32Erase() == STM32ACK) {
      sendJsonOK("Erase OK");
    } else if (stm32Erasen() == STM32ACK) {
      sendJsonOK("Erase OK");
    }
    sendJsonFAIL("Erase failure");
  });

  webServer.on("/delete", HTTP_GET, []() {
    if (SPIFFS.exists(upload_filename)) {
      sendJsonOK("Deleted");
      SPIFFS.remove(upload_filename);
    } else {
      sendJsonOK("No file to delete");
    }
  });
  
  webServer.onFileUpload([](){
    if (webServer.uri() != "/upload") return;
    HTTPUpload& upload = webServer.upload();
    if (upload.status == UPLOAD_FILE_START) {
      fsUploadFile = SPIFFS.open(upload_filename, "w");
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (fsUploadFile) {
        fsUploadFile.write(upload.buf, upload.currentSize);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (fsUploadFile) {
        fsUploadFile.close();
      }
    }
  });
  
  webServer.on("/upload", HTTP_POST, []() {
    sendJsonOK("Uploaded OK");
  });
  
  webServer.on("/sync", HTTP_GET, []() {
    bool success = false;

    disableGwLoop = true;
    delay(10);
    Serial.flush();
    Serial.begin(115200, SERIAL_8E1);
    Serial.setRxBufferSize(256);
    delay(100);

    FlashMode();

    Serial.write(STM32INIT);
    delay(10);
    if (Serial.available() > 0);
    rdtmp = Serial.read();
    if (rdtmp == STM32ACK)   {
      stringtmp = STM32_CHIPNAME[stm32GetId()];
      success = true;
    } else if (rdtmp == STM32NACK) {
      Serial.write(STM32INIT);
      delay(10);
      if (Serial.available() > 0);
      rdtmp = Serial.read();
      if (rdtmp == STM32ACK)   {
        stringtmp = STM32_CHIPNAME[stm32GetId()];
        success = true;
      }
    } else {
      stringtmp = "ERROR";
    }

    if (success) {
      sendJsonOK(stringtmp.c_str());
    } else {
      sendJsonFAIL("Could not init MCU");
    }
  });
  
#ifdef WITH_MDNS
  MDNS.begin(host);
#endif /* WITH_MDNS */

#ifdef WITH_GWOTA
  httpUpdater.setup(&webServer);
#endif /* WITH_GWOTA */
  
  webServer.begin();

#ifdef WITH_GATEWAY
  telnetServer.begin();
  telnetServer.setNoDelay(true);
#endif /* WITH_GATEWAY */
  
#ifdef WITH_MDNS
  MDNS.addService("http", "tcp", 80);
#endif /* WITH_MDNS */
}

void loop(void) {
  
#ifdef WITH_GATEWAY
  if (!disableGwLoop) {
    gwLoop();
  }
#endif /* WITH_GATEWAY */

  webServer.handleClient();
  
#ifdef WITH_MDNS
  MDNS.update();
#endif /* WITH_MDNS */
}


void FlashMode() {
  return bootMode(HIGH);
}

void RunMode() {
  return bootMode(LOW);
}

void bootMode(int mode)  {    //Tested  Change to runmode
  digitalWrite(BOOT0, mode);
  delay(100);
  digitalWrite(NRST, LOW);
  delay(50);
  digitalWrite(NRST, HIGH);
  delay(200);
}

#ifdef WITH_GATEWAY
void gwLoop() {
  //check if there are any new clients
  if (telnetServer.hasClient()) {
    //find free/disconnected spot
    int i;
    for (i = 0; i < MAX_SRV_CLIENTS; i++)
      if (!telnetServerClients[i]) { // equivalent to !telnetServerClients[i].connected()
        telnetServerClients[i] = telnetServer.available();
#ifdef WITH_LOGGING
        logger->print("New client: index ");
        logger->print(i);
#endif /* WITH_LOGGING */
        break;
      }

    //no free/disconnected spot so reject
    if (i == MAX_SRV_CLIENTS) {
      telnetServer.available().println("busy");
      // hints: server.available() is a WiFiClient with short-term scope
      // when out of scope, a WiFiClient will
      // - flush() - all data will be sent
      // - stop() - automatically too
#ifdef WITH_LOGGING
      logger->printf("server is busy with %d active connections\n", MAX_SRV_CLIENTS);
#endif /* WITH_LOGGING */
    }
  }
#ifdef WITH_GATEWAY_TX
  //check TCP clients for data
#if 1
  // Incredibly, this code is faster than the bufferred one below - #4620 is needed
  // loopback/3000000baud average 348KB/s
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    while (telnetServerClients[i].available() && Serial.availableForWrite() > 0) {
      // working char by char is not very efficient
      Serial.write(telnetServerClients[i].read());
    }
#else
  // loopback/3000000baud average: 312KB/s
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    while (telnetServerClients[i].available() && Serial.availableForWrite() > 0) {
      size_t maxToSerial = std::min(telnetServerClients[i].available(), Serial.availableForWrite());
      maxToSerial = std::min(maxToSerial, (size_t)STACK_PROTECTOR);
      uint8_t buf[maxToSerial];
      size_t tcp_got = telnetServerClients[i].read(buf, maxToSerial);
      size_t serial_sent = Serial.write(buf, tcp_got);

      if (serial_sent != maxToSerial) {
#ifdef WITH_LOGGING
        logger->printf("len mismatch: available:%zd tcp-read:%zd serial-write:%zd\n", maxToSerial, tcp_got, serial_sent);
#endif /* WITH_LOGGING */
      }
    }
#endif /* 1 */
#endif /* WITH_GATEWAY_TX */

  // determine maximum output size "fair TCP use"
  // client.availableForWrite() returns 0 when !client.connected()
  size_t maxToTcp = 0;
  for (int i = 0; i < MAX_SRV_CLIENTS; i++)
    if (telnetServerClients[i]) {
      size_t afw = telnetServerClients[i].availableForWrite();
      if (afw) {
        if (!maxToTcp) {
          maxToTcp = afw;
        } else {
          maxToTcp = std::min(maxToTcp, afw);
        }
      } else {
        // warn but ignore congested clients
#ifdef WITH_LOGGING
        logger->println("one client is congested");
#endif /* WITH_LOGGING */
      }
    }

  //check UART for data
  size_t len = std::min((size_t)Serial.available(), maxToTcp);
  len = std::min(len, (size_t)STACK_PROTECTOR);
  if (len) {
    uint8_t sbuf[len];
    size_t serial_got = Serial.readBytes(sbuf, len);
    // push UART data to all connected telnet clients
    for (int i = 0; i < MAX_SRV_CLIENTS; i++)
      // if client.availableForWrite() was 0 (congested)
      // and increased since then,
      // ensure write space is sufficient:
      if (telnetServerClients[i].availableForWrite() >= serial_got) {
        size_t tcp_sent = telnetServerClients[i].write(sbuf, serial_got);
        if (tcp_sent != len) {
#ifdef WITH_LOGGING
          logger->printf("len mismatch: available:%zd serial-read:%zd tcp-write:%zd\n", len, serial_got, tcp_sent);
#endif /* WITH_LOGGING */
        }
      }
  }
}
#endif /* WITH_GATEWAY */
