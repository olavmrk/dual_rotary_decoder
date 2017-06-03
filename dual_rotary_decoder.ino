#include "decoder_foo.h"
#include "decoder_bar.h"

blinker_t state_change_blinker;

ab_reader_t foo_input;
rotation_pulser_t foo_output;
decoder_foo_t foo_decoder;

ab_reader_t bar_input;
rotation_pulser_t bar_output;
decoder_bar_t bar_decoder;

void setup() {
  state_change_blinker.setup(13, 100, 100); // LED is connected to pin 13. Blink for 100ms on state change.

  foo_input.setup(A0, A1); // Inputs connected to A0 & A1.
  foo_output.setup(11, 12, 50, 100); // Output connected to pins 11 & 12. Sends pulses of 50ms, with minimum 100ms between each pulse.
  foo_decoder.setup(foo_input, foo_output, state_change_blinker);

  bar_input.setup(A4, A5); // Inputs connected to A4 & A5.
  bar_output.setup(2, 3, 50, 100); // Output connected to pins 2 & 3. Sends pulses of 50ms, with minimum 100ms between each pulse.
  bar_decoder.setup(bar_input, bar_output, state_change_blinker);
}

void loop() {
  state_change_blinker.update();
  foo_decoder.update();
  bar_decoder.update();
}
