#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#ifdef __cplusplus
 extern "C" {
#endif

// Configuração Básica do RP2040
//#define CFG_TUSB_MCU             OPT_MCU_RP2040
//#define CFG_TUSB_OS              OPT_OS_NONE
#define CFG_TUSB_DEBUG           0

// Ativa apenas a pilha Device (desativa Host)
#define CFG_TUD_ENABLED          1
#define CFG_TUH_ENABLED          0

#define CFG_TUSB_RHPORT0_MODE    (OPT_MODE_DEVICE | OPT_MODE_FULL_SPEED)

// Configurações de Endpoints e Buffers
#define CFG_TUD_ENDPOINT0_SIZE   64

// 7 Interfaces CDC (ACM0 até ACM5 + CLI)
#define CFG_TUD_CDC              7

// Reduzido para 128 bytes para não estourar a RAM de buffers USB (4KB DPRAM)
#define CFG_TUD_CDC_RX_BUFSIZE   128
#define CFG_TUD_CDC_TX_BUFSIZE   128
#define CFG_TUD_CDC_EP_BUFSIZE   64

#ifdef __cplusplus
 }
#endif

#endif /* _TUSB_CONFIG_H_ */