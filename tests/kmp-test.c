#include <stdio.h>
#include "../src/kmp.h"

int main (int argc, char *argv[])
{
  if (argc != 3) {
    printf("Really? Even in test?\n");
    return 1;
  }

  mmuInit();
  tString text = strCreate(argv[1]);
  tString search = strCreate(argv[2]);

  int position = kmpSearch(text, search);
  printf("Text: %s\nHledany string: %s\nPozice je: %d\n",text.data, search.data, position);

  return 0;
}
