#ifndef CAN_SERVICE_H
#define CAN_SERVICE_H

#include <stdbool.h>

/*
 * The schematic defines the CAN pins and transceiver but not the bitrate or
 * message map. The service therefore remains disabled in the initial release.
 */
bool can_service_init(void);
void can_service_poll(void);

#endif
