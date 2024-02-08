#include "WiFiTimeSync.h"

WiFiTimeSync::WiFiTimeSync() {}

bool WiFiTimeSync::addNetwork(const String& ssid, const String& password) {
    if (numberOfNetworks < MAX_WIFI_NETWORKS) {
        wifiNetworks[numberOfNetworks++] = {ssid, password};
        return true;
    }
    return false; // Return false if there's no room for more networks
}

void WiFiTimeSync::connectToNetwork(int networkIndex) {
    if (networkIndex >= 0 && networkIndex < numberOfNetworks) {
        currentNetworkIndex = networkIndex;
        auto& credentials = wifiNetworks[networkIndex];
        connectToWiFi(credentials.ssid, credentials.password);
    }
}

String WiFiTimeSync::getConnectionStatus() {
    if (WiFi.status() == WL_CONNECTED) {
        return "Connected";
    } else if (WiFi.status() == WL_NO_SSID_AVAIL) {
        return "WiFi not available";
    } else {
        return "Disconnected";
    }
}

void WiFiTimeSync::syncTime() {
    configTime(3600, 3600, "pool.ntp.org", "time.nist.gov");
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();
}

String WiFiTimeSync::getSynchronizedTime() {
    if (WiFi.status() != WL_CONNECTED) {
        return "WiFi is not connected";
    }
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "Failed to obtain time";
    }
    char timeString[64];
    strftime(timeString, sizeof(timeString), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    return String(timeString);
}

void WiFiTimeSync::connectToWiFi(const String& ssid, const String& password) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid.c_str(), password.c_str());

    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 30000; // Set timeout to 30 seconds

    // Attempt to connect until the timeout is reached
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println(" Failed to connect.");
        // Handle failed connection attempt here (e.g., retry, switch to another network, go to sleep mode)
    } else {
        Serial.println(" Connected.");
    }
}
