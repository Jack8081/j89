#ifndef _BTDRV_API_H 
#define _BTDRV_API_H 

#include <zephyr/types.h>

typedef void (*btdrv_rx_cb_t)(void *buf, int len, uint8_t type);

/******************************************************************************/
/*!
 * \par  Description:
 *	bluetooth driver initialize
 * \param[in]	rx_cb: hci data receive call back function
 * \param[out]	none
 * \return init result: 0 success, other: failed
 *******************************************************************************/
int btdrv_init(btdrv_rx_cb_t rx_cb);

/******************************************************************************/
/*!
 * \par  Description:
 *	bluetooth driver exit
 * \param[in]	none
 * \param[out]	none
 * \return	none
 *******************************************************************************/
void btdrv_exit(void);

/******************************************************************************/
/*!
 * \par  Description:
 *	send data from buffer to uart tx.
 * \param[in]	buf  data-buffer.
 * \param[in]	len	  length of data to be sent.
 * \param[in]	type hci packet type.
 * \param[out]   none
 * \return	   length of data to be sent.
 *****************************************************************************/
int btdrv_send(void *buf, int len, uint8_t type);

/******************************************************************************/
/*!
 * \par  Description:
 *	send sync msg to controller.
 * \param[in]	msg_id  message id, > 0.
 * \param[in]	data	data to be sent.
 * \param[in]	len     length of data to be sent.
 * \param[out]   none
 * \return	   error or not, 0 - success, < 0 - errors.
 *****************************************************************************/
int btdrv_send_mailbox_sync(uint8_t msg_id, uint8_t *data, uint16_t len);
void * btdrv_get_mailbox_ctx(void);

/******************************************************************************/
/*!
 * \par  Description:
 *	set hci acl data print enable.
 * \param[in]	enable: enable or disable.
 * \param[out]   none
 * \return pre-enabel state.
 *****************************************************************************/
uint8_t hci_set_acl_print_enable(uint8_t enable);

/******************************************************************************/
/*!
 * \par  Description:
 * get bt controller init
 * \param[in]	none
 * \param[out]	none
 * \return	-1:not initialized  0:already initialized
 *******************************************************************************/
int btdrv_get_init_status(void);

/******************************************************************************/
/*!
 * \par  Description:
 * set controller active device
 * \param[in]  link_handle  ---- esco handle for esco link; acl handle for acl link ( not support acl now. )
 * \param[in]  mode         ---- 1 for acl link;  2 for esco link;
 * \return	-1:not initialized  0:already initialized
 *******************************************************************************/
void bt_controller_set_active_device(uint32_t link_handle, uint32_t mode);

/******************************************************************************/
/*!
 * \par  Description:
 * get bt controller bqb mode
 * \param[in]	none
 * \param[out]	none
 * \return bqb mode (call after btdrv_init finish)
 *******************************************************************************/
uint8_t bt_controller_get_bqb_mode(void);
#endif
