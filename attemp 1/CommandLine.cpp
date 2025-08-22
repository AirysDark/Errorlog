#include "CommandLine.h"
#include <Arduino.h>

// ---- LinkedList compatibility helpers ----
template <typename T>
static inline T& LL_get(LinkedList<T>* L, int i) { return (*L)[i]; }

template <typename T>
static inline void LL_set(LinkedList<T>* L, int i, const T& v) { (*L)[i] = v; }

template <typename T>
static inline int LL_size(LinkedList<T>* L) { return L->size(); }

template <typename T>
static inline bool LL_remove_index(LinkedList<T>* L, int i) { return L->remove((*L)[i]); }
// ------------------------------------------------

bool CommandLine::checkValueExists(LinkedList<String>* cmd_args_list, int index) {
  return index < (LL_size(cmd_args_list) - 1);
}

bool CommandLine::apSelected() {
  for (int i = 0; i < LL_size(access_points); i++) {
    if (LL_get(access_points, i).selected) return true;
  }
  return false;
}

bool CommandLine::hasSSIDs() {
  return LL_size(ssids) > 0;
}

void CommandLine::filterAccessPoints(String filter) {
  LinkedList<String> filters([](const String&){});
  filters.add(filter);

  for (int i = 0; i < LL_size(access_points); i++) {
    for (int j = 0; j < LL_size(&filters); j++) {
      String f = filters[j];
      auto ap = LL_get(access_points, i);
      if (ap.essid.indexOf(f) >= 0) {
        ap.selected = true;
        LL_set(access_points, i, ap);
      }
    }
  }
}

void CommandLine::runCommand(String input) {
  LinkedList<String> cmd_args = this->parseCommand(input, " ");
  if (LL_size(&cmd_args) == 0) return;

  String cmd0 = LL_get(&cmd_args, 0);

  if (cmd0 == HELP_CMD) {
    Serial.println("Help menu");
  } else if (cmd0 == STOPSCAN_CMD) {
    Serial.println("Stopping scan");
  } else if (cmd0 == GPS_CMD) {
    Serial.println("GPS command");
  }
  // Extend with more cases as needed, always using LL_get/LL_set/LL_size
}