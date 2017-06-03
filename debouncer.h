#pragma once

class debouncer_t {
private:
  unsigned next_signal;
  unsigned current_signal;
  unsigned long stable_since;
  unsigned long debounce_time;
public:
  void setup(unsigned init_signal, unsigned long debounce_time) {
    this->debounce_time = debounce_time;
    this->next_signal = this->current_signal = init_signal;
    this->stable_since = millis();
  }

  unsigned update(unsigned raw_signal) {
    unsigned long current_time = millis();
    if (raw_signal != this->next_signal) {
      // Signal has changed
      this->next_signal = raw_signal;
      this->stable_since = current_time;
    } else if (current_time - this->stable_since > debounce_time) {
      // Signal has been stable for the required period
      // Update the output signal.
      this->current_signal = this->next_signal;
      this->stable_since = current_time; // Reseting the stable timer ensures that we never overflow it.
    }
    return this->current_signal;
  }
};
