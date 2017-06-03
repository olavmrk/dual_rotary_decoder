#pragma once
#include "ab_reader.h"
#include "blinker.h"
#include "rotation_pulser.h"

/* This is a decoder for a rotary encoder that changes between three signals when it is rotated.
 * It has the following states, where each state is the result of one position on the encoder:
 * (A is the pin to the left, B is the pin to the rigth. The middle pin is connected to ground.)
 *
 *         ------------
 *  A      |          |
 *     -----          -----
 *
 *               ------
 *  B            |    |
 *     -----------    |----
 *
 * Signal:
 *      0    A     AB   0
 */
class decoder_bar_t {
private:
  ab_reader_t *reader;
  rotation_pulser_t *pulser;
  blinker_t *state_change_blinker;

  enum decoder_state_t {
    RESET,
    ONE,
    TWO,
    THREE,
  } current_state;

  static const unsigned SIGNAL_POS_ONE = SIGNAL_0;
  static const unsigned SIGNAL_POS_TWO = SIGNAL_A;
  static const unsigned SIGNAL_POS_THREE = SIGNAL_AB;
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
        // State at reset. We don't know what position the encoder is in, so wait for a stable signal from it.
        if (new_signal == SIGNAL_POS_ONE) {
          this->current_state = ONE;
        } else if (new_signal == SIGNAL_POS_TWO) {
          this->current_state = TWO;
        } else if (new_signal == SIGNAL_POS_THREE) {
          this->current_state = THREE;
        }
        break;
      case ONE:
        if (new_signal == SIGNAL_POS_TWO) {
          this->current_state = TWO;
          this->pulser->pulse_clockwise();
        } else if (new_signal == SIGNAL_POS_THREE) {
          this->current_state = THREE;
          this->pulser->pulse_counterclockwise();
        }
        break;
      case TWO:
        if (new_signal == SIGNAL_POS_THREE) {
          this->current_state = THREE;
          this->pulser->pulse_clockwise();
        } else if (new_signal == SIGNAL_POS_ONE) {
          this->current_state = ONE;
          this->pulser->pulse_counterclockwise();
        }
        break;
      case THREE:
        if (new_signal == SIGNAL_POS_ONE) {
          this->current_state = ONE;
          this->pulser->pulse_clockwise();
        } else if (new_signal == SIGNAL_POS_TWO) {
          this->current_state = TWO;
          this->pulser->pulse_counterclockwise();
        }
        break;
    }
    if (prev_state != this->current_state) {
      this->state_change_blinker->blink();
    }
  }
};
