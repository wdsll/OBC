#include "can_service.h"

#include "app_config.h"

bool can_service_init(void)
{
#if APP_CAN_ENABLE
    /*
     * Add the reviewed CAN0 timing and acceptance filters here after the
     * vehicle/host protocol is supplied.
     */
    return false;
#else
    return false;
#endif
}

void can_service_poll(void)
{
    /* No communication side effects are allowed in the initial release. */
}
