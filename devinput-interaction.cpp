
#include "devinput-interaction.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <string.h>

#include <iostream>
#include <stdexcept>
#include "third-party/json.hpp"
#include <thread>

using nlohmann::json;
using std::cout;

#define die(str) do { \
  perror(str); \
  throw std::runtime_error(str); \
} while(0)

struct Grabbing {
  Grabbing(int fd) :
    fd(fd)
  {
    if (ioctl(fd, EVIOCGRAB, 1)) {
      die("Failed to grab input device");
    }
  }

  ~Grabbing() {
    if (ioctl(fd, EVIOCGRAB, 0)) {
      perror("Failed to ungrab input device");
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

void runDevInputLoop(string const& inDevPath, string const& outDevName, bool printDiagnostics, DevInputHandler handler) {
  int fdo, fdi;
  struct input_event ev;

  fdo = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fdo < 0) {
    die("Failed to open /dev/uinput");
  }

  fdi = open(inDevPath.c_str(), O_RDONLY);
  if (fdi < 0) {
    die((string("Failed to open input device path ") + inDevPath).c_str());
  }

  // This consumes the event so X doesn't use it.
  Grabbing grabbing(fdi);

  if (ioctl(fdo, UI_SET_EVBIT, EV_SYN) < 0) die("error: ioctl");
  if (ioctl(fdo, UI_SET_EVBIT, EV_KEY) < 0) die("error: ioctl");
  if (ioctl(fdo, UI_SET_EVBIT, EV_MSC) < 0) die("error: ioctl");

  for (int i=0; i<KEY_MAX; ++i) {
    if (ioctl(fdo, UI_SET_KEYBIT, i) < 0) die("error: ioctl");
  }

  struct uinput_user_dev uidev;
  memset(&uidev, 0, sizeof(uidev));
  strncpy(uidev.name, outDevName.c_str(), UINPUT_MAX_NAME_SIZE-1);
  uidev.id.bustype = BUS_USB;
  uidev.id.vendor  = 0x1;
  uidev.id.product = 0x1;
  uidev.id.version = 1;

  if (write(fdo, &uidev, sizeof(uidev)) < 0) die("error: write");
  if (ioctl(fdo, UI_DEV_CREATE) < 0) die("error: ioctl");
  
  // Some software won't notice this device unless it presses some keys
  for (int i=0; i<3; i++) {
    sendFullSet(fdo, 29, pressed);
    sendFullSet(fdo, 29, released);
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
  }
  
  while (true) {
    if (read(fdi, &ev, sizeof(struct input_event)) < 0) {
      die("error: read");
    }
    
    if (printDiagnostics) {
      json j;
      j["type"] = "in";
      json data;
      data["type"] = ev.type;
      data["code"] = ev.code;
      data["value"] = ev.value;
      j["data"] = data;
      cout << j << "\n"; cout.flush();
    }

    if (ev.type==1) {
      if (printDiagnostics) {
        json j;
        j["type"] = "in-action";
        json data;
        data["code"] = ev.code;
        data["value"] = ev.value;
        j["data"] = data;
        cout << j << "\n"; cout.flush();
      }
      
      list<DevInputEvent> out = handler({ ev.code, (DevInputValue) ev.value });
      
      for (DevInputEvent const& ev : out) {
        if (printDiagnostics) {
          json j;
          j["type"] = "out-action";
          json data;
          data["code"] = ev.code;
          data["value"] = (int) ev.value;
          j["data"] = data;
          cout << j << "\n"; cout.flush();
        }
        sendFullSet(fdo, ev.code, ev.value);
      }
    }
    
    if (printDiagnostics) {
      json j;
      j["type"] = "loop-end";
      cout << j << "\n"; cout.flush();
    }
  }

  if (ioctl(fdo, UI_DEV_DESTROY) < 0) perror("error: ioctl");

  close(fdi);
  close(fdo);
}

