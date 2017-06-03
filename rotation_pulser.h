#pragma once

class rotation_pulser_t {
private:
  unsigned pin_clockwise;
  unsigned pin_counterclockwise;
  unsigned long pulse_width;
  unsigned long pause_width;
  
  enum {
    STATE_OFF,
    STATE_PULSE_CLOCKWISE,
    STATE_PULSE_COUNTERCLOCKWISE,
    STATE_PAUSE,
  } state;
  unsigned long state_from;
public:
  void setup(unsigned pin_clockwise, unsigned pin_counterclockwise, unsigned long pulse_width, unsigned pause_width) {
    this->pin_clockwise = pin_clockwise;
    this->pin_counterclockwise = pin_counterclockwise;
    this->pulse_width = pulse_width;
    this->pause_width = pause_width;
    this->state = STATE_OFF;
    pinMode(this->pin_clockwise, OUTPUT);
    digitalWrite(this->pin_clockwise, 0);
    pinMode(this->pin_counterclockwise, OUTPUT);
    digitalWrite(this->pin_counterclockwise, 0);
  }

  void pulse_clockwise() {
    if (this->state != STATE_OFF) {
      // We are not ready to send another pulse -- still working on the previous.
      return;
    }
    this->state = STATE_PULSE_CLOCKWISE;
    digitalWrite(this->pin_clockwise, 1);
    this->state_from = millis();
  }

  void pulse_counterclockwise() {
    if (this->state != STATE_OFF) {
      // We are not ready to send another pulse -- still working on the previous.
      return;
    }
    this->state = STATE_PULSE_COUNTERCLOCKWISE;
    digitalWrite(this->pin_counterclockwise, 1);
    this->state_from = millis();
  }

  void update() {
    unsigned long state_duration = millis() - this->state_from;
    if (this->state == STATE_PULSE_CLOCKWISE && state_duration > this->pulse_width) {
      this->state = STATE_PAUSE;
      this->state_from = millis();
      digitalWrite(this->pin_clockwise, 0);
    } else if (this->state == STATE_PULSE_COUNTERCLOCKWISE && state_duration > this->pulse_width) {
      this->state = STATE_PAUSE;
      this->state_from = millis();
      digitalWrite(this->pin_counterclockwise, 0);
    } else if (this->state == STATE_PAUSE && state_duration > this->pause_width) {
      this->state = STATE_OFF;
    }
  }
};
