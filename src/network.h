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

void initialize_socket(void);
int accept_connection(void);
void close_connection(int);
void send_packet(int client_id, u8 *data, u64 size);
void change_client_phase(int, enum client_phase);
