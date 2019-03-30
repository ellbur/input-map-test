
// http://stackoverflow.com/questions/27581500/hook-into-linux-key-event-handling/27693340#27693340

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>

#define die(n, str, args...) do { \
  perror(str); \
  exit(n); \
} while(0)

struct Grabbing {
  Grabbing(int fd) :
    fd(fd)
  {
    ioctl(fd, EVIOCGRAB, 1);
  }

  ~Grabbing() {
    ioctl(fd, EVIOCGRAB, 0);
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

int main() {
  printf("Hello!!");
  fflush(stdout);
  
  int fdo, fdi;
  struct uinput_user_dev uidev;
  struct input_event ev;

  fdo = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (fdo < 0) die(3, "error: open");

  fdi = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDONLY);
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
  
  const int leftShift = 42;
  const int rightShift = 54;
  const int capsLock = 58;

  const int pressed = 1;
  const int released = 0;
  const int repeated = 2;
  
  bool movementDown = false;
  bool shiftIsLastPressed = false;

  auto release = [&](int code) {
    sendFullSet(fdo, code, released);
  };

  enum {
    S0,
    S1,
    S2
  } state = S0;
  int workingCode = 0;
  int workingValue = 0;

  while (true) {
    if (read(fdi, &ev, sizeof(struct input_event)) < 0) {
      die(11, "error: read");
    }

    if (state == S0) {
      if (ev.type==4 && ev.code==4) {
        state = S1;
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
        bool send = true;

        if (workingCode==15) {
          if (workingValue==pressed) {
            if (!shiftIsLastPressed) {
              release(leftShift);
              release(rightShift);
            }
            release(capsLock);

            movementDown = true;
            send = false;
          }
          else if (workingValue==released) {
            release(102);
            release(103);
            release(104);
            release(105);
            release(106);
            release(107);
            release(108);
            
            movementDown = false;
            send = false;
          }
          else {
            send = false;
          }
        }
        
        if (movementDown) {
          if (workingCode==36) {
            workingCode = 105;
          }
          else if (workingCode==37) {
            workingCode = 108;
          }
          else if (workingCode==38) {
            workingCode = 106;
          }
          else if (workingCode==23) {
            workingCode = 103;
          }
          else if (workingCode==35) {
            workingCode = 102;
          }
          else if (workingCode==39) {
            workingCode = 107;
          }
          else if (workingCode==22) {
            workingCode = 104;
          }
          else if (workingCode==50) {
            workingCode = 109;
          }
          else if (workingCode==49) {
            send = false;
            if (workingValue == 1) {
              sendFullSet(fdo, 29, 1);
              sendFullSet(fdo, 105, 1);
            }
            else if (workingValue == 2) {
              sendFullSet(fdo, 105, 2);
            }
            else if (workingValue == 0) {
              sendFullSet(fdo, 105, 0);
              sendFullSet(fdo, 29, 0);
            }
          }
          else if (workingCode==51) {
            send = false;
            if (workingValue == pressed) {
              sendFullSet(fdo, 29, pressed);
              sendFullSet(fdo, 106, pressed);
            }
            else if (workingValue == repeated) {
              sendFullSet(fdo, 106, repeated);
            }
            else if (workingValue == released) {
              sendFullSet(fdo, 106, released);
              sendFullSet(fdo, 29, released);
            }
          }
        }

        if (send) {
          shiftIsLastPressed = (workingValue != released) && (
               (workingCode == leftShift)
            || (workingCode == rightShift));
          
          sendFullSet(fdo, workingCode, workingValue);
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

