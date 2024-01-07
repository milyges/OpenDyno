#ifndef __GPS_H
#define __GPS_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <stdint.h>

struct gps_state {
	uint32_t gps_time;
	uint8_t gps_fix;
	uint8_t gps_sats;
	int32_t gps_speed;
};

void gps_init(void);
//int gps_loop(struct gps_state * dest);
void gps_loop(void);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* __GPS_H */
