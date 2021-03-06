
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

int main(int argc, char **argv) {
  if (argc != 1 + 1) {
    fprintf(stderr, "Usage: see-codes <dev>\n");
    fflush(stderr);
    return 1;
  }
  
  char const* path = argv[1];
  
  int fdi;
  struct input_event ev;

  fdi = open(path, O_RDONLY);
  if (fdi < 0) {
    die(4, "error: open");
  }

  while (true) {
    if (read(fdi, &ev, sizeof(struct input_event)) < 0) {
      die(11, "error: read");
    }

    printf("%d %d %d\n", ev.type, ev.code, ev.value);
    fflush(stdout);
  }

  close(fdi);

  return 0;
}

