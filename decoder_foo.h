#pragma once
#include "ab_reader.h"
#include "blinker.h"
#include "rotation_pulser.h"

/* This is a decoder for a rotary encoder that has the following pattern for each tick rotated clockwise:
 * (A is the pin to the left, B is the pin to the right. The middle pin is connected to ground.)
 *
 *     -----    --------------
 *  A      |    |
 *         ------
 *
 *     -----         ---------
 *  B      |         |
 *         -----------
 *
 * Signal:
 *     AB    0    B     AB
 */
class decoder_foo_t {
private:
  ab_reader_t *reader;
  rotation_pulser_t *pulser;
  blinker_t *state_change_blinker;

  enum decoder_state_t {
    RESET,
    NEUTRAL,
    MOVING_CLOCKWISE_1,
    MOVING_CLOCKWISE_2,
    MOVING_COUNTERCLOCKWISE_1,
    MOVING_COUNTERCLOCKWISE_2,
  } current_state;

public:
  void setup(ab_reader_t &reader, rotation_pulser_t &pulser, blinker_t &state_change_blinker) {
    this->reader = &reader;
    this->pulser = &pulser;
    this->state_change_blinker = &state_change_blinker;
    this->current_state = RESET;
  }
  void update() {
    this->pulser->update();
    unsigned new_signal = this->reader->read();
    decoder_state_t prev_state = this->current_state;
    switch (this->current_state) {
      case RESET:
        // State at reset. Waiting for the neutral signal
        if (new_signal == SIGNAL_AB) {
          this->current_state = NEUTRAL;
        }
        break;
      case NEUTRAL:
        // The neutral state, where we wait for the user to start turning the button.
        if (new_signal == SIGNAL_0) {
          this->current_state = MOVING_CLOCKWISE_1;
        } else if (new_signal == SIGNAL_B) {
          this->current_state = MOVING_COUNTERCLOCKWISE_1;
        }
        break;
      case MOVING_CLOCKWISE_1:
        if (new_signal == SIGNAL_B) {
          this->current_state = MOVING_CLOCKWISE_2;
        } else if (new_signal == SIGNAL_AB) {
          this->current_state = NEUTRAL;
        }
        break;
      case MOVING_CLOCKWISE_2:
        if (new_signal == SIGNAL_AB) {
          this->current_state = NEUTRAL;
          this->pulser->pulse_clockwise();
        } else if (new_signal == SIGNAL_0) {
          this->current_state = MOVING_CLOCKWISE_1;
        }
        break;
      case MOVING_COUNTERCLOCKWISE_1:
        if (new_signal == SIGNAL_0) {
          this->current_state = MOVING_COUNTERCLOCKWISE_2;
        } else if (new_signal == SIGNAL_AB) {
          this->current_state = NEUTRAL;
        }
        break;
      case MOVING_COUNTERCLOCKWISE_2:
        if (new_signal == SIGNAL_AB) {
          this->current_state = NEUTRAL;
          this->pulser->pulse_counterclockwise();
        } else if (new_signal == SIGNAL_B) {
          this->current_state = MOVING_COUNTERCLOCKWISE_1;
        }
        break;
    }
    if (prev_state != this->current_state) {
      this->state_change_blinker->blink();
    }
  }
};
