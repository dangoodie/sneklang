#include "../lexer/lexer.h"
#include "../vm/vm.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: sneklang <script.snek>\n");
    return 1;
  }

  const char *script_path = argv[1];
  FILE *file = fopen(script_path, "r");
  if (!file) {
    printf("Error: Could not open script %s\n", script_path);
    return 1;
  }

  // Read script contents
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  fseek(file, 0, SEEK_SET);
  char *source = malloc(length + 1);
  if (!source) {
    printf("Error: Could not allocate memory for script\n");
    fclose(file);
    return 1;
  }

  fread(source, 1, length, file);
  source[length] = '\0';
  fclose(file);

  // Store pointer to source for later freeing
  char *original_source = source;

  // Initialize VM
  vm_t *vm = vm_new();
  printf("Executing %s...\n", script_path);

  // Print script contents
  printf("\n### Script Contents ###\n");
  printf("%s\n", source);

  source = original_source;

  // Lexer
  lexer_t *lexer = lexer_new(source);
  token_t *token;

  printf("\n### Tokenised Output ###\n");
  while ((token = lexer_next_token(lexer))->type != TOKEN_EOF) {
    token_print(*token);
    token_free(token);
  }
  token_print(*token);
  token_free(token);

  // TODO: Call the parser and execution engine (not implemented yet)
  // execute_script(vm, source);

  vm_free(vm);
  free(source);
  return 0;
}
