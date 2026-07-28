/*
 * MPC5748_wdog.c
 *
 *  Created on: 2019��4��2��
 *      Author: haishengliu
 */
/* Including needed modules to compile this module/procedure */
#include <main.h>


void Wdog_Init(void)//timeout 500ms
{
	 /* Initialize WDOG */
	WDG_Init(&wdg_pal1_Instance, &wdg_pal1_Config0);
}

void Wdog_Feed(void)
{
	/* Reset Watchdog counter */
	WDG_Refresh(&wdg_pal1_Instance);
}


