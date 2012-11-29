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

  printf("Text: %s\n"
         "Text len: %u\n", text.data, text.len);
  printf("Hledany string: %s\n"
         "Hledany len: %u\n", search.data, search.len);
  int position = kmpSearch(text, search);
  printf("Pozice je: %d\n", position);

  strFree(&text);
  strFree(&search);

  mmuGlobalFree();

  return 0;
}
