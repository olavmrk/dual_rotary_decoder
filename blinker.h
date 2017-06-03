#pragma once

class blinker_t {
private:
  unsigned pin;
  unsigned long pulse_width;
  unsigned long pause_width;
  
  enum {
    STATE_OFF,
    STATE_PULSE,
    STATE_PAUSE,
  } state;
  unsigned long state_from;
public:
  void setup(unsigned pin, unsigned long pulse_width, unsigned pause_width) {
    this->pin = pin;
    this->pulse_width = pulse_width;
    this->pause_width = pause_width;
    this->state = STATE_OFF;
    pinMode(this->pin, OUTPUT);
    digitalWrite(this->pin, 0);
  }

  void blink() {
    if (this->state != STATE_OFF) {
      // We are not ready to send another pulse -- still working on the previous.
      return;
    }
    this->state = STATE_PULSE;
    digitalWrite(this->pin, 1);
    this->state_from = millis();
  }

  void update() {
    unsigned long state_duration = millis() - this->state_from;
    if (this->state == STATE_PULSE && state_duration > this->pulse_width) {
      this->state = STATE_PAUSE;
      this->state_from = millis();
      digitalWrite(this->pin, 0);
    } else if (this->state == STATE_PAUSE && state_duration > this->pause_width) {
      this->state = STATE_OFF;
    }
  }
};
