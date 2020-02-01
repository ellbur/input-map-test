
#include "my-keyboard-layout.hpp"
#include "my-keyboard-layout.hpp"

#include <iostream>
using std::cout;

MyBasicMappings::MyBasicMappings(PhysKeyboardRevMap const& revMap) :
  mappings()
{
  addRow(revMap, key_top, my_top_row, false, false);
  addRow(revMap, key_q, my_q_row, false, false);
  addRow(revMap, key_a, my_a_row, false, false);
  addRow(revMap, key_z, my_z_row, false, false);
  
  addRow(revMap, key_top, my_top_row_shift, true, false);
  addRow(revMap, key_q, my_q_row_shift, true, false);
  addRow(revMap, key_a, my_a_row_shift, true, false);
  addRow(revMap, key_z, my_z_row_shift, true, false);
}

void MyBasicMappings::addRow(PhysKeyboardRevMap const& revMap, int codeStart, string const& chars, bool hasShift, bool hasAltGR) {
  for (size_t i=0; i<chars.length(); i++) {
    char c = chars[i];
    if (c != ' ') {
      int sourceCode = codeStart + i;
      TypedKey tk { sourceCode, hasShift, hasAltGR };
      optional<PhysRevKey> rk = revMap.charToKey(c);
      
      if (rk) {
        mappings[tk] = *rk;
      }
      else {
        cout << "No physical mapping for char " << c << "(" << (int)c << ")" << "\n"; cout.flush();
      }
    }
  }
}

