#include "../tests.h"

int main()
{
  list_t *data = NULL;
  list_t *errors = NULL;
  int ret = 0;
  int i = 0;
  char *test = NULL;

  SIGCATCH_INIT

  CALLOC(errors, 1, sizeof(list_t));

  /* LIST_T_MSG */
  CALLOC(data, 1, sizeof(list_t));
  data->type = LIST_T_MSG;
  test = "\
THIS IS SAMPLE\n\
TWO-LINE TEXT\n";
  STRNDUP(data->buf, test, 4096);
  data->len = strlen(data->buf) + 1;
  data->items = 2;

  ret = list_validate(data, errors, 0);
  assert(ret == 0);
  FREE(data);

  /* LIST_L_FILES */
  CALLOC(data, 1, sizeof(list_t));
  data->type = LIST_L_FILES;
  test =
#include "../samples/test_list_of_files.inc"

  data->len = strlen(test) + 1;
  data->items = 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0', data->items++;

  ret = list_validate(data, errors, 0);
  assert(ret == 0);
  assert(data->items == 5);
  FREE(data);

  /* LIST_L_UUIDS */
  CALLOC(data, 1, sizeof(list_t));
  data->type = LIST_L_UUIDS;
  test =
#include "../samples/test_list_of_uuids.inc"

  data->len = strlen(test) + 1;
  data->items = 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0', data->items++;

  ret = list_validate(data, errors, 0);
  assert(ret == 0);
  assert(data->items == 4);
  FREE(data);

  /* LIST_M_UUID_FILE */
  CALLOC(data, 1, sizeof(list_t));
  data->type = LIST_M_UUID_FILE;
  test =
#include "../samples/test_map_of_uuid_and_file.inc"

  data->len = strlen(test) + 1;
  data->items = 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0', data->items++;

  ret = list_validate(data, errors, 0);
  assert(ret == 0);
  assert(data->items == 6);
  FREE(data);

  /* LIST_M_UUID_TAGS */
  CALLOC(data, 1, sizeof(list_t));
  data->type = LIST_M_UUID_TAGS;
  test =
#include "../samples/test_map_of_uuid_and_tags.inc"

  data->len = strlen(test) + 1;
  data->items = 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0', data->items++;

  ret = list_validate(data, errors, 0);
  assert(ret == 0);
  assert(data->items == 6);
  FREE(data);

  return 0;
}
