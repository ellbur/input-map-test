
#include "my-keyboard-layout.hpp"
#include <iostream>

using std::pair;
using std::cout;
using std::string;

string b2s(bool b) {
  if (b) {
    return "X";
  }
  else {
    return " ";
  }
}

int main() {
  PhysKeyboardRevMap revMap;

  //map<char, PhysRevKey> &keys = revMap.keys;
  //for (pair<char, PhysRevKey> key : keys) {
  //  char c = key.first;
  //  PhysRevKey t = key.second;
  //  
  //  cout << c << " " << t.keyCode << "[" << b2s(t.needsShift) << "]\n";
  //}
  
  MyBasicMappings mbm(revMap);
  
  for (pair<TypedKey, PhysRevKey> m : mbm.mappings) {
    TypedKey f = m.first;
    PhysRevKey t = m.second;
    
    cout << f.code << "[" << f.shift << "][" << f.altGR << "]" " -> " << t.keyCode << "[" << b2s(t.needsShift) << "]" << "\n";
  }
}

