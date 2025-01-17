/*
 * avrcp_internal.h - avrcp handling

 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <acts_bluetooth/avrcp.h>


#define BT_AVRCP_MAX_MTU						CONFIG_BT_L2CAP_RX_MTU
#define BT_L2CAP_PSM_AVCTP_CONTROL				0x0017

#define BT_SIG_COMPANY_ID						0x001958
#define BT_ACTION_COMPANY_ID					0x03E0

#ifdef CONFIG_BT_TRANSCEIVER //CONFIG_BT_BR_TRANSCEIVER
#define BT_L2CAP_PSM_AVCTP_BROWSING				0x001B
#endif
/* Command or responed */
enum {
	BT_AVRCP_CMD = 0x00,
	BT_AVRCP_RESOPEN = 0x01,
};

/* Ctype */
enum {
	BT_AVRCP_CTYPE_CONTROL = 0x0,
	BT_AVRCP_CTYPE_STATUS = 0x1,
	BT_AVRCP_CTYPE_SPECIFIC_INQUIRY = 0x2,
	BT_AVRCP_CTYPE_NOTIFY = 0x3,
	BT_AVRCP_CTYPE_GENERAL_INQUIRY = 0x4,
	BT_AVRCP_CTYPE_NOT_IMPLEMENTED = 0x8,
	BT_AVRCP_CTYPE_ACCEPTED = 0x9,
	BT_AVRCP_CTYPE_REJECTED = 0xa,
	BT_AVRCP_CTYPE_IN_TRANSITION = 0xb,
	BT_AVRCP_CTYPE_IMPLEMENTED_STABLE = 0xc,
	BT_AVRCP_CTYPE_CHANGED_STABLE = 0xd,
	BT_AVRCP_CTYPE_INTERIM = 0xf,
};

/* Sununit type */
enum {
	BT_AVRCP_SUBUNIT_TYPE_PANEL = 9,
	BT_AVRCP_SUBUNIT_TYPE_UNIT = 0x1F,
};

enum {
	BT_AVRCP_SUBUNIT_ID = 0x0,
	BT_AVRCP_SUBUNIT_ID_IGNORE = 0x7,
};


/* Operation code */
enum {
	BT_AVRCP_VENDOR_DEPENDENT_OPCODE = 0x00,
	BT_AVRCP_UNIT_INFO_OPCODE = 0x30,
	BT_AVRCP_SUBUNIT_INFO_OPCODE = 0x31,
	BT_AVRCP_PASS_THROUGH_OPCODE = 0x7c,
};

enum {
	BT_AVRCP_PDU_ID_GET_CAPABILITIES = 0x10,
	BT_AVRCP_PDU_ID_GetCurrentPlayerApplicationSettingValue = 0x13,
	BT_AVRCP_PDU_ID_SetPlayerApplicationSettingValue = 0x14,
	BT_AVRCP_PDU_ID_GET_ELEMENT_ATTRIBUTES = 0x20,
	BT_AVRCP_PDU_ID_GET_PLAY_STATUS = 0x30,
	BT_AVRCP_PDU_ID_REGISTER_NOTIFICATION = 0x31,
	BT_AVRCP_PDU_ID_REQUEST_CONTINUING_RESPONSE = 0x40,
	BT_AVRCP_PDU_ID_REQUEST_ABORT_CONTINUING_RESPONSE = 0x41,
	BT_AVRCP_PDU_ID_SET_ABSOLUTE_VOLUME = 0x50,
#ifdef CONFIG_BT_TRANSCEIVER //CONFIG_BT_BR_TRANSCEIVER
    BT_SET_ADDRESSEDPLAYER = 0x60,
	BT_AVRCP_PDU_ID_SET_BROWSED_PLAYER = 0x70,
	BT_AVRCP_PDU_ID_GET_FOLDER_ITEMS = 0x71,
	BT_AVRCP_PDU_ID_CHANGER_PATH = 0x72,
	BT_AVRCP_PDU_ID_GET_ITEMS_ATTRI = 0x73,
	BT_AVRCP_PDU_ID_GET_TOLTAL_NUM_ITEMS = 0x75,
	BT_AVRCP_PDU_ID_SEARCH = 0x80,

	BT_AVRCP_PDU_ID_GENERAL_REJ = 0xA0,
#endif
	BT_AVRCP_PDU_ID_UNDEFINED = 0xFF,
};

enum {
	BT_AVRCP_CAPABILITY_ID_COMPANY = 0x02,
	BT_AVRCP_CAPABILITY_ID_EVENT = 0x03,
};

/* to do */
enum {
	BT_AVRCP_ERROR_INVALID_CMD = 0x00,
	BT_AVRCP_ERROR_INVALID_PARAM = 0x01,
	BT_AVRCP_ERROR_PARAM_CONTENT_ERR = 0x02,
	BT_AVRCP_ERROR_INTERNAL_ERR = 0x03,
	BT_AVRCP_ERROR_OP_COMLETED = 0x04,

#ifdef CONFIG_BT_TRANSCEIVER //CONFIG_BT_BR_TRANSCEIVER
    BT_AVRCP_ERROR_RANGE_OUTOF_BOUNDS = 0x0B,
    BT_AVRCP_ERROR_INVALID_PLAYERID = 0x11,
#endif

};

#define BT_AVRCP_EVENT_BIT_MAP(x)		(0x01 << (x))
#define BT_AVRCP_EVENT_SUPPORT(bitmap, event)	((bitmap)&(0x01 << (event)))

#ifdef CONFIG_BT_TRANSCEIVER //CONFIG_BT_BR_TRANSCEIVER
#define AVRCP_LOCAL_TG_SUPPORT_EVENT (0)
#endif

enum {
	BT_AVRCP_STATE_IDLE,
	BT_AVRCP_STATE_CONNECTED,
	BT_AVRCP_STATE_UNIT_INFO_ING,
	BT_AVRCP_STATE_UNIT_INFO_ED,
	BT_AVRCP_STATE_SUBUNIT_INFO_ING,
	BT_AVRCP_STATE_SUBUNIT_INFO_ED,
	BT_AVRCP_STATE_GET_CAPABILITIES_ING,
	BT_AVRCP_STATE_GET_CAPABILITIES_ED,
	BT_AVRCP_STATE_GET_PLAY_STATUS_ING,
	BT_AVRCP_STATE_GET_PLAY_STATUS_ED,
	BT_AVRCP_STATE_REGISTER_NOTIFICATION_ING,
	BT_AVRCP_STATE_REGISTER_NOTIFICATION_ED,
	BT_AVRCP_STATE_STATUS_CHANGED_ED,
	BT_AVRCP_STATE_TRACK_CHANGED_ED,
};

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
struct bt_avctp_header {
	uint8_t ipid:1;
	uint8_t cr:1;				/* command/respone bit */
	uint8_t packet_type:2;
	uint8_t tid:4;				/* Transaction label */
	uint16_t pid;				/* profile id */
} __packed;

struct bt_avrcp_header {
	uint8_t ctype:4;
	uint8_t rfa0:4;
	uint8_t subunit_id:3;
	uint8_t subunit_type:5;
	uint8_t opcode;
} __packed;

struct bt_avrcp_unit_info {
	struct bt_avrcp_header hdr;
	uint8_t data[0];
} __packed;

struct bt_avrcp_subunit_info {
	struct bt_avrcp_header hdr;
	uint8_t data[0];
} __packed;

struct bt_avrcp_vendor_info {
	struct bt_avrcp_header hdr;
	uint8_t company_id[3];
	uint8_t pdu_id;
	uint8_t packet_type:2;
	uint8_t rfa:6;
	uint8_t data[0];
} __packed;

struct bt_avrcp_pass_through_info {
	struct bt_avrcp_header hdr;
	uint8_t op_id:7;
	uint8_t state:1;
	uint8_t data_len;
} __packed;

struct bt_avrcp_vendor_capabilities {
	struct bt_avrcp_vendor_info info;
	uint16_t len;
	uint8_t capabilityid;
	uint8_t capabilitycnt;
	uint8_t capability[0];
} __packed;

struct bt_avrcp_vendor_getplaystatus_rsp {
	struct bt_avrcp_vendor_info info;
	uint16_t len;
	uint8_t SongLen[4];
	uint8_t SongPos[4];
	uint8_t status;
} __packed;

struct bt_avrcp_vendor_notify_cmd {
	struct bt_avrcp_vendor_info info;
	uint16_t len;
	uint8_t event_id;
	uint8_t interval[4];
} __packed;

struct bt_avrcp_vendor_notify_rsp {
	struct bt_avrcp_vendor_info info;
	uint16_t len;
	uint8_t event_id;
	uint8_t status;
} __packed;

struct bt_avrcp_vendor_setvolume_cmd {
	struct bt_avrcp_vendor_info info;
	uint16_t len;
	uint8_t volume:7;
	uint8_t rfd:1;
} __packed;

struct bt_avrcp_vendor_setvolume_rsp {
	struct bt_avrcp_vendor_info info;
	uint16_t len;
	uint8_t volume:7;
	uint8_t rfd:1;
} __packed;

struct Attribute{
	uint32_t id;
	uint16_t character_id;
	uint16_t len;
	uint8_t data[0];
}__packed;

struct bt_avrcp_vendor_getelementatt_rsp {
	struct bt_avrcp_vendor_info info;
	uint16_t len;
	uint8_t attribute_num;
	struct Attribute attribute[0];
} __packed;

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
/* TODO */
#endif

struct bt_avrcp;
struct bt_avrcp_req;

typedef int (*bt_avrcp_func_t)(struct bt_avrcp *session,
			       struct bt_avrcp_req *req);

struct bt_avrcp_req {
	uint8_t subunit_type;
	uint8_t opcode;
	uint8_t tid;
	bt_avrcp_func_t timeout_func;
	struct k_delayed_work timeout_work;
	bt_avrcp_func_t state_sm_func;
};

/** @brief Global AVRCP session structure. */
struct bt_avrcp {
	struct bt_l2cap_br_chan br_chan;
	struct bt_avrcp_req req;
	uint16_t l_tg_ebitmap;				/* Local target support notify event bit map */
	uint16_t r_tg_ebitmap;				/* Remote target support notify event bit map */
	uint32_t r_reg_notify_interval;	/* Remote register notify interval */
	uint32_t l_reg_notify_event;		/* Local register notify event*/
	uint32_t r_reg_notify_event;		/* Remote register notify event*/
	uint8_t CT_state;
	uint8_t ct_tid;
	uint8_t tg_tid;
	uint8_t tg_notify_tid;
#ifdef CONFIG_BT_TRANSCEIVER //CONFIG_BT_BR_TRANSCEIVER
	u8_t track_stat;
	u8_t playback_stat;
	u32_t l_reg_notify_event_accept;		/* Local register notify event, rmt already accept*/

    void *rspptr;
    u16_t rsplen;

    struct bt_l2cap_br_chan browing_br_chan;
#endif
};

struct bt_avrcp_event_cb {
	int (*accept)(struct bt_conn *conn, struct bt_avrcp **session);
#ifdef CONFIG_BT_TRANSCEIVER //CONFIG_BT_BR_TRANSCEIVER
	int (*accept_brs)(struct bt_conn *conn, struct bt_avrcp **session);
#endif
	void (*connected)(struct bt_avrcp *session);
	void (*disconnected)(struct bt_avrcp *session);
	void (*notify)(struct bt_avrcp *session, uint8_t event_id, uint8_t param);
	void (*pass_ctrl)(struct bt_avrcp *session, uint8_t op_id, uint8_t state);
	void (*get_play_status)(struct bt_avrcp *session, uint32_t *song_len, uint32_t *song_pos, uint8_t *play_state);
	void (*get_volume)(struct bt_avrcp *session, uint8_t *volume);
	void (*update_id3_info)(struct bt_avrcp *session, struct id3_info * info);
};

int bt_avrcp_init(void);
int bt_avrcp_cttg_init(void);
int bt_avrcp_connect(struct bt_conn *conn, struct bt_avrcp *session);
int bt_avrcp_disconnect(struct bt_avrcp *session);
int bt_avrcp_pass_through_cmd(struct bt_avrcp *session, avrcp_op_id opid, uint8_t pushedstate);
int bt_avrcp_ctrl_register(struct bt_avrcp_event_cb *cb);

int bt_avrcp_get_capabilities(struct bt_avrcp *session);
int bt_avrcp_get_play_status(struct bt_avrcp *session);
int bt_avrcp_register_notification(struct bt_avrcp *session, uint8_t event_id);
int bt_avrcp_notify_change(struct bt_avrcp *session, uint8_t event_id, uint8_t *param, uint8_t len);
int bt_avrcp_set_absolute_volume(struct bt_avrcp *session, uint32_t param);
int bt_avrcp_get_id3_info(struct bt_avrcp *session);

bool bt_avrcp_cttg_ready_sync_info(struct bt_conn *conn);
int bt_avrcp_cttg_get_sync_info(struct bt_conn *conn, void *info);
int bt_avrcp_cttg_set_sync_info(struct bt_conn *conn, void *info);
int bt_avrcp_set_sync_info_create(struct bt_avrcp *session);
void bt_avrcp_dump_info(void);
#ifdef CONFIG_BT_TRANSCEIVER
#include "tr_avrcp_internal.h"
#endif
