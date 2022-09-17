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

#include "packet/C2SHandshake.h"

#include "network.h"
#include "packet/packet.h"
#include "log.h"

struct handshake_packets handshake_packets = {.current_packet_id = 0, .max_packet_id = HANDSHAKE_PACKET_MAX_ID};

__attribute__((constructor)) void init(void) {
    DECL_PACKET_HANDLER(handshake, &c2s_handshake);
}

void c2s_handshake(int client, packet_data *packet) {
    i32 requested_protocol_version = packet_read_var_int(packet);
    char *server_addr = packet_read_string(packet);
    u16 port = packet_read_u16(packet);
    i32 next_state = packet_read_var_int(packet);

    if (requested_protocol_version != protocol_version) {
        ERROR("requested protocol %d, but using protocol %d", requested_protocol_version, protocol_version);
        close_connection(client);
    }

    INFO("connecting with addr %s and port %d", server_addr, port);

    enum client_phase phase;

    switch(next_state) {
        case 1: phase = phase_status; break;
        case 2: phase = phase_login; break;
        default: ERROR("unknown phase: %d", next_state); return;
    }

    change_client_phase(client, phase);
}
