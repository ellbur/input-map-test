
#pragma once

#include <vector>
#include <variant>
#include <map>
#include <optional>

using std::vector;
using std::variant;
using std::map;
using std::optional;

struct PhysRevKey {
  int keyCode;
  bool needsShift;
};

struct PhysicalLayout {
  int tildeCode;
  int k1Code;
  int qCode;
  int aCode;
  int zCode;
  map<char, PhysRevKey> keys;
};

struct NullLayoutKey { };
struct CharLayoutKey { char c; };
struct CodeLayoutKey { int code; };

typedef variant<NullLayoutKey, CharLayoutKey, CodeLayoutKey> LayoutKey;

struct KeyboardLayout {
  LayoutKey tilde;
  LayoutKey tildeShift;
  LayoutKey tildeAltGr;
  
  vector<LayoutKey> k1Row;
  vector<LayoutKey> qRow;
  vector<LayoutKey> aRow;
  vector<LayoutKey> zRow;
  
  vector<LayoutKey> k1RowShift;
  vector<LayoutKey> qRowShift;
  vector<LayoutKey> aRowShift;
  vector<LayoutKey> zRowShift;
  
  vector<LayoutKey> k1RowAltGr;
  vector<LayoutKey> qRowAltGr;
  vector<LayoutKey> aRowAltGr;
  vector<LayoutKey> zRowAltGr;
};

struct TypedKey {
  int code;
  bool shift;
  bool altGR;
};

inline bool operator<(TypedKey const& a, TypedKey const& b) {
  if (a.code < b.code) return true;
  else if (a.code > b.code) return false;
  else if (a.shift < b.shift) return true;
  else if (a.shift > b.shift) return false;
  else if (a.altGR < b.altGR) return true;
  else if (a.altGR > b.altGR) return false;
  else return false;
}

struct FullMappingSet {
  map<TypedKey, optional<PhysRevKey>> mappings;
};

