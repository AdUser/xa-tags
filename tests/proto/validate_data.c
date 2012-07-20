#include "../tests.h"

int main()
{
  data_t *data = NULL;
  int ret = 0;
  int i = 0;
  char *test = NULL;

  SIGCATCH_INIT

  /* DATA_T_MSG */
  CALLOC(data, 1, sizeof(data_t));
  data->type = DATA_T_MSG;
  data->buf = "\
THIS IS SAMPLE\n\
TWO-LINE TEXT\n";
  data->len = strlen(data->buf);

  ret = validate_data(data);
  assert(ret == 0);
  FREE(data);
  
  /* DATA_L_FILES */
  CALLOC(data, 1, sizeof(data_t));
  data->type = DATA_L_FILES;
  test = \
  "/home/user/test / file.png\n" /* pass */ \
  "home/user/test/file.png\n"    /* fail */ \
  "~/test/file.png\n"            /* pass */ \
  "file.png\n"                   /* fail */ \
  "  ~/test/file.png  \n"        /* pass */ \
  "~/test/\n"                    /* pass */ \
  "~/test/file.png"              /* pass */ ;
  data->len = strlen(test) + 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0';

  ret = validate_data(data);
  assert(ret == 0);
  assert(data->items == 5);
  FREE(data);

  /* DATA_L_UUIDS */
  CALLOC(data, 1, sizeof(data_t));
  data->type = DATA_L_UUIDS;
  test = \
  "1BF050-7490EB-AE6BEB\n"    /* pass */ \
  "FC56B55198E28-6EDAC8\n"    /* fail */ \
  "20A97C-K78B2E-ECCD58\n"    /* fail */ \
  "\t17E6B0-93DC11-99EB8E\n"  /* fail */ \
  "  52AB14-5BD3A2-2C6F5E\n"  /* fail */ \
  "ADC68D-A6D0CB-A54CC\n"     /* fail */ \
  "141899-A504B5-31A714\t\n"  /* pass */ \
  "857CE0-21C51A-B209AD  \n"  /* pass */ \
  "52AB14C-9304F-69340D\n"    /* fail */ \
  "AAD452-1FE824-0B8EE4"      /* pass */ ;

  data->len = strlen(test) + 1;
  STRNDUP(data->buf, test, 4096);
  for (i = 0; i < data->len; i++)
    if (data->buf[i] == '\n')
      data->buf[i] = '\0';

  ret = validate_data(data);
  assert(ret == 0);
  assert(data->items == 4);
  FREE(data);

  return 0;
}
