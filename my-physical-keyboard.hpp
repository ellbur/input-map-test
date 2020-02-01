
#pragma once

#include <string>
#include <optional>
#include <map>

using std::string;
using std::optional;
using std::map;

// 2 16 30 44

constexpr int key_top = 1;
constexpr int key_q = 16;
constexpr int key_a = 30;
constexpr int key_z = 44;

const string phys_top_row = "`1234567890-=";
const string phys_q_row = "qwertyuiop[]";
const string phys_a_row = "asdfghjkl;'";
const string phys_z_row = "zxcvbnm,./";

const string phys_top_row_shift = "~!@#$%^&*()_+";
const string phys_q_row_shift = "QWERTYUIOP{}";
const string phys_a_row_shift = "ASDFGHJKL:\"";
const string phys_z_row_shift = "ZXCVBNM<>?";

struct PhysRevKey {
  int keyCode;
  bool needsShift;
};

struct PhysKeyboardRevMap {
  map<char, PhysRevKey> keys;
  PhysKeyboardRevMap();
  
  optional<PhysRevKey> charToKey(char c) const;
  
  private:
    void addRow(int codeStart, string const& chars, bool shift);
};

