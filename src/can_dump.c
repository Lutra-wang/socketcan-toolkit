#include "can_common.h"
#include "mock_io.h"

#include <stdlib.h>
#include <string.h>

static void usage(const char *p) {
  fprintf(stderr,
    "Usage: %s --mock <in.csv> [--filter <id:mask>] [--log <out.csv>]\n"
    "Example: %s --mock demo.csv --filter 0x123:0x7FF --log out.csv\n", p, p);
}

static int parse_filter(const char *s, unsigned int *id, unsigned int *mask) {
  return (sscanf(s, "%x:%x", id, mask) == 2) ? 0 : -1;
}

static int pass_filter(unsigned int id, unsigned int mask, const struct can_frame *fr, int has) {
  if (!has) return 1;
  return ((fr->can_id & mask) == (id & mask));
}

int main(int argc, char **argv) {
  const char *in = NULL, *log = NULL, *fstr = NULL;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--mock") == 0 && i + 1 < argc) in = argv[++i];
    else if (strcmp(argv[i], "--log") == 0 && i + 1 < argc) log = argv[++i];
    else if (strcmp(argv[i], "--filter") == 0 && i + 1 < argc) fstr = argv[++i];
    else { usage(argv[0]); return 2; }
  }
  if (!in) { usage(argv[0]); return 2; }

  unsigned int fid = 0, fmask = 0;
  int has_filter = 0;
  if (fstr) {
    if (parse_filter(fstr, &fid, &fmask) != 0) { fprintf(stderr, "bad filter\n"); return 2; }
    has_filter = 1;
  }

  FILE *fin = fopen(in, "r");
  if (!fin) { perror("fopen in"); return 1; }

  FILE *fout = NULL;
  if (log) {
    fout = fopen(log, "w");
    if (!fout) { perror("fopen log"); fclose(fin); return 1; }
    csv_write_header(fout);
  }

  fprintf(stderr, "[INFO] MOCK dump from %s%s\n", in, has_filter ? " (filtered)" : "");

  uint64_t ts = 0;
  struct can_frame fr;
  while (csv_read_frame(fin, &ts, &fr)) {
    if (!pass_filter(fid, fmask, &fr, has_filter)) continue;
    print_frame_human(ts, &fr);
    if (fout) { csv_write_frame(fout, ts, &fr); }
  }

  fclose(fin);
  if (fout) fclose(fout);
  return 0;
}