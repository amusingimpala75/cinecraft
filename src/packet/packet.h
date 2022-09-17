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

#pragma once

#include "memory.h"
#include "network.h"
#include "types.h"

#include <stdlib.h>

#define _TYPE(type)                                                            \
  type packet_read_##type(packet_data *);                                      \
  void packet_write_##type(type, packet_data *)
#define _ALIAS_TYPE(type, name)                                                \
  type packet_read_##name(packet_data *);                                      \
  void packet_write_##name(type, packet_data *)

typedef struct packet_data {
  u8 *data;
  size_t size;
  u8 *current_pos;
} packet_data;

_TYPE(bool);

_TYPE(u8);
_TYPE(u16);

_TYPE(i8);
_TYPE(i16);
_TYPE(i32);
_TYPE(i64);

_TYPE(f32);
_TYPE(f64);
// todo:chat, identifier
char *packet_read_string(packet_data *);
void packet_write_string(char *, size_t length, packet_data *);
_ALIAS_TYPE(i32, var_int);
_ALIAS_TYPE(i64, var_long);
// todo: entity metadata
// todo: slot
// todo: nbt
// todo: position
// todo: angle
// todo: uuid
// todo: opt, array, enum
#define PACKET_READ_ENUM(type, size_type, data)                                \
  (type) packet_read_##size_type(data);
// todo: byte array

#undef _TYPE

struct s2c_packet;

typedef void (*packet_sender)(struct s2c_packet *, packet_data *);

struct s2c_packet {
  packet_sender to_packet_data;
  size_t size;
  i32 id;
};

#define DECL_S2C_PACKET(name, fields)                                          \
  typedef struct s2c_##name {                                                  \
    packet_sender to_packet_data;                                              \
    size_t size;                                                               \
    i32 id;                                                                    \
    fields                                                                     \
  } s2c_##name;                                                                \
  void s2c_##name##_to_packet_data(s2c_##name *, packet_data *);               \
  extern int s2c_##name##_id;                                                  \
  inline void create_s2c_##name(s2c_##name *packet) {                          \
    packet->id = s2c_##name##_id;                                              \
    packet->to_packet_data = (packet_sender)&s2c_##name##_to_packet_data;      \
  }

#define REGISTER_S2C_PACKET(group, name) \
  name##_id = group##_packets.current_sender_id++;

void send_packet_data(struct s2c_packet *, i64);

#define SEND_PACKET(packet, client)                                            \
  send_packet_data((struct s2c_packet *)&(packet), client)

#define DECL_PACKET_HANDLER(group, handler)                                     \
  do {                                                                          \
    if (group##_packets.max_packet_id < group##_packets.current_packet_id + 1) {\
      ERROR("tried to register packet id to group" #group "but the max packet id was too low");                                                                  \
    } else {                                                                    \
      group##_packets.handlers[group##_packets.current_packet_id++] = handler;  \
    }                                                                           \
  } while (false)
