#include <iostream>

#include "archiver.h"

int main() {
  Archiver archiver {"/home/boyatman/Downloads/correct.data.txt", "/home/boyatman/Downloads/output"};

  /* Archive in binary format */
  archiver.archive();

  /* Reconstruct to ASCII */
  archiver.unarchive();
  return 0;
}
