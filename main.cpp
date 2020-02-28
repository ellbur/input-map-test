
// http://stackoverflow.com/questions/27581500/hook-into-linux-key-event-handling/27693340#27693340

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <map>
#include <set>
#include <list>
#include <boost/range/adaptor/reversed.hpp>
#include "my-physical-keyboard.hpp"
#include "my-keyboard-layout.hpp"
#include <thread>
#include <chrono>

using std::map;
using std::set;
using std::list;
using boost::adaptors::reverse;

#define die(n, str, args...) do { \
  perror(str); \
  exit(n); \
} while(0)

struct Grabbing {
  Grabbing(int fd) :
    fd(fd)
  {
    if (ioctl(fd, EVIOCGRAB, 1)) {
      die(1, "Failed to grab input device");
    }
  }

  ~Grabbing() {
    if (ioctl(fd, EVIOCGRAB, 0)) {
      die(1, "Failed to ungrab input device");
    }
  }

  private:
    int fd;
};

void sendTypeCodeValue(int fdo, int type, int code, int value) {
  struct input_event ev2;
  ev2.time.tv_sec = 0;
  ev2.time.tv_usec = 0;
  ev2.type = type;
  ev2.code = code;
  ev2.value = value;
  write(fdo, &ev2, sizeof(struct input_event));
}

void sendFullSet(int fdo, int code, int value) {
  sendTypeCodeValue(fdo, 4, 4, code);
  sendTypeCodeValue(fdo, 1, code, value);
  sendTypeCodeValue(fdo, 0, 0, 0);
}

int main(int argc, char **argv) {
  if (argc != 1 + 1) {
    fprintf(stderr, "Usage: map-keyboard <input-file>\n");
    return 1;
  }
  
  string keyboardFilePath(argv[1]);
  
  int fdo, fdi;
  struct uinput_user_dev uidev;
  struct input_event ev;

  fdo = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fdo < 0) die(3, "error: open");

  fdi = open(keyboardFilePath.c_str(), O_RDONLY);
  if (fdi < 0) die(4, "error: open");

  // This consumes the event so X doesn't use it.
  Grabbing grabbing(fdi);

  if (ioctl(fdo, UI_SET_EVBIT, EV_SYN) < 0) die(5, "error: ioctl");
  if (ioctl(fdo, UI_SET_EVBIT, EV_KEY) < 0) die(6, "error: ioctl");
  if (ioctl(fdo, UI_SET_EVBIT, EV_MSC) < 0) die(7, "error: ioctl");

  for (int i=0; i<KEY_MAX; ++i) {
    if (ioctl(fdo, UI_SET_KEYBIT, i) < 0) die(8, "error: ioctl");
  }

  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-sample");
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1;
  uidev.id.product = 0x1;
  uidev.id.version = 1;

  if (write(fdo, &uidev, sizeof(uidev)) < 0) die(9, "error: write");
  if (ioctl(fdo, UI_DEV_CREATE) < 0) die(10, "error: ioctl");
  
  const int leftControl = 29;
  const int capsLock = 58;
  const int myTabKey = 15;

  const int pressed = 1;
  const int released = 0;
  const int repeated = 2;
  
  bool movementDown = false;

  struct Action {
    list<int> keys;
  };
  
  map<int, Action> movementMappings;
  movementMappings[36] = { { 105 } };
  movementMappings[37] = { { 108 } };
  movementMappings[38] = { { 106 } };
  movementMappings[23] = { { 103 } };
  movementMappings[35] = { { 102 } };
  movementMappings[39] = { { 107 } };
  movementMappings[22] = { { 104 } };
  movementMappings[50] = { { 109 } };
  movementMappings[49] = { { leftControl, 105 } };
  movementMappings[51] = { { leftControl, 106 } };
  
  set<int> nativePressedKeys;
  set<int> ordinaryPressedKeys;
  set<int> magicPressedKeys;
  
  auto press = [&](int code) {
    sendFullSet(fdo, code, pressed);
  };
  
  auto release = [&](int code) {
    sendFullSet(fdo, code, released);
  };
  
  auto ordinaryPress = [&](int code) {
    ordinaryPressedKeys.insert(code);
    press(code);
  };
  
  auto ordinaryRelease = [&](int code) {
    ordinaryPressedKeys.erase(code);
    if (! magicPressedKeys.count(code)) {
      release(code);
    }
  };
  
  auto magicPress = [&](int code) {
    magicPressedKeys.insert(code);
    press(code);
  };
  
  auto magicRelease = [&](int code) {
    magicPressedKeys.erase(code);
    if (! ordinaryPressedKeys.count(code)) {
      release(code);
    }
  };
  
  auto applyAction = [&](int native, Action const& action) {
    ordinaryRelease(native);
    for (int key : action.keys) {
      magicPress(key);
    }
  };
  
  auto swallowAction = [&](Action const& action) {
    for (int key : reverse(action.keys)) {
      magicRelease(key);
    }
  };
  
  enum {
    S0,
    S1,
    S2
  } state = S0;
  int workingCode = 0;
  int workingValue = 0;
  
  for (int i=0; i<3; i++) {
    printf("Go %d\n", i);
    fflush(stdout);
    press(leftControl);
    release(leftControl);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  
  while (true) {
    if (read(fdi, &ev, sizeof(struct input_event)) < 0) {
      die(11, "error: read");
    }

    if (state == S0) {
      if (ev.type==4 && ev.code==4) {
        state = S1;
      }
      else if (ev.type==1 && ev.value != repeated) {
        workingCode = ev.code;
        workingValue = ev.value;
        state = S2;
      }
    }
    else if (state == S1) {
      if (ev.type==1 && ev.value != repeated) {
        workingCode = ev.code;
        workingValue = ev.value;
        state = S2;
      }
      else {
        state = S0;
      }
    }
    else if (state == S2) {
      if (ev.type==0 && ev.code==0 && ev.value==0) {
        if (workingValue == pressed) {
          nativePressedKeys.insert(workingCode);
        }
        else if (workingValue == released) {
          nativePressedKeys.erase(workingCode);
        }
        
        if (workingCode==myTabKey) {
          if (workingValue==pressed) {
            for (int key : nativePressedKeys) {
              if (movementMappings.count(key)) {
                applyAction(key, movementMappings[key]);
              }
            }
            
            release(capsLock);
            movementDown = true;
          }
          else if (workingValue==released) {
            for (int key : nativePressedKeys) {
              if (movementMappings.count(key)) {
                swallowAction(movementMappings[key]);
              }
            }
            
            movementDown = false;
          }
        }
        else if (movementDown) {
          if (workingValue == pressed) {
            if (movementMappings.count(workingCode)) {
              Action action = movementMappings[workingCode];
              for (int key : action.keys) {
                magicPress(key);
              }
            }
            else {
              ordinaryPress(workingCode);
            }
          }
          else if (workingValue == released) {
            if (movementMappings.count(workingCode)) {
              Action action = movementMappings[workingCode];
              for (int key : reverse(action.keys)) {
                magicRelease(key);
              }
            }
            else {
              ordinaryRelease(workingCode);
            }
          }
        }
        else {
          if (workingValue == pressed) {
            ordinaryPress(workingCode);
          }
          else if (workingValue == released) {
            ordinaryRelease(workingCode);
          }
        }

        state = S0;
      }
    }
  }

  if (ioctl(fdo, UI_DEV_DESTROY) < 0) die(12, "error: ioctl");

  close(fdi);
  close(fdo);

  return 0;
}

