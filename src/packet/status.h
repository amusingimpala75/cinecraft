#pragma once

#include "packet/packet.h"

DECL_S2C_PACKET(ping, i32 val;);
DECL_S2C_PACKET(status, );

void c2s_status(int, packet_data *);
void c2s_ping(int, packet_data *);
