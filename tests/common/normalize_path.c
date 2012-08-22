#include "../tests.h"

int main()
{
  char *out = NULL;

  SIGCATCH_INIT

  out = normalize_path("~///very/long/../../and/../buggy//../path/./to/some/././file");
  assert(out != NULL);
  assert(strcmp(out, "~/path/to/some/file") == 0);
  FREE(out);

  out = normalize_path("/absolute/very///long/../../and/../buggy//../path/./to/some/././file");
  assert(out != NULL);
  assert(strcmp(out, "/absolute/path/to/some/file") == 0);
  FREE(out);

  return 0;
}
