#include "../lexer/lexer.h"
#include "../parser/parser.h"
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

  // Print script contents
  printf("\n### Script Contents ###\n");
  printf("%s\n", source);

  source = original_source;

  lexer_t *lexer = lexer_new(source);
  parser_t *parser = parser_new(lexer);

  // Parse the script
  parser->root = parse_root(parser);
  

  // Print the AST
  printf("\n### AST ###\n");
  for (int i = 0; i < parser->root->count; i++) {
    print_ast_tree(parser->root->nodes[i]);
  }

  // TODO: Call the execution engine (not implemented yet)
  
  // Clean up
  lexer_free(lexer);
  parser_free(parser);
  free(source);
  return 0;
}
