#include "tusb.h"

// --- Descritor do Dispositivo USB ---
tusb_desc_device_t const desc_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = TUSB_CLASS_MISC,
    .bDeviceSubClass    = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol    = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0x2E8A, // Raspberry Pi Vendor ID
    .idProduct          = 0x000A,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

uint8_t const * tud_descriptor_device_cb(void) {
    return (uint8_t const *) &desc_device;
}

// --- Descritor de Configuração USB (7 CDC Ports) ---
#define MAIN_CONFIG_TOTAL_LEN (TUD_CONFIG_DESC_LEN + 7 * TUD_CDC_DESC_LEN)

// Endpoints IN/OUT mapeados dentro do limite do hardware RP2040 (Max 15 EPs)
#define EPNUM_CDC_0_NOTIF 0x81
#define EPNUM_CDC_0_OUT   0x01
#define EPNUM_CDC_0_IN    0x82

#define EPNUM_CDC_1_NOTIF 0x83
#define EPNUM_CDC_1_OUT   0x02
#define EPNUM_CDC_1_IN    0x84

#define EPNUM_CDC_2_NOTIF 0x85
#define EPNUM_CDC_2_OUT   0x03
#define EPNUM_CDC_2_IN    0x86

#define EPNUM_CDC_3_NOTIF 0x87
#define EPNUM_CDC_3_OUT   0x04
#define EPNUM_CDC_3_IN    0x88

#define EPNUM_CDC_4_NOTIF 0x89
#define EPNUM_CDC_4_OUT   0x05
#define EPNUM_CDC_4_IN    0x8A

#define EPNUM_CDC_5_NOTIF 0x8B
#define EPNUM_CDC_5_OUT   0x06
#define EPNUM_CDC_5_IN    0x8C

#define EPNUM_CDC_6_NOTIF 0x8D
#define EPNUM_CDC_6_OUT   0x07
#define EPNUM_CDC_6_IN    0x8E

uint8_t const desc_configuration[] = {
    // Config: 1 configuration, 14 interfaces (2 por CDC)
    TUD_CONFIG_DESCRIPTOR(1, 14, 0, MAIN_CONFIG_TOTAL_LEN, 0x00, 100),

    // Interfaces CDC 0 a 6
    TUD_CDC_DESCRIPTOR(0, 4, EPNUM_CDC_0_NOTIF, 8, EPNUM_CDC_0_OUT, EPNUM_CDC_0_IN, 64),
    TUD_CDC_DESCRIPTOR(2, 5, EPNUM_CDC_1_NOTIF, 8, EPNUM_CDC_1_OUT, EPNUM_CDC_1_IN, 64),
    TUD_CDC_DESCRIPTOR(4, 6, EPNUM_CDC_2_NOTIF, 8, EPNUM_CDC_2_OUT, EPNUM_CDC_2_IN, 64),
    TUD_CDC_DESCRIPTOR(6, 7, EPNUM_CDC_3_NOTIF, 8, EPNUM_CDC_3_OUT, EPNUM_CDC_3_IN, 64),
    TUD_CDC_DESCRIPTOR(8, 8, EPNUM_CDC_4_NOTIF, 8, EPNUM_CDC_4_OUT, EPNUM_CDC_4_IN, 64),
    TUD_CDC_DESCRIPTOR(10, 9, EPNUM_CDC_5_NOTIF, 8, EPNUM_CDC_5_OUT, EPNUM_CDC_5_IN, 64),
    TUD_CDC_DESCRIPTOR(12, 10, EPNUM_CDC_6_NOTIF, 8, EPNUM_CDC_6_OUT, EPNUM_CDC_6_IN, 64),
};

uint8_t const * tud_descriptor_configuration_cb(uint8_t index) {
    (void) index;
    return desc_configuration;
}

// --- Descritores de String ---
char const* string_desc_arr [] = {
    (const char[]) { 0x09, 0x04 }, // 0: Idioma (English 0x0409)
    "Raspberry Pi",                // 1: Fabricante
    "Pico Multi-CDC Bridge",       // 2: Produto
    "1234567890",                  // 3: Serial
    "ACM0 Bridge",                 // 4: Interface CDC 0
    "ACM1 Bridge",                 // 5: Interface CDC 1
    "ACM2 Bridge",                 // 6: Interface CDC 2
    "ACM3 Bridge",                 // 7: Interface CDC 3
    "ACM4 Bridge",                 // 8: Interface CDC 4
    "ACM5 Bridge",                 // 9: Interface CDC 5
    "CLI Command Interface"        // 10: Interface CDC 6
};

static uint16_t _desc_str[32];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void) langid;
    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        if (!(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0]))) return NULL;

        const char* str = string_desc_arr[index];
        chr_count = strlen(str);
        if (chr_count > 31) chr_count = 31;

        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
    return _desc_str;
}