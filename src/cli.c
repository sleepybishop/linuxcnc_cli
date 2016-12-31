#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ae/anet.h>
#include <art/art.h>
#include <linenoise/linenoise.h>

#define DELAY 25000

art_tree t;

static void load_autocomp(const char *filename) {
  FILE *f = fopen(filename, "r");
  art_tree_init(&t);

  if (f) {
    char line[256];
    while (fgets(line, sizeof(line), f)) {
      line[strlen(line) - 1] = 0;
      art_insert(&t, line, strlen(line), NULL);
    }
    fclose(f);
  }
}

static int autocomp_cb(void *data, const unsigned char *k, uint32_t k_len,
                       void *val) {
  linenoiseCompletions *lc = (linenoiseCompletions *)data;
  char *m = calloc(1, k_len + 1);
  memcpy(m, k, k_len);
  linenoiseAddCompletion(lc, m);
	free(m);
  return 0;
}

void completion(const char *buf, linenoiseCompletions *lc) {
  if (buf[0]) {
    art_iter_prefix(&t, buf, strlen(buf), autocomp_cb, lc);
  }
}

void send_cmd(int fd, char *cmd, int len) {
  char cmd_buf[4096];
  int cmd_len;

  if (len > 0) {
    cmd_len = snprintf(cmd_buf, sizeof(cmd_buf) - 1, "%.*s\r\n", cmd, len);
  } else {
    cmd_len = snprintf(cmd_buf, sizeof(cmd_buf) - 1, "%s\r\n", cmd);
  }
  anetWrite(fd, cmd_buf, cmd_len);
  usleep(DELAY);
}

int main(int argc, char **argv) {
  char *line, prompt[256], *rdbuf[4096];
  char *prgname = argv[0];
  int cmd_sock, rdlen;
  char anet_err[ANET_ERR_LEN];

  if (argc < 2) {
    fprintf(stderr, "usage: %s <host>\n", prgname);
    exit(1);
  }

  cmd_sock = anetTcpConnect(anet_err, argv[1], 5007);

  if (cmd_sock < 0) {
    fprintf(stderr, "failed to create command socket: %s\n", anet_err);
    exit(1);
  }

  linenoiseSetMultiLine(0);

  snprintf(prompt, sizeof(prompt), "%s> ", argv[1]);

  load_autocomp("autocomp.txt");

  /* Set the completion callback. This will be called every time the
   * user uses the <tab> key. */
  linenoiseSetCompletionCallback(completion);

  /* Load history from file. The history file is just a plain text file
   * where entries are separated by newlines. */
  linenoiseHistoryLoad("history.txt"); /* Load the history at startup */

  /* Now this is the main loop of the typical linenoise-based application.
   * The call to linenoise() will block as long as the user types something
   * and presses enter.
   *
   * The typed string is returned as a malloc() allocated string by
   * linenoise, so the user needs to free() it. */
  while ((line = linenoise(prompt)) != NULL) {
    if (line[0] != '\0' && line[0] != '/') {
      send_cmd(cmd_sock, line, 0);

      linenoiseHistoryAdd(line);           /* Add to the history. */
      linenoiseHistorySave("history.txt"); /* Save the history on disk. */

      rdlen = read(cmd_sock, rdbuf, sizeof(rdbuf) - 1);
      fprintf(stdout, "%.*s", rdlen, rdbuf);
    } else if (!strncmp(line, "/historylen", 11)) {
      /* The "/historylen" command will change the history len. */
      int len = atoi(line + 11);
      linenoiseHistorySetMaxLen(len);
    } else if (line[0] == '/') {
      char macro_file[256];
      snprintf(macro_file, sizeof(macro_file), "./macros/%s", line + 1);
      FILE *mfd = fopen(macro_file, "r");
      char line[256];
      if (mfd) {
        while (fgets(line, sizeof(line), mfd)) {
          line[strlen(line) - 1] = 0;
          send_cmd(cmd_sock, line, 0);
          rdlen = read(cmd_sock, rdbuf, sizeof(rdbuf) - 1);
          fprintf(stdout, "%.*s", rdlen, rdbuf);
        }
        fclose(mfd);
      } else {
        printf("Unreconized command: %s\n", line);
        close(cmd_sock);
        exit(0);
      }
    }
    free(line);
  }
  art_tree_destroy(&t);
  return 0;
}

/*
 *
 * TODO:
 * get list of available programs from $HOME/linuxcnc/nc_files/...
 * add support for get/set temperature: use custom M-CODE or set MDI-COMMAND in
 * inifile
 * return linuxcnc errors on bad commands or system failure
 *
 */
