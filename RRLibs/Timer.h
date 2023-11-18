/*
 * Ruby Conn-ectivity
 * 
 *  Timer
 */
#pragma once


class Timer {
  private:
    unsigned long fire = 0;
    unsigned int duration = 0;
  public:
    Timer() {}
    Timer(unsigned int duration) {
      start(duration);
    }
    void start(unsigned int duration) {
      this->duration = duration;
      this->fire = millis() + duration;
    }
    void restart() {
      start(this->duration);
    }
    
    bool hasFired() {
      if ((fire != 0) && (millis() > fire)) {
        fire = 0;
        return true;
      } else {
        return false;
      }
    }
    bool hasFiredMulti() {
      if (hasFired()) {
        restart();
        return true;
      } else {
        return false;
      }
    }
};
