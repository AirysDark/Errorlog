#include "CommandLine.h"
#include <Arduino.h>
#define list_get(lst, i) (*(lst))[i]
#define list_set(lst, i, val) (*(lst))[i] = val
#define list_size(lst) ((lst)->size())
#define list_remove(lst, i) ((lst)->remove(i))
bool CommandLine::checkValueExists(LinkedList<String>* cmd_args_list, int index) {
  if (index < list_size(cmd_args_list) - 1) return true; return false;
}
bool CommandLine::apSelected() {
  for (int i = 0; i < list_size(access_points); i++) { if (list_get(access_points, i).selected) return true; } return false;
}
bool CommandLine::hasSSIDs() { return (list_size(ssids) > 0); }
void CommandLine::filterAccessPoints(String filter) {
  LinkedList<String> filters([](const String&){}); filters.add(filter);
  for (int i = 0; i < list_size(access_points); i++) {
    for (int j = 0; j < list_size(&filters); j++) {
      String f = filters[j];
      if (list_get(access_points, i).essid.indexOf(f) >= 0) { auto ap = list_get(access_points, i); ap.selected = true; list_set(access_points, i, ap); }
    }
  }
}
void CommandLine::runCommand(String input) {
  LinkedList<String> cmd_args = this->parseCommand(input, " ");
  if (list_size(&cmd_args) == 0) return;
  if (list_get(&cmd_args, 0) == HELP_CMD) { Serial.println("Help menu");
  } else if (list_get(&cmd_args, 0) == STOPSCAN_CMD) { Serial.println("Stopping scan");
  } else if (list_get(&cmd_args, 0) == GPS_CMD) { Serial.println("GPS command"); }
}
