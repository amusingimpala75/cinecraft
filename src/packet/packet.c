/*
    Cinecraft - voxel game server written in C
    Copyright (C) 2022  AmusingImpala75

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "packet/packet.h"
#include "config.h"
#include "io.h"
#include "log.h"
#include "memory.h"
#include "system.h"

#include <b64/cencode.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void send_packet_data(struct s2c_packet *packet, i64 client_id) {
  packet_data data;
  data.data = safe_malloc(sizeof(u8));
  data.current_pos = data.data + 1;
  data.size = 1;
  packet_write_var_int(packet->id, &data);
  packet->to_packet_data(packet, &data);
  packet_data full;
  full.data = safe_malloc(sizeof(u8));
  full.current_pos = full.data + 1;
  full.size = 1;
  packet_write_var_int(data.size - 1, &full);
  data.current_pos = data.data + 1;
  while (data.data + data.size > data.current_pos) {
    packet_write_u8(packet_read_u8(&data), &full);
  }
  free(data.data);
  send_packet(client_id, full.data + 1, full.size - 1);
  INFO("sent %d bytes (id %d) to %d", full.size - 1, packet->id, client_id);
  free(full.data);
}

static bool space(packet_data *data, size_t type) {
  return data->current_pos - data->data + type <= data->size;
}

#define CHECK_SPACE(size)                                                      \
  do {                                                                         \
    if (!space(data, size)) {                                                  \
      ERROR("cannot read " #size " bytes because packet is at end");           \
    }                                                                          \
  } while (false)

#define REALLOC_DATA(type_size)                                                \
  do {                                                                         \
    u64 dist = data->current_pos - data->data;                                 \
    data->data = safe_realloc(data->data, data->size + (type_size));           \
    data->current_pos = data->data + dist;                                     \
    data->size += type_size;                                                   \
  } while (false)

bool packet_read_bool(packet_data *data) {
  CHECK_SPACE(sizeof(bool));
  return (bool)*(data->current_pos++);
}

void packet_write_bool(bool val, packet_data *data) {
  REALLOC_DATA(sizeof(bool));
  *(data->current_pos++) = val;
}

u8 packet_read_u8(packet_data *data) {
  CHECK_SPACE(sizeof(u8));
  return *(data->current_pos++);
}

void packet_write_u8(u8 val, packet_data *data) {
  REALLOC_DATA(sizeof(u8));
  *(data->current_pos++) = val;
}

u16 packet_read_u16(packet_data *data) {
  CHECK_SPACE(sizeof(u16));
  u8 *vals = data->current_pos;
  data->current_pos += 2;
  return (u16)*vals;
}

void packet_write_u16(u16 val, packet_data *data) {
  REALLOC_DATA(sizeof(u16));
  *(data->current_pos) = val;
  data->current_pos += 2;
}

f32 packet_read_f32(packet_data *data) {
  CHECK_SPACE(sizeof(f32));
  u8 *vals = data->current_pos;
  data->current_pos += sizeof(f32) / sizeof(u8);
  return (f32)*vals;
}

void packet_write_f32(f32 val, packet_data *data) {
  REALLOC_DATA(sizeof(f32));
  *(data->current_pos) = val;
  data->current_pos += sizeof(f32) / sizeof(u8);
}

f64 packet_read_f64(packet_data *data) {
  CHECK_SPACE(sizeof(f64));
  u8 *vals = data->current_pos;
  data->current_pos += sizeof(f64) / sizeof(u8);
  return (f64)*vals;
}

void packet_write_f64(f64 val, packet_data *data) {
  REALLOC_DATA(sizeof(f64));
  *(data->current_pos) = val;
  data->current_pos += sizeof(f64) / sizeof(f32);
}

/* varint/varlong ripped off of wiki.vg */

const static int SEGMENT_BITS = 0x7F;
const static int CONTINUE_BIT = 0x80;

i32 packet_read_var_int(packet_data *data) {
  int value = 0;
  int position = 0;
  u8 currentByte;

  while (true) {
    currentByte = packet_read_u8(data);
    value |= (currentByte & SEGMENT_BITS) << position;

    if ((currentByte & CONTINUE_BIT) == 0)
      break;
    position += 7;

    if (position >= 32)
      ERROR("var_int was too long");
  }
  return value;
}

void write_byte(u8 val, packet_data *data) {
  REALLOC_DATA(sizeof(u8));
  *(data->current_pos++) = val;
}

void packet_write_var_int(i32 val, packet_data *data) {
  while (true) {
    if ((val & ~SEGMENT_BITS) == 0) {
      write_byte(val, data);
      return;
    }

    write_byte((val & SEGMENT_BITS) | CONTINUE_BIT, data);

    // Note: >>> means that the sign bit is shifted with the rest of the number
    // rather than being left alone
    val = (i32)(((u32)val) >> 7);
  }
}

i64 packet_read_var_long(packet_data *data) {
  long value = 0;
  int position = 0;
  u8 currentByte;

  while (true) {
    currentByte = packet_read_u8(data);
    value |= (long)(currentByte & SEGMENT_BITS) << position;

    if ((currentByte & CONTINUE_BIT) == 0)
      break;

    position += 7;

    if (position >= 64)
      ERROR("var_long was to long");
  }

  return value;
}

void packet_write_var_long(i64 val, packet_data *data) {
  while (true) {
    if ((val & ~((long)SEGMENT_BITS)) == 0) {
      write_byte(val, data);
      return;
    }

    write_byte((val & SEGMENT_BITS) | CONTINUE_BIT, data);

    // Note: >>> means that the sign bit is shifted with the rest of the number
    // rather than being left alone
    val = (i64)(((u64)val) >> 7);
  }
}

/* end of ripped off of wiki.vg code */

char *packet_read_string(packet_data *data) {
  i32 length = packet_read_var_int(data);
  char *val = safe_malloc(((u64)length) + 1);
  for (u64 i = 0; i < length; i++) {
    u32 character = packet_read_u8(data);
    val[i] = character;
  }
  val[length] = '\0';
  return val;
}

void packet_write_string(char *val, size_t size, packet_data *data) {
  if (size > 32767) {
    ERROR("string too large (%d bytes)", size);
  }
  packet_write_var_int((i32)size, data);
  REALLOC_DATA(size);
  memcpy(data->current_pos, val, size);
  data->current_pos += size;
}

void packet_write_i64(i64 val, packet_data *data) {
  REALLOC_DATA(sizeof(i64));
  memcpy(data->current_pos, &val, sizeof(i64));
  data->current_pos += sizeof(i64);
}

i64 packet_read_i64(packet_data *data) {
  i64 val;
  CHECK_SPACE(sizeof(i64));
  memcpy(&val, data->current_pos, sizeof(i64));
  return val;
}

void packet_write_byte_array(packet_data *packet, u64 count, u8 *data) {
  for (u64 i = 0; i < count; i++) {
    packet_write_u8(data[i], packet);
  }
}

void packet_read_byte_array(packet_data *packet, u64 count, u8 *write_to) {
  for (u64 i = 0; i < count; i++) {
    write_to[i] = packet_read_u8(packet);
  }
}

void packet_write_uuid(uuid uuid, packet_data *packet) {
  packet_write_i64(uuid.lower, packet);
  packet_write_i64(uuid.upper, packet);
}

uuid packet_read_uuid(packet_data *packet) {
  uuid uuid = {.lower = packet_read_i64(packet), .upper = packet_read_i64(packet)};
  return uuid;
}
