#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <linux/can.h>
#include <linux/can/raw.h>

static inline uint64_t now_us(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (uint64_t)ts.tv_sec * 1000000ULL + (uint64_t)ts.tv_nsec / 1000ULL;
}

static inline void bytes_to_hex(const uint8_t *data, uint8_t dlc, char *out, size_t out_sz) {
  size_t need = (size_t)dlc * 2 + 1;
  if (out_sz < need) { if (out_sz > 0) out[0] = '\0'; return; }
  for (uint8_t i = 0; i < dlc; i++) snprintf(out + (size_t)i * 2, 3, "%02X", data[i]);
  out[dlc * 2] = '\0';
}

static inline int hex_to_bytes(const char *hex, uint8_t *out, size_t out_max) {
  size_t n = strlen(hex);
  if (n % 2 != 0) return -1;
  size_t bytes = n / 2;
  if (bytes > out_max) return -1;
  for (size_t i = 0; i < bytes; i++) {
    unsigned int v = 0;
    if (sscanf(hex + i * 2, "%2x", &v) != 1) return -1;
    out[i] = (uint8_t)v;
  }
  return (int)bytes;
}

static inline void print_frame_human(uint64_t ts_us, const struct can_frame *fr) {
  char data_hex[8 * 2 + 1];
  bytes_to_hex(fr->data, fr->can_dlc, data_hex, sizeof(data_hex));

  int is_eff = (fr->can_id & CAN_EFF_FLAG) != 0;
  uint32_t id = is_eff ? (fr->can_id & CAN_EFF_MASK) : (fr->can_id & CAN_SFF_MASK);

  printf("%llu us  %s  id=0x%X  dlc=%u  data=%s\n",
         (unsigned long long)ts_us,
         is_eff ? "EFF" : "SFF",
         id, fr->can_dlc, data_hex);
}