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

#ifndef STASSID 
#define STASSID "UPC6968727"
#define STAPSK  "aeZsvskmsex2"
#endif


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

ESP8266WebServer server(80);

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
bool Runflag = 0;


void sendJsonResponse(int code, const char *msg) {
  server.send(code, "application/json", "{\"msg\":\"" + String(msg) + "\"}");
}

void sendJsonOK(const char *msg) {
  sendJsonResponse(200, msg);
}

void sendJsonFAIL(const char *msg) {
  sendJsonResponse(400, msg);
}

void handleFlash()
{
  String FileName, flashwr;
  int lastbuf = 0;
  uint8_t cflag, fnum = 256;

  fsUploadFile = SPIFFS.open(upload_filename, "r");
  
  if (!fsUploadFile) {
    sendJsonFAIL("Could not open firmware");
  } else {
    bini = fsUploadFile.size() / 256;
    lastbuf = fsUploadFile.size() % 256;
    flashwr = String(bini) + "-" + String(lastbuf) + "<br>";
    for (int i = 0; i < bini; i++) {
      fsUploadFile.read(binread, 256);
      stm32SendCommand(STM32WR);
      while (!Serial.available()) ;
      cflag = Serial.read();
      if (cflag == STM32ACK)
        if (stm32Address(STM32STADDR + (256 * i)) == STM32ACK) {
          if (stm32SendData(binread, 255) == STM32ACK) {
            flashwr += ".";
          } else {
            flashwr = "Error";
          }
        }
    }
    fsUploadFile.read(binread, lastbuf);
    stm32SendCommand(STM32WR);
    while (!Serial.available()) ;
    cflag = Serial.read();
    if (cflag == STM32ACK) {
      if (stm32Address(STM32STADDR + (256 * bini)) == STM32ACK) {
        if (stm32SendData(binread, lastbuf) == STM32ACK)
          flashwr += "<br>Finished<br>";
        else flashwr = "Error";
      }
    }
    fsUploadFile.close();
    sendJsonOK("Programming");
  }
}

void setup(void)
{
  SPIFFS.begin();
  Serial.begin(115200, SERIAL_8E1);
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

  server.on("/list", HTTP_GET, []() {
    if (SPIFFS.exists(upload_filename)) {
      sendJsonOK("Ready to /erase, /programm, /delete");
    } else {
      sendJsonFAIL("No firmware uploaded");
    }
  });
  
  server.on("/programm", HTTP_GET, handleFlash);
  
  server.on("/run", HTTP_GET, []() {
    if (Runflag == 0) {
      RunMode();
      Runflag = 1;
    }
    else {
      FlashMode();
      Runflag = 0;
    }
    sendJsonOK("Run");
  });
  
  server.on("/erase", HTTP_GET, []() {
    if (stm32Erase() == STM32ACK) {
      sendJsonOK("Erase OK");
    } else if (stm32Erasen() == STM32ACK) {
      sendJsonOK("Erase OK");
    }
    sendJsonFAIL("Erase failure");
  });

  server.on("/delete", HTTP_GET, []() {
    if (SPIFFS.exists(upload_filename)) {
      sendJsonOK("Deleted");
      SPIFFS.remove(upload_filename);
    } else {
      sendJsonFAIL("No file to delete");
    }
  });
  
  server.onFileUpload([](){
    if (server.uri() != "/upload") return;
    HTTPUpload& upload = server.upload();
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
  
  server.on("/upload", HTTP_POST, []() {
    sendJsonOK("Uploaded OK");
  });
  
  server.on("/sync", HTTP_GET, []() {
    bool success = false;
    if (Runflag == 1) {
      FlashMode();
      Runflag = 0;
    }

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
  httpUpdater.setup(&server);
#endif /* WITH_GWOTA */
  
  server.begin();
  
#ifdef WITH_MDNS
  MDNS.addService("http", "tcp", 80);
#endif /* WITH_MDNS */
}

void loop(void) {
  server.handleClient();
#ifdef WITH_MDNS
  MDNS.update();
#endif /* WITH_MDNS */
}

void FlashMode()  {    //Tested  Change to flashmode
  digitalWrite(BOOT0, HIGH);
  delay(100);
  digitalWrite(NRST, LOW);
  delay(50);
  digitalWrite(NRST, HIGH);
  delay(200);
}

void RunMode()  {    //Tested  Change to runmode
  digitalWrite(BOOT0, LOW);
  delay(100);
  digitalWrite(NRST, LOW);
  delay(50);
  digitalWrite(NRST, HIGH);
  delay(200);
}
