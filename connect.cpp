#include "connect.h"

void registerNetworkConfigPaths(AsyncWebServer* server) {
  // Mask the networks file, and give scan information in its place
  server->on("/api/networks", HTTP_GET, [](AsyncWebServerRequest* request) {
    String json = "[";
    int n = WiFi.scanComplete();
    if (n == -2) {
      WiFi.scanNetworks(true);
    } else if (n) {
      for (int i = 0; i < n; ++i) {
        if (i)
          json += ",";
        json += "{";
        json += "\"rssi\":" + String(WiFi.RSSI(i));
        json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
        json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
        json += ",\"channel\":" + String(WiFi.channel(i));
        json += ",\"secure\":" + String(WiFi.encryptionType(i));
        json += ",\"hidden\":" + String(WiFi.isHidden(i) ? "true" : "false");
        json += "}";
      }
      WiFi.scanDelete();
      if (WiFi.scanComplete() == -2) {
        WiFi.scanNetworks(true);
      }
    }
    json += "]";
    request->send(200, "text/json", json);
    json = String();
  });

  // Get a list of known networks
  server->on("/api/known-networks", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/json", knownNetworksJson());
  });

  // Add a new network
  server->on("/api/networks/add", HTTP_POST, [](AsyncWebServerRequest* request) {
        char ssid[32];
        char password[64];
        char response[256];
        if (request->hasParam("ssid")) {
          sprintf(ssid, "%s", request->getParam("ssid")->value().c_str());
        }
        if (request->hasParam("password")) {
          sprintf(password, "%s",request->getParam("password")->value().c_str());
        } else {
          sprintf(password, "");
        }
        if (request->hasParam("ssid")) {
          switch (addNetwork(ssid, password)) {
            case 0:
              sprintf(response, "{status:'success', ssid:'%s'}", ssid);
              break;
            case -1:
              sprintf(response,
                      "{status:'error', error:'Could not Write to file'}");
              break;
            case 1:
              sprintf(response,
                      "{status:'error', error:'Duplicate alreadey exists, forget network first'}");
              break;
            default:
              sprintf(response, "{status:'error', error:'Unknown error'}");
          }
        } else {
          sprintf(response, "{error:'SSID missing'}");
        }
        request->send(200, "text/json", response);
      });

  // Remove a new network
  server->on("/api/networks/remove", HTTP_POST, [](AsyncWebServerRequest* request) {
        char ssid[32];
        char password[64];
        char response[256];

        if (request->hasParam("ssid")) {
          sprintf(ssid, "%s", request->getParam("ssid")->value().c_str());
          switch (removeNetwork(ssid)) {
            case 0:
              sprintf(response, "{status:'success', ssid:'%s'}", ssid);
              break;
            case -1:
              sprintf(response, "{status:'error', error:'Could not Write to file'}");
              break;
            case 1:
              sprintf(response, "{status:'error', error:'SSID not found'}");
              break;
            default:
              sprintf(response, "{status:'error', error:'Unknown error'}");
          }
        } else {
          sprintf(response, "{error:'SSID missing'}");
        }
        request->send(200, "text/json", response);
      });
}

// Read through the "networks" file and return the SSID/Passwords stored
int multiConnect(int numTries) {
  ESP8266WiFiMulti wifiMulti;
  if (safeOpen("/api/networks", "r") == 0) {
    File networksFile = SPIFFS.open("/api/networks", "r");
    networksFile.setTimeout(0);
    // // Loop through all the lines of the file, and return the newtork config
    while (true) {
      // If the line is empty, break
      if (networksFile.available()==0) {
        break;
      } 
      char ssid[32];
      char password[64];
      networksFile.readStringUntil(',').toCharArray(ssid, sizeof(ssid));
      networksFile.readStringUntil('\n').toCharArray(password, sizeof(password));
      wifiMulti.addAP(ssid, password);
    }
    networksFile.close();
    for (int i = 0; i < numTries; i++) {
      if (wifiMulti.run() == WL_CONNECTED) {
        Serial.println("");
        Serial.println("WiFi connected");
        Serial.println("IP address: ");
        Serial.println(WiFi.localIP());
        return 0;
      } else {
        Serial.print("MultiConnect Attempt ");
        Serial.print(i + 1);
        Serial.println(" failed, WiFi not connected!");
        delay(1000);
      }
    }
    return 1;
  } else {
    return -1;
  }
}

// Check if entry already exists in networks file
int checkDuplicate(String newSsid) {
  if (safeOpen("/api/networks", "r") == 0) {
    File networksFile = SPIFFS.open("/api/networks", "r");
    networksFile.setTimeout(0);
    while (true) {
      // If the line is empty, break
      if (networksFile.available() == 0) {
        break;
      }
      String ssid = networksFile.readStringUntil(',');
      String password = networksFile.readStringUntil('\n');
      if (ssid == newSsid) {
        // If the file already has the exact same SSID, return -1
        networksFile.close();
        return 1;
      }
    }
    networksFile.close();
    return 0;
  } else {
    return -1;
  }
}

// Add an item to the networks file
int addNetwork(String ssid, String password) {
  int duplicate = checkDuplicate(ssid);
  if (duplicate == 0) {
    File networksFile = SPIFFS.open("/api/networks", "a");
    // Add it to the end of the file.
    networksFile.print(ssid);
    networksFile.print(",");
    networksFile.print(password);
    networksFile.print("\n");
    networksFile.close();
    return 0;
  } else {
    return duplicate;
  }
}

int removeNetwork(String ssidRm){
  int duplicate = checkDuplicate(ssidRm);
  if(duplicate==0){
    // Duplicate not found
    return 1;
  }else if(duplicate == 1){
    // Open two files, the one to copy from, and one to copy to
    File networksOld = SPIFFS.open("/api/networks", "r");
    File networksNew = SPIFFS.open("/api/networks~", "w+");
    // Loop though the lines in the file
    while(true){
      // If the line is empty, break
      if (networksOld.available() == 0) {
        break;
      }
      String ssid = networksOld.readStringUntil(',');
      String password = networksOld.readStringUntil('\n');
      if(ssid != ssidRm){
        networksNew.print(ssid);
        networksNew.print(",");
        networksNew.print(password);
        networksNew.print("\n");
      }
    }
    networksOld.close();
    networksNew.close();
    SPIFFS.remove("/api/networks");
    SPIFFS.rename("/api/networks~", "/api/networks");
    return 0;
  } else {
    // File error
    return -1;
  }
}

String knownNetworksJson(){
  String json = "[";
  if(safeOpen("/api/networks", "r") == 0){
    File networksFile = SPIFFS.open("/api/networks", "r");
    while(true){
      if (networksFile.available() == 0) {
        break;
      }
      json += networksFile.readStringUntil(',');
      networksFile.readStringUntil('\n');
      // If the line is empty, break
      if (networksFile.available() != 0) {
        json += ",";
      }
    }
    networksFile.close();
  } else {
    return "{error:'Could not open file'}";
  }
  json += "]";
  return json;
}

int safeOpen(char* path, char* mode) {
  // Open the network list files
  if (SPIFFS.exists(path)) {
    File file = SPIFFS.open(path, mode);
    if (!file) {
      Serial.print(path);
      Serial.print(" could not be opened in ");
      Serial.print(mode);
      Serial.println(" mode");
      return -1;
    } else {
      file.close();
      return 0;
    }
  } else {
    Serial.print(path);
    Serial.println(" not found, Creating");
    File file = SPIFFS.open(path, "w");
    file.close();
    return safeOpen(path, mode);
  }
}