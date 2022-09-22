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

#include "config.h"
#include "log.h"
#include "memory.h"
#include "types.h"
#include "packet/login.h"
#include "packet/packet.h"
#include "network.h"

#include <stdlib.h>
#include <string.h>

#include <unistd.h>

struct login_packets login_packets = {.current_sender_id = 0, .current_packet_id = 0, .max_packet_id = LOGIN_PACKET_MAX_ID};

int s2c_login_disconnect_id;
int s2c_enc_req_id;
int s2c_login_success_id;
int s2c_set_compression_id;
int s2c_plugin_request_id;

__attribute__((constructor)) static void init(void) {
  REGISTER_S2C_PACKET(login, s2c_login_disconnect);
  REGISTER_S2C_PACKET(login, s2c_enc_req);
  REGISTER_S2C_PACKET(login, s2c_login_success);
  REGISTER_S2C_PACKET(login, s2c_set_compression);
  REGISTER_S2C_PACKET(login, s2c_plugin_request);
  DECL_PACKET_HANDLER(login, &c2s_login_start);
  DECL_PACKET_HANDLER(login, &c2s_enc_response);
  DECL_PACKET_HANDLER(login, &c2s_plugin_response);
}

void c2s_login_start(int client, packet_data *packet) {
  char *username = packet_read_string(packet);
  bool has_sig_data = packet_read_bool(packet);
  u8 *sig;
  u8 *key;
  if (has_sig_data) {
    i64 timestamp __attribute__((unused)) = packet_read_i64(packet);
    i32 key_length = packet_read_var_int(packet);
    key = safe_malloc(key_length);
    packet_read_byte_array(packet, key_length, key);
    i32 sig_length = packet_read_var_int(packet);
    sig = safe_malloc(sig_length);
    packet_read_byte_array(packet, sig_length, sig);
  }
  bool has_uuid = packet_read_bool(packet);
  uuid uuid = {.upper = 1, .lower = 1};
  if (has_uuid) {
    uuid = packet_read_uuid(packet);
  }

  if (config.auth) {
    //TODO authentication
  } else {
    s2c_login_success ret;
    create_s2c_login_success(&ret);
    ret.fields.uuid = uuid;
    if (strlen(username) > 15) {
      memcpy(ret.fields.username, username, 15);
      ret.fields.username[15] = '\0';
    } else {
      strcpy(ret.fields.username, username);
    }
    ret.fields.property_count = 0;
    //TODO properties?
    //SEND_PACKET(ret, client);

    //TODO switch to play networking

    s2c_login_disconnect disc;
    create_s2c_login_disconnect(&disc);
    disc.reason = "{ \"text\": \"TODO not implemented yet; come back later\" }";
    SEND_PACKET(disc, client);
    close_connection(client);
  }

  if (has_sig_data) {
    free(key);
    free(sig);
  }
}

void c2s_enc_response(int client, packet_data *packet) {
  i32 secret_length = packet_read_var_int(packet);
  u8 *secret = safe_malloc(secret_length);
  packet_read_byte_array(packet, secret_length, secret);
  bool has_verify_token = packet_read_bool(packet);
  union {
    struct {
      i32 token_length;
      u8 *token;
    } t;
    struct {
      i64 salt;
      i32 sig_length;
      u8 *sig;
    } f;
  } u;
  if (has_verify_token) {
    u.t.token_length = packet_read_var_int(packet);
    u.t.token = safe_malloc(u.t.token_length);
    packet_read_byte_array(packet, u.t.token_length, u.t.token);
  } else {
    u.f.salt = packet_read_i64(packet);
    u.f.sig_length = packet_read_var_int(packet);
    u.f.sig = safe_malloc(u.f.sig_length);
    packet_read_byte_array(packet, u.f.sig_length, u.f.sig);
  }

  //TODO

  free(secret);
  if (has_verify_token) {
    free(u.t.token);
  } else {
    free(u.f.sig);
  }
}

void c2s_plugin_response(int client, packet_data *packet) {
  i32 message_id __attribute__((unused)) = packet_read_var_int(packet);
  bool success __attribute__((unused)) = packet_read_bool(packet);
  u64 remaining_length = packet->size - (packet->current_pos - packet->data);
  u8 *data = safe_malloc(remaining_length);
  packet_read_byte_array(packet, remaining_length, data);

  //TODO

  free(data);
}

void s2c_login_disconnect_to_packet_data(s2c_login_disconnect *disc, packet_data *packet) {
  packet_write_string(disc->reason, strlen(disc->reason), packet);
}

void s2c_login_success_to_packet_data(s2c_login_success *success, packet_data *packet) {
  packet_write_uuid(success->fields.uuid, packet);
  packet_write_string(success->fields.username, strlen(success->fields.username), packet);
  packet_write_var_int(success->fields.property_count, packet);
  for (u64 i = 0; i < success->fields.property_count; i++) {
    struct property *prop = &(success->fields.properties[i]);
    packet_write_string(prop->name, strlen(prop->name), packet);
    packet_write_string(prop->value, strlen(prop->value), packet);
    packet_write_bool(prop->is_signed, packet);
    if (prop->is_signed) {
      packet_write_string(prop->sig, strlen(prop->sig), packet);
    }
  }
}
