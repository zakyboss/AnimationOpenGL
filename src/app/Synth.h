#pragma once
#include <string>
#include <vector>

// Sound effects made in code from sine waves and filtered noise, for the
// moments we had no recording of. Writes 16-bit stereo samples at the given
// rate; returns false for an unknown name.
//
//   sting   the shock when she sees them: a deep boom, a heartbeat and a sour high chord
//   thud    a small box landing on a wooden floor, with one little bounce
//   crunch  a cardboard box stamped flat
//   whoosh  something swung fast through the air (a thrown jacket, a snatched phone)
//   shove   a push: a rush of air, a thump on the chest and stumbling feet
bool synthesize(const std::string& name, int rate, std::vector<short>& out);
