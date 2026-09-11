#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

typedef enum {
    CMD_GET_INFO = 0x01,
    CMD_GET_STATUS = 0x02,

    CMD_POWER_ON = 0x10,
    CMD_POWER_OFF = 0x11,
    CMD_POWER_CYCLE = 0x12,

    CMD_GET_CURRENT = 0x20,

    CMD_SET_BAUD = 0x30,
} protocol_command_t;

#endif // !__PROTOCOL_H__
