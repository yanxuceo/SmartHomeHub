#ifndef WiFiTimeSync_h
#define WiFiTimeSync_h

#include <WiFi.h>

// Define the maximum number of WiFi networks
#define MAX_WIFI_NETWORKS 5 

class WiFiTimeSync {
public:
    struct WiFiCredentials {
        String ssid;
        String password;
    };

    WiFiTimeSync();
    bool addNetwork(const String& ssid, const String& password);
    void connectToNetwork(int networkIndex);
    String getConnectionStatus();
    void syncTime();
    String getSynchronizedTime();

private:
    WiFiCredentials wifiNetworks[MAX_WIFI_NETWORKS];
    int numberOfNetworks = 0;
    int currentNetworkIndex = -1;
    void connectToWiFi(const String& ssid, const String& password);
};

#endif
