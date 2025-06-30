#ifndef TIME_SCHEDULER_MANAGER_H
#define TIME_SCHEDULER_MANAGER_H

#include <Arduino.h>

class TimeSchedulerManager {
public:
  bool begin() { Serial.println("⏰ Scheduler stub inicializado"); return true; }
  void update() {}
};

#endif 