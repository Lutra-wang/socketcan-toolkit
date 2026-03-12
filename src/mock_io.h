#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <linux/can.h>
#include "can_common.h"

// CSV: ts_us,id,is_eff,dlc,data_hex
// 12345,0x123,0,8,11223344AABBCCDD

static inline int csv_read_frame(FILE *f, uint64_t *ts_us_out, struct can_frame *fr_out) {
  char line[256];
  while (fgets(line, sizeof(line), f)) {
    if (line[0] == '\n' || line[0] == '\r') continue;
    if (strncmp(line, "ts_us", 5) == 0) continue;

    unsigned long long ts_us = 0;
    unsigned int id = 0;
    int is_eff = 0;
    unsigned int dlc = 0;
    char data_hex[16 + 1] = {0};

    int n = sscanf(line, "%llu,0x%x,%d,%u,%16s", &ts_us, &id, &is_eff, &dlc, data_hex);
    if (n < 5) continue;

    uint8_t data[8] = {0};
    int got = hex_to_bytes(data_hex, data, sizeof(data));
    if (got < 0) continue;

    struct can_frame fr;
    memset(&fr, 0, sizeof(fr));
    fr.can_dlc = (uint8_t)((dlc > 8) ? 8 : dlc);
    memcpy(fr.data, data, fr.can_dlc);
    fr.can_id = (uint32_t)id;
    if (is_eff) fr.can_id |= CAN_EFF_FLAG;

    *ts_us_out = (uint64_t)ts_us;
    *fr_out = fr;
    return 1;
  }
  return 0;
}

static inline void csv_write_header(FILE *f) {
  fprintf(f, "ts_us,id,is_eff,dlc,data_hex\n");
}

static inline void csv_write_frame(FILE *f, uint64_t ts_us, const struct can_frame *fr) {
  int is_eff = (fr->can_id & CAN_EFF_FLAG) != 0;
  uint32_t id = is_eff ? (fr->can_id & CAN_EFF_MASK) : (fr->can_id & CAN_SFF_MASK);

  char data_hex[8 * 2 + 1];
  bytes_to_hex(fr->data, fr->can_dlc, data_hex, sizeof(data_hex));
  fprintf(f, "%llu,0x%X,%d,%u,%s\n",
          (unsigned long long)ts_us, id, is_eff, fr->can_dlc, data_hex);
}