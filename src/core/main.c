#include <stdio.h>
#include <stdlib.h>
#include "../vm/vm.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Usage: sneklang <script.snek>\n");
    return 1;
  }

  const char *script_path = argv[1];
  FILE *file = fopen(script_path, "r");
  if (!file)
  {
    printf("Error: Could not open script %s\n", script_path);
    return 1;
  }

  // Read script contents
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *source = malloc(length + 1);
  fread(source, 1, length, file);
  fclose(file);
  source[length] = '\0';

  // Initialize VM
  vm_t *vm = vm_new();
  printf("Executing %s...\n", script_path);

  // TODO: Call the parser and execution engine (not implemented yet)
  // execute_script(vm, source);

  vm_free(vm);
  free(source);
  return 0;
}
