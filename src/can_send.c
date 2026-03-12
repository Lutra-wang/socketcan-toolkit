#include "can_common.h"
#include "mock_io.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void usage(const char *p) {
  fprintf(stderr,
    "Usage:\n"
    "  %s --mock_out <out.csv> --id <hex> --data <hexbytes> [--eff] [--rate_ms N] [--count N]\n"
    "Example:\n"
    "  %s --mock_out demo.csv --id 0x123 --data 11223344AABBCCDD --count 5 --rate_ms 10\n",
    p, p);
}

int main(int argc, char **argv) {
  if (argc < 2) { usage(argv[0]); return 2; }

  const char *mock_out = NULL;
  unsigned int id = 0;
  const char *data_hex = NULL;
  int use_eff = 0;
  int rate_ms = 0;
  int count = 1;

  int i = 1;
  if (strcmp(argv[i], "--mock_out") == 0 && i + 1 < argc) {
    mock_out = argv[i + 1];
    i += 2;
  } else {
    usage(argv[0]);
    return 2;
  }

  for (; i < argc; i++) {
    if (strcmp(argv[i], "--id") == 0 && i + 1 < argc) sscanf(argv[++i], "%x", &id);
    else if (strcmp(argv[i], "--data") == 0 && i + 1 < argc) data_hex = argv[++i];
    else if (strcmp(argv[i], "--eff") == 0) use_eff = 1;
    else if (strcmp(argv[i], "--rate_ms") == 0 && i + 1 < argc) rate_ms = atoi(argv[++i]);
    else if (strcmp(argv[i], "--count") == 0 && i + 1 < argc) count = atoi(argv[++i]);
    else { usage(argv[0]); return 2; }
  }
  if (!data_hex) { fprintf(stderr, "--data required\n"); return 2; }

  uint8_t data[8];
  int dlc = hex_to_bytes(data_hex, data, sizeof(data));
  if (dlc < 0 || dlc > 8) { fprintf(stderr, "bad data\n"); return 2; }

  FILE *f = fopen(mock_out, "w");
  if (!f) { perror("fopen"); return 1; }
  csv_write_header(f);

  for (int k = 0; k < count; k++) {
    struct can_frame fr;
    memset(&fr, 0, sizeof(fr));
    fr.can_dlc = (uint8_t)dlc;
    memcpy(fr.data, data, (size_t)dlc);
    fr.can_id = (uint32_t)id;
    if (use_eff) fr.can_id |= CAN_EFF_FLAG;

    uint64_t ts = now_us();
    csv_write_frame(f, ts, &fr);
    print_frame_human(ts, &fr);

    if (rate_ms > 0) usleep((useconds_t)rate_ms * 1000U);
  }

  fclose(f);
  fprintf(stderr, "[OK] wrote %s\n", mock_out);
  return 0;
}