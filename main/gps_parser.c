#include "gps_parser.h"
#include <string.h>

#define UBX_SYNC1 0xB5
#define UBX_SYNC2 0x62
#define UBX_CLASS_NAV 0x01
#define UBX_ID_PVT 0x07
#define UBX_PAYLOAD_SIZE 92

typedef enum {
    STATE_IDLE,
    STATE_SYNC1,
    STATE_SYNC2,
    STATE_CLASS,
    STATE_ID,
    STATE_LEN1,
    STATE_LEN2,
    STATE_PAYLOAD,
    STATE_CK_A,
    STATE_CK_B
} gps_state_t;

static gps_state_t state = STATE_IDLE;
static uint16_t payload_idx = 0;
static uint16_t expected_len = 0;
static uint8_t ck_a = 0, ck_b = 0;
static uint8_t calc_ck_a = 0, calc_ck_b = 0;

void gps_parser_init() {
    state = STATE_IDLE;
    payload_idx = 0;
    expected_len = 0;
    calc_ck_a = 0;
    calc_ck_b = 0;
}

static void update_checksum(uint8_t c) {
    calc_ck_a += c;
    calc_ck_b += calc_ck_a;
}

bool gps_parse_byte(uint8_t c, ubx_nav_pvt_t *pvt) {
    bool pvt_ready = false;

    switch (state) {
        case STATE_IDLE:
            if (c == UBX_SYNC1) state = STATE_SYNC1;
            break;
        case STATE_SYNC1:
            if (c == UBX_SYNC2) {
                state = STATE_SYNC2;
                calc_ck_a = 0;
                calc_ck_b = 0;
            } else if (c != UBX_SYNC1) {
                state = STATE_IDLE;
            }
            break;
        case STATE_SYNC2:
            if (c == UBX_CLASS_NAV) {
                state = STATE_CLASS;
                update_checksum(c);
            } else {
                state = STATE_IDLE;
            }
            break;
        case STATE_CLASS:
            if (c == UBX_ID_PVT) {
                state = STATE_ID;
                update_checksum(c);
            } else {
                state = STATE_IDLE;
            }
            break;
        case STATE_ID:
            expected_len = c;
            update_checksum(c);
            state = STATE_LEN1;
            break;
        case STATE_LEN1:
            expected_len |= (c << 8);
            update_checksum(c);
            if (expected_len == UBX_PAYLOAD_SIZE) {
                state = STATE_PAYLOAD;
                payload_idx = 0;
            } else {
                state = STATE_IDLE;
            }
            break;
        case STATE_PAYLOAD:
            if (payload_idx < UBX_PAYLOAD_SIZE) {
                ((uint8_t*)pvt)[payload_idx++] = c;
                update_checksum(c);
            }
            if (payload_idx == UBX_PAYLOAD_SIZE) {
                state = STATE_CK_A;
            }
            break;
        case STATE_CK_A:
            ck_a = c;
            state = STATE_CK_B;
            break;
        case STATE_CK_B:
            ck_b = c;
            if (ck_a == calc_ck_a && ck_b == calc_ck_b) {
                pvt_ready = true;
            }
            state = STATE_IDLE;
            break;
        default:
            state = STATE_IDLE;
            break;
    }

    return pvt_ready;
}
