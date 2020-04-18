
#pragma once

#include <set>
#include <map>

#include "keyboard-model.hpp"
#include "devinput-model.hpp"

using std::set;
using std::map;

struct RemappingHander {
  RemappingHander(FullMappingSet mappings, set<int> shiftKeys, set<int> altGrKeys);
  
  list<DevInputEvent> handle(DevInputEvent const&);
  
  private:
    FullMappingSet mappings;
    set<int> shiftKeys;
    set<int> altGrKeys;
    map<int, optional<PhysRevKey>> inPressedKeys;
    set<int> outPressedKeys;
    set<int> outPressedModifiers;
};

