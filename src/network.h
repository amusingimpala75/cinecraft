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

#include "types.h"

extern const i64 protocol_version;

struct packet_data;

enum client_phase { phase_handshake, phase_status, phase_login, phase_play };

typedef void (*packet_handler)(int, struct packet_data *);

struct packet_phase {
  i16 current_packet_id;
  const i16 max_packet_id;
  i16 current_sender_id;
  packet_handler handlers[];
};

#define STATUS_PACKET_MAX_ID 2
extern struct status_packets {
  i16 current_packet_id;
  const i16 max_packet_id;
  i16 current_sender_id;
  packet_handler handlers[STATUS_PACKET_MAX_ID];
} status_packets;

#define HANDSHAKE_PACKET_MAX_ID 1
extern struct handshake_packets {
  i16 current_packet_id;
  const i16 max_packet_id;
  i16 current_sender_id;
  packet_handler handlers[HANDSHAKE_PACKET_MAX_ID];
} handshake_packets;
#define LOGIN_PACKET_MAX_ID 5
extern struct login_packets {
  i16 current_packet_id;
  const i16 max_packet_id;
  i16 current_sender_id;
  packet_handler handlers[LOGIN_PACKET_MAX_ID];
} login_packets;

void initialize_socket(void);
int accept_connection(void);
void close_connection(int);
void send_packet(int client_id, u8 *data, u64 size);
void change_client_phase(int, enum client_phase);
