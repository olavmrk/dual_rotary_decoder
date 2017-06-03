#pragma once
#include "debouncer.h"

static const unsigned SIGNAL_0 = 0;
static const unsigned SIGNAL_A = 1;
static const unsigned SIGNAL_B = 2;
static const unsigned SIGNAL_AB = SIGNAL_A | SIGNAL_B;
static const unsigned SIGNAL_RESET = 5;

class ab_reader_t {
private:
  debouncer_t debouncer;
  unsigned pin_a;
  unsigned pin_b;

  unsigned read_raw() {
    unsigned ret = SIGNAL_0;
    if (digitalRead(this->pin_a)) {
      ret |= SIGNAL_A;
    }
    if (digitalRead(this->pin_b)) {
      ret |= SIGNAL_B;
    }
    return ret;
  }

public:
  void setup(unsigned pin_a, unsigned pin_b) {
    this->pin_a = pin_a;
    this->pin_b = pin_b;
    pinMode(this->pin_a, INPUT_PULLUP);
    pinMode(this->pin_b, INPUT_PULLUP);
    this->debouncer.setup(SIGNAL_RESET, 5);
  }

  unsigned read() {
    unsigned raw_signal = this->read_raw();
    return this->debouncer.update(raw_signal);
  }
};
