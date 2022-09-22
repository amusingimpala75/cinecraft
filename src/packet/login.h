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

#include "packet/packet.h"

DECL_S2C_PACKET(login_disconnect, char *reason;);
DECL_S2C_PACKET(enc_req,
                struct {
                    char server_id[20];
                    i32 key_length;
                    u8 *key;
                    i32 token_length;
                    u8 *token;
                } fields;
);
DECL_S2C_PACKET(login_success,
                struct {
                    uuid uuid;
                    char username[16];
                    i32 property_count;
                    struct property {
                        char *name;
                        char *value;
                        bool is_signed;
                        char *sig;
                    } properties[];
                } fields;
);
DECL_S2C_PACKET(set_compression, i32 compression;);
DECL_S2C_PACKET(login_plugin_request,
                struct {
                    i32 message_id;
                    char *identifier;
                    u8 *data;
                } fields;
);

void c2s_login_start(int, packet_data *);
void c2s_enc_response(int, packet_data *);
void c2s_plugin_response(int, packet_data *);
