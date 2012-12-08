#include <stdio.h>
#include "../src/quicksort.h"

int main (int argc, char *argv[])
{
  if (argc != 2) {
    printf("Really? Even in test?\n");
    return 1;
  }

  mmuInit();
  tString text = strCreate(argv[1]);

  printf("Text: %s\n"
         "Text len: %u\n", text.data, text.len);
  quicksort(&text.data, 0, text.len-1);
  printf("Pozice je: %s\n", text.data);

  strFree(&text);

  mmuGlobalFree();

  return 0;
}
