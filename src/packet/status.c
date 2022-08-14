#include "packet/status.h"
#include "config.h"
#include "io.h"
#include "log.h"
#include "packet/packet.h"

#include <b64/cencode.h>
#include <string.h>

struct status_packets status_packets = {.current_sender_id = 0, .current_packet_id = 0, .max_packet_id = STATUS_PACKET_MAX_ID};

int s2c_status_id;
int s2c_ping_id;

__attribute__((constructor)) static void init(void) {
  DECL_PACKET_HANDLER(status, &c2s_status);
  DECL_PACKET_HANDLER(status, &c2s_ping);
  REGISTER_S2C_PACKET(status, s2c_status);
  REGISTER_S2C_PACKET(status, s2c_ping);
}

void c2s_status(int client, packet_data *data) {
  s2c_status ret;
  create_s2c_status(&ret);
  SEND_PACKET(ret, client);
}

void c2s_ping(int client, packet_data *data) {
  i64 val = packet_read_i64(data);
  s2c_ping ret;
  create_s2c_ping(&ret);
  ret.val = val;
  SEND_PACKET(ret, client);
  close_connection(client);
}

static const char *version_name = "1.19.1";

static char *encoded_icon = NULL;

__attribute__((destructor)) static void cleanup(void) { free(encoded_icon); }

static char *get_encoded_icon(void) {
  if (!encoded_icon) {
    FILE **file = open_file(config.preview.icon, "r");
    if (!file) {
      exit(1);
    }
    fseek(*file, 0, SEEK_END);
    size_t size = ftell(*file);
    fseek(*file, 0, SEEK_SET);
    char *input = safe_malloc(size + 1);
    char *encoded = safe_malloc(2 * size + 1);
    char *write_to = encoded;
    base64_encodestate es;
    int count;
    base64_init_encodestate(&es);
    while (true) {
      count = fread(input, sizeof(char), size, *file);
      if (count == 0)
        break;
      count = base64_encode_block(input, count, write_to, &es);
      write_to += count;
    }
    count = base64_encode_blockend(write_to, &es);
    encoded_icon = encoded;
    free(input);
    close_file(file);

    u64 index = 0;
    u64 newline_count = 0;
    while (encoded[index + newline_count] != '\0') {
      if (encoded[index+newline_count] == '\n') {
        newline_count++;
      }
      encoded[index] = encoded[index + newline_count];
      index++;
    }
    encoded[index] = '\0';
  }
  return encoded_icon;
}

static const char *status_fmt =
    "{                                               \n"
    "    \"version\": {                              \n"
    "        \"name\": \"%s\",                       \n"
    "        \"protocol\": %d                        \n"
    "    },                                          \n"
    "    \"players\": {                              \n"
    "        \"max\": %d,                            \n"
    "        \"online\": %d,                         \n"
    "        \"sample\": []                          \n"
    "    },                                          \n"
    "    \"description\": {                          \n"
    "        \"text\": \"%s\"                        \n"
    "    },                                          \n"
    "    \"favicon\": \"data:image/png;base64,%s\",  \n"
    "    \"previewsChat\": %s                        \n"
    "}                                                 ";

void s2c_status_to_packet_data(s2c_status *info, packet_data *data) {
  char *formatted = NULL;
  int player_current = 0; // todo: fix
  if (!asprintf(&formatted, status_fmt, version_name, protocol_version,
                config.max_player_count, player_current, config.preview.motd,
                get_encoded_icon(), config.chat_preview ? "true" : "false")) {
    ERROR("could not serialize status packet to client");
    return;
  }
  packet_write_string(formatted, strlen(formatted), data);
  free(formatted);
}

void s2c_ping_to_packet_data(s2c_ping *ping, packet_data *data) {
  packet_write_i64(ping->val, data);
}
