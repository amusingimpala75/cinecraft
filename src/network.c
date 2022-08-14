#include "network.h"
#include "config.h"
#include "log.h"
#include "memory.h"
#include "packet/packet.h"

#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>

const i64 protocol_version = 760;

static int socket_id = 0;

static void *listen_for_clients(void *args) {
  while (true) {
    accept_connection();
  }
  return NULL;
}

static pthread_t network_main_thread;

void initialize_socket(void) {
  socket_id = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;

  memset(&addr, '0', sizeof(addr));

  addr.sin_family = AF_INET;
  addr.sin_port = htons(config.server.port);
  addr.sin_addr.s_addr = htons(INADDR_ANY);

  if (bind(socket_id, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    ERROR("Could not open port %s", config.server.port);
    exit(1);
  }
  listen(socket_id, 10);

  pthread_create(&network_main_thread, NULL, listen_for_clients, NULL);
}

/*
 * Networking setup:
 * Main network thread spins waiting for incoming connections.
 * When a connection is received, a thread is spun off for the individual client,
 * which then waits for further packets sent by them. Closing the connection terminates
 * the thread.
 *
 * */

struct client {
  int connection_id;
  enum client_phase phase;
  bool should_close;
  pthread_t thread;
};

static struct clients {
  int count;
  int max_count;
  struct client **clients;
} clients;

static size_t find_client(int client_id, struct client **write_to) {
  for (size_t i = 0; i < clients.count; i++) {
    if (clients.clients[i]->connection_id == client_id) {
      *write_to = clients.clients[i];
      return i;
    }
  }
  ERROR("could not find client with id %d", client_id);
  return -1;
}

void close_connection(int client_id) {
  struct client *client = NULL;
  size_t index = find_client(client_id, &client);
  if (client == NULL) {
    ERROR("could not find client with id %d", client_id);
    return;
  }
  close(client_id);
  client->should_close = true;
  INFO("closed connection id %d", client->connection_id);
  clients.clients[index] = clients.clients[clients.count - 1];
  clients.clients[clients.count - 1] = NULL;
  clients.count--;
}

const i8 segment_bits = 0x7f;
const i8 continue_bit = 0x80;

void send_packet(int client_id, u8 *data, u64 size) {
  send(client_id, data, sizeof(u8) * (size), 0);
}

void change_client_phase(int client_id, enum client_phase phase) {
  struct client *client = NULL;
  find_client(client_id, &client);
  if (client) {
    INFO("changing client %d from %d to %d", client->connection_id, client->phase, phase);
    client->phase = phase;
  }
}

static i32 read_packet_length(int conn_id) {
  int val = 0;
  int position = 0;
  u8 current_byte;

  while (true) {
    recv(conn_id, &current_byte, sizeof(u8), 0);
    val |= (current_byte & segment_bits);
    if ((current_byte & continue_bit) == 0) break;

    position += 7;

    if (position >= 32) {
      ERROR("malformed packet length");
      break;
    }
  }
  return val;
}

static void *client_network_thread(void *args) {
  struct client *this_client = args;
  while (!this_client->should_close) {

    u64 packet_length = read_packet_length(this_client->connection_id);

    u8 *packet_data = safe_malloc(packet_length * sizeof(u8));
    recv(this_client->connection_id, packet_data, packet_length * sizeof(u8), 0);

    struct packet_data packet;
    packet.data = packet_data;
    packet.size = packet_length * sizeof(u8);
    packet.current_pos = packet_data;

    i32 packet_id = packet_read_var_int(&packet);

   struct packet_phase *phase;
   switch(this_client->phase) {
     case phase_status:
       phase = (struct packet_phase *) &status_packets;
       break;
     case phase_handshake:
       phase = (struct packet_phase *) &handshake_packets;
       break;
     default:
       ERROR("unknown phase: %d", this_client->phase);
       continue;
   };

    if (packet_id > phase->max_packet_id) {
      ERROR("unknown packet id %d with size %d from client %d, skipping", packet_id, packet_length, this_client->connection_id);
      continue;
    }

    packet_handler handler = phase->handlers[packet_id];
    if (!handler) {
      ERROR("packet id %d too high, skipping", packet_id);
      continue;
    }

    INFO("%d received %d bytes (id %d)", this_client->connection_id, packet_length, packet_id);

    handler(this_client->connection_id, &packet);
  }
  free(this_client);
  return NULL;
}

int accept_connection(void) {
  int conn_id = accept(socket_id, NULL, NULL);
  struct client *client = safe_malloc(sizeof(struct client));
  client->connection_id = conn_id;
  client->phase = phase_handshake;
  client->should_close = false;

  if (!clients.clients) {
    clients.max_count = 8;
    clients.count = 0;
    clients.clients = safe_malloc(sizeof(struct client **) * 8);
  } else if (clients.count + 1 > clients.max_count) {
    clients.max_count *= 2;
    clients.clients = safe_realloc(clients.clients, sizeof(struct client **) * clients.max_count);
  }

  clients.clients[clients.count++] = client;

  //spawn pthread for client
  if (pthread_create(&(client->thread), NULL, client_network_thread, client)) {
    ERROR("Could not create client networking thread");
  }
  INFO("started connection %d", conn_id);

  return conn_id;
}

__attribute__((destructor)) static void cleanup(void) {
  for (size_t i = 0; i < clients.count; i++) {
    free(clients.clients[i]);
  }
  free(clients.clients);
}
