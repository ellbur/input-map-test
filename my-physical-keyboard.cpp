
#include "my-physical-keyboard.hpp"

PhysKeyboardRevMap::PhysKeyboardRevMap() :
  keys()
{
  addRow(key_top, phys_top_row, false);
  addRow(key_q, phys_q_row, false);
  addRow(key_a, phys_a_row, false);
  addRow(key_z, phys_z_row, false);
  
  addRow(key_top, phys_top_row_shift, true);
  addRow(key_q, phys_q_row_shift, true);
  addRow(key_a, phys_a_row_shift, true);
  addRow(key_z, phys_z_row_shift, true);
}

void PhysKeyboardRevMap::addRow(int codeStart, string const& chars, bool shift) {
  for (size_t i=0; i<chars.length(); i++) {
    int code = codeStart + i;
    char c = chars[i];
    keys[c] = PhysRevKey { code, shift };
  }
}

optional<PhysRevKey> PhysKeyboardRevMap::charToKey(char c) const {
  if (keys.count(c)) {
    return { keys.at(c) };
  }
  else {
    return { };
  }
}

