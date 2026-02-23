#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "../main/telemetry.h"

int main() {
    printf("Testing telemetry structures...\n");
    
    // Check size of ubx_nav_pvt_t payload (should be 92 bytes)
    printf("Size of ubx_nav_pvt_t: %zu\n", sizeof(ubx_nav_pvt_t));
    assert(sizeof(ubx_nav_pvt_t) == 92);
    
    // Check some members
    ubx_nav_pvt_t pvt = {0};
    pvt.year = 2025;
    assert(pvt.year == 2025);
    
    printf("Telemetry tests passed!\n");
    return 0;
}
