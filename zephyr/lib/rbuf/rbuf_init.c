/*******************************************************************************
 * @file    rbuf_init.c
 * @author  MEMS Application Team
 * @version V1.0
 * @date    2020-10-15
 * @brief   ring buffer init for interaction RAM
*******************************************************************************/

/******************************************************************************/
//includes
/******************************************************************************/
#include <device.h>
#include <rbuf/rbuf_msg.h>
#include <rbuf/rbuf_pool.h>

/******************************************************************************/
//constants
/******************************************************************************/

/******************************************************************************/
//typedef
/******************************************************************************/

/******************************************************************************/
//variables
/******************************************************************************/

/******************************************************************************/
//functions
/******************************************************************************/

static int my_pool_init(const struct device *arg)
{
	ARG_UNUSED(arg);

	rbuf_pool_init((char*)RB_ST_POOL, RB_SZ_POOL);;

	return 0;
}

SYS_INIT(my_pool_init, PRE_KERNEL_1, 5);
