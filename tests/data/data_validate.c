#include "../tests.h"

int main()
{
  data_t *data = NULL;
  data_t *errors = NULL;
  int ret = 0;
  int i = 0;
  char *test = NULL;

  SIGCATCH_INIT

  CALLOC(errors, 1, sizeof(data_t));

  /* DATA_T_MSG */
  CALLOC(data, 1, sizeof(data_t));
  data->type = DATA_T_MSG;
  data->buf = "\
THIS IS SAMPLE\n\
TWO-LINE TEXT\n";
  data->len = strlen(data->buf);

  ret = data_validate(data, errors, 0);
  assert(ret == 0);
  FREE(data);

  /* DATA_L_FILES */
  CALLOC(data, 1, sizeof(data_t));
  data->type = DATA_L_FILES;
  test =
#include "../samples/test_list_of_files.inc"

  data->len = strlen(test) + 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0';

  ret = data_validate(data, errors, 0);
  assert(ret == 0);
  assert(data->items == 5);
  FREE(data);

  /* DATA_L_UUIDS */
  CALLOC(data, 1, sizeof(data_t));
  data->type = DATA_L_UUIDS;
  test =
#include "../samples/test_list_of_uuids.inc"

  data->len = strlen(test) + 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0';

  ret = data_validate(data, errors, 0);
  assert(ret == 0);
  assert(data->items == 4);
  FREE(data);

  /* DATA_M_UUID_FILE */
  CALLOC(data, 1, sizeof(data_t));
  data->type = DATA_M_UUID_FILE;
  test =
#include "../samples/test_map_of_uuid_and_file.inc"

  data->len = strlen(test) + 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0';

  ret = data_validate(data, errors, 0);
  assert(ret == 0);
  assert(data->items == 6);
  FREE(data);

  /* DATA_M_UUID_TAGS */
  CALLOC(data, 1, sizeof(data_t));
  data->type = DATA_M_UUID_TAGS;
  test =
#include "../samples/test_map_of_uuid_and_tags.inc"

  data->len = strlen(test) + 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0';

  ret = data_validate(data, errors, 0);
  assert(ret == 0);
  assert(data->items == 6);
  FREE(data);

  return 0;
}
