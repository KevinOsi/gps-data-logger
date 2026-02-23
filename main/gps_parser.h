#ifndef GPS_PARSER_H
#define GPS_PARSER_H

#include <stdint.h>
#include <stdbool.h>
#include "telemetry.h"

/**
 * @brief Initialize the GPS parser state machine.
 */
void gps_parser_init();

/**
 * @brief Parse a single byte of GPS data.
 * 
 * @param c The byte to parse.
 * @param pvt Pointer to the structure where parsed PVT data will be stored.
 * @return true if a complete and valid UBX-NAV-PVT packet has been parsed.
 * @return false otherwise.
 */
bool gps_parse_byte(uint8_t c, ubx_nav_pvt_t *pvt);

#endif // GPS_PARSER_H
