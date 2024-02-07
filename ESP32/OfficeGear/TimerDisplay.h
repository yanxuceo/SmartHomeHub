#ifndef TimerDisplay_h
#define TimerDisplay_h
class OfficeTimer {
private:
    unsigned long startTime;
    bool timerRunning;
    unsigned long alertThreshold;

public:
    OfficeTimer() : startTime(0), timerRunning(false), alertThreshold(0) {}

    void start() {
      if (!timerRunning) {
          startTime = millis();
          timerRunning = true;
      }
    }

    void reset() {
      startTime = millis();
    }

    void stop() {
      timerRunning = false;
    }

    void setThreshold(unsigned long thresholdInSeconds) {
      alertThreshold = thresholdInSeconds;
    }

    bool checkThreshold() {
      if (!timerRunning) return false;

      unsigned long currentTime = millis();
      unsigned long elapsedTime = (currentTime - startTime) / 1000; // Convert to seconds

      if (elapsedTime >= alertThreshold) {
          return true;
      }
      return false;
    }

    String getTimerString() {
      if (!timerRunning) return "00:00";

      unsigned long currentTime = millis();
      unsigned long elapsedTime = currentTime - startTime;
      int seconds = (elapsedTime / 1000) % 60;
      int minutes = (elapsedTime / (1000 * 60)) % 60;
      char timeString[6];
      sprintf(timeString, "%02d:%02d", minutes, seconds);

      return String(timeString);
    }

    bool isRunning() const {
      return timerRunning;
    }
};

#endif