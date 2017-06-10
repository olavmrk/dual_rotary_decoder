#pragma once
#include "ab_reader.h"
#include "blinker.h"
#include "rotation_pulser.h"

/* This is a decoder for a rotary encoder that alternates between two states when it is rotated.
 * In order to determine the direction we look at which signal changes first. On clockwise rotation
 * signal "A" changes first:
 *
 *         ----------
 *  A      |        |
 *     -----        -------
 *
 *           ----------
 *  B        |        |
 *     -------        |----
 *
 * Signal:
 *      0   A  AB    B  0
 */
class decoder_bar_t {
private:
  ab_reader_t *reader;
  rotation_pulser_t *pulser;
  blinker_t *state_change_blinker;

  enum decoder_state_t {
    RESET,
    X,
    X_CLOCKWISE,
    X_COUNTERCLOCKWISE,
    Y,
    Y_CLOCKWISE,
    Y_COUNTERCLOCKWISE,
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
        // State at reset. We don't know what position the encoder is in, so wait for a stable signal from it.
        if (new_signal == SIGNAL_0) {
          this->current_state = X;
        } else if (new_signal == SIGNAL_AB) {
          this->current_state = Y;
        }
        break;
      case X:
        if (new_signal == SIGNAL_A) {
          this->current_state = X_CLOCKWISE;
        } else if (new_signal == SIGNAL_B) {
          this->current_state = X_COUNTERCLOCKWISE;
        } else if (new_signal == SIGNAL_AB) {
          this->current_state = Y; // We missed a signal, but don't know which direction we turned.
        }
        break;
      case X_CLOCKWISE:
        if (new_signal == SIGNAL_AB) {
          this->current_state = Y;
          this->pulser->pulse_clockwise();
        } else if (new_signal == SIGNAL_0) {
          this->current_state = X;
        }
        break;
      case X_COUNTERCLOCKWISE:
        if (new_signal == SIGNAL_AB) {
          this->current_state = Y;
          this->pulser->pulse_counterclockwise();
        } else if (new_signal == SIGNAL_0) {
          this->current_state = X;
        }
        break;
      case Y:
        if (new_signal == SIGNAL_B) {
          this->current_state = Y_CLOCKWISE;
        } else if (new_signal == SIGNAL_A) {
          this->current_state = Y_COUNTERCLOCKWISE;
        } else if (new_signal == SIGNAL_0) {
          this->current_state = X; // We missed a signal, but don't know which direction we turned.
        }
        break;
      case Y_CLOCKWISE:
        if (new_signal == SIGNAL_0) {
          this->current_state = X;
          this->pulser->pulse_clockwise();
        } else if (new_signal == SIGNAL_AB) {
          this->current_state = Y;
        }
        break;
      case Y_COUNTERCLOCKWISE:
        if (new_signal == SIGNAL_0) {
          this->current_state = X;
          this->pulser->pulse_counterclockwise();
        } else if (new_signal == SIGNAL_AB) {
          this->current_state = Y;
        }
        break;
    }
    if (prev_state != this->current_state) {
      this->state_change_blinker->blink();
    }
  }
};
