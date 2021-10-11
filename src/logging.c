#include "main.h"
#include "string.h"

void get_event_str(char* str, Event e) {
  switch (e) {
    case WORK_E:
      strncpy(str, "Work", EVENT_STR_SIZE);
      break;
    case COMPLETE_E:
      strncpy(str, "Complete", EVENT_STR_SIZE);
      break;
    case ASK_E:
      strncpy(str, "Ask", EVENT_STR_SIZE);
      break;
    case RECEIVE_E:
      strncpy(str, "Receive", EVENT_STR_SIZE);
      break;
    case END_E:
      strncpy(str, "End", EVENT_STR_SIZE);
      break;
    case SLEEP_E:
      strncpy(str, "Sleep", EVENT_STR_SIZE);
      break;
    default:
      break;
  }
}