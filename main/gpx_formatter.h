#ifndef GPX_FORMATTER_H
#define GPX_FORMATTER_H

#include <stddef.h>
#include "telemetry.h"

/**
 * @brief Format a single telemetry snapshot into a GPX <trkpt>.
 * 
 * @param buffer Output buffer.
 * @param size Buffer size.
 * @param snapshot Telemetry snapshot.
 * @return size_t Length of the formatted string.
 */
size_t gpx_format_point(char *buffer, size_t size, const global_telemetry_t *snapshot);

/**
 * @brief Format a single telemetry snapshot into a GPX <wpt>.
 * 
 * @param buffer Output buffer.
 * @param size Buffer size.
 * @param snapshot Telemetry snapshot.
 * @return size_t Length of the formatted string.
 */
size_t gpx_format_waypoint(char *buffer, size_t size, const global_telemetry_t *snapshot);

/**
 * @brief Return the GPX header string.
 */
const char* gpx_get_header(void);

/**
 * @brief Return the GPX footer string.
 */
const char* gpx_get_footer(void);

#endif // GPX_FORMATTER_H
