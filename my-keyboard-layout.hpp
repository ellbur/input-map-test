
#pragma once

#include "my-physical-keyboard.hpp"

#include <string>
using std::string;

#include <map>
using std::map;

const string my_top_row = "17531902468`";
const string my_q_row = ";,.pyfgcrl~@";
const string my_a_row = "aoeuidhtns-";
const string my_z_row = "'qjkxbmwvz";

const string my_top_row_shift = "";
const string my_q_row_shift = ":<>PYFGCRL?^";
const string my_a_row_shift = "AOEUIDHTNS@";
const string my_z_row_shift = "\"QJKXBMWVZ";

const string my_top_row_altgr = "";
const string my_q_row_altgr = " {}% \\*][|  ";
const string my_a_row_altgr = "   = &)(/_$";
const string my_z_row_altgr = "     !+#  ";

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

struct MyBasicMappings {
  map<TypedKey, PhysRevKey> mappings;
  MyBasicMappings(PhysKeyboardRevMap const& revMap);

  private:
    void addRow(PhysKeyboardRevMap const& revMap, int codeStart, string const& chars, bool hasShift, bool hasAltGR);
};

