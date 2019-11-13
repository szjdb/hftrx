/*
 * usbd_cdc.c
 * Проект HF Dream Receiver (КВ приёмник мечты)
 * автор Гена Завидовский mgs2001@mail.ru
 * UA1ARN
*/

#include "hardware.h"
#include "board.h"
#include "audio.h"
#include "display/display.h"
#include "formats.h"
#include <string.h>

#include "../inc/gpio.h"
#include "../inc/spi.h"

#if WITHUSBHW && WITHUSBCDC

#include "usb_core.h"


// CDC class-specific request codes
// (usbcdc11.pdf, 6.2, Table 46)
// see Table 45 for info about the specific requests.
#define CDC_SEND_ENCAPSULATED_COMMAND           0x00
#define CDC_GET_ENCAPSULATED_RESPONSE           0x01
#define CDC_SET_COMM_FEATURE                    0x02
#define CDC_GET_COMM_FEATURE                    0x03
#define CDC_CLEAR_COMM_FEATURE                  0x04
#define CDC_SET_AUX_LINE_STATE                  0x10
#define CDC_SET_HOOK_STATE                      0x11
#define CDC_PULSE_SETUP                         0x12
#define CDC_SEND_PULSE                          0x13
#define CDC_SET_PULSE_TIME                      0x14
#define CDC_RING_AUX_JACK                       0x15
#define CDC_SET_LINE_CODING                     0x20
#define CDC_GET_LINE_CODING                     0x21
#define CDC_SET_CONTROL_LINE_STATE              0x22
#define CDC_SEND_BREAK                          0x23
#define CDC_SET_RINGER_PARMS                    0x30
#define CDC_GET_RINGER_PARMS                    0x31
#define CDC_SET_OPERATION_PARMS                 0x32
#define CDC_GET_OPERATION_PARMS                 0x33
#define CDC_SET_LINE_PARMS                      0x34
#define CDC_GET_LINE_PARMS                      0x35
#define CDC_DIAL_DIGITS                         0x36
#define CDC_SET_UNIT_PARAMETER                  0x37
#define CDC_GET_UNIT_PARAMETER                  0x38
#define CDC_CLEAR_UNIT_PARAMETER                0x39
#define CDC_GET_PROFILE                         0x3A
#define CDC_SET_ETHERNET_MULTICAST_FILTERS      0x40
#define CDC_SET_ETHERNET_PMP_FILTER             0x41
#define CDC_GET_ETHERNET_PMP_FILTER             0x42
#define CDC_SET_ETHERNET_PACKET_FILTER          0x43
#define CDC_GET_ETHERNET_STATISTIC              0x44
#define CDC_SET_ATM_DATA_FORMAT                 0x50
#define CDC_GET_ATM_DEVICE_STATISTICS           0x51
#define CDC_SET_ATM_DEFAULT_VC                  0x52
#define CDC_GET_ATM_VC_STATISTICS               0x53

// Communication feature selector codes
// (usbcdc11.pdf, 6.2.2..6.2.4, Table 47)
#define CDC_ABSTRACT_STATE                      0x01
#define CDC_COUNTRY_SETTING                     0x02

// Feature Status returned for ABSTRACT_STATE Selector
// (usbcdc11.pdf, 6.2.3, Table 48)
#define CDC_IDLE_SETTING                        (1 << 0)
#define CDC_DATA_MULTPLEXED_STATE               (1 << 1)


// Control signal bitmap values for the SetControlLineState request
// (usbcdc11.pdf, 6.2.14, Table 51)
#define CDC_DTE_PRESENT                         (1 << 0)
#define CDC_ACTIVATE_CARRIER                    (1 << 1)

// CDC class-specific notification codes
// (usbcdc11.pdf, 6.3, Table 68)
// see Table 67 for Info about class-specific notifications
#define CDC_NOTIFICATION_NETWORK_CONNECTION     0x00
#define CDC_RESPONSE_AVAILABLE                  0x01
#define CDC_AUX_JACK_HOOK_STATE                 0x08
#define CDC_RING_DETECT                         0x09
#define CDC_NOTIFICATION_SERIAL_STATE           0x20
#define CDC_CALL_STATE_CHANGE                   0x28
#define CDC_LINE_STATE_CHANGE                   0x29
#define CDC_CONNECTION_SPEED_CHANGE             0x2A

// UART state bitmap values (Serial state notification).
// (usbcdc11.pdf, 6.3.5, Table 69)
#define CDC_SERIAL_STATE_OVERRUN                (1 << 6)  // receive data overrun error has occurred
#define CDC_SERIAL_STATE_PARITY                 (1 << 5)  // parity error has occurred
#define CDC_SERIAL_STATE_FRAMING                (1 << 4)  // framing error has occurred
#define CDC_SERIAL_STATE_RING                   (1 << 3)  // state of ring signal detection
#define CDC_SERIAL_STATE_BREAK                  (1 << 2)  // state of break detection
#define CDC_SERIAL_STATE_TX_CARRIER             (1 << 1)  // state of transmission carrier
#define CDC_SERIAL_STATE_RX_CARRIER             (1 << 0)  // state of receiver carrier

// Состояние - выбранные альтернативные конфигурации по каждому интерфейсу USB configuration descriptor
//static RAMDTCM uint8_t altinterfaces [INTERFACE_count];

static RAMDTCM volatile uint_fast16_t usb_cdc_control_state [INTERFACE_count];


static uint_fast32_t ulmin32(uint_fast32_t a, uint_fast32_t b)
{
	return a < b ? a : b;
}

static uint_fast32_t ulmax32(uint_fast32_t a, uint_fast32_t b)
{
	return a > b ? a : b;
}

static uint_fast16_t ulmin16(uint_fast16_t a, uint_fast16_t b)
{
	return a < b ? a : b;
}

static uint_fast16_t ulmax16(uint_fast16_t a, uint_fast16_t b)
{
	return a > b ? a : b;
}

/* получить 32-бит значение */
static uint_fast32_t
USBD_peek_u32(
	const uint8_t * buff
	)
{
	return
		((uint_fast32_t) buff [3] << 24) +
		((uint_fast32_t) buff [2] << 16) +
		((uint_fast32_t) buff [1] << 8) +
		((uint_fast32_t) buff [0] << 0);
}

/* записать в буфер для ответа 32-бит значение */
static void USBD_poke_u32(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
	buff [2] = HI_24BY(v);
	buff [3] = HI_32BY(v);
}

/* записать в буфер для ответа 24-бит значение */
static void USBD_poke_u24(uint8_t * buff, uint_fast32_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
	buff [2] = HI_24BY(v);
}

/* записать в буфер для ответа 16-бит значение */
static void USBD_poke_u16(uint8_t * buff, uint_fast16_t v)
{
	buff [0] = LO_BYTE(v);
	buff [1] = HI_BYTE(v);
}

/* записать в буфер для ответа 8-бит значение */
static void USBD_poke_u8(uint8_t * buff, uint_fast8_t v)
{
	buff [0] = v;
}


static USBALIGN_BEGIN uint8_t cdc1buffout [VIRTUAL_COM_PORT_OUT_DATA_SIZE] USBALIGN_END;
static USBALIGN_BEGIN uint8_t cdc1buffin [VIRTUAL_COM_PORT_IN_DATA_SIZE] USBALIGN_END;
static RAMDTCM uint_fast16_t cdc1buffinlevel;

static USBALIGN_BEGIN uint8_t cdc2buffout [VIRTUAL_COM_PORT_OUT_DATA_SIZE] USBALIGN_END;
static USBALIGN_BEGIN uint8_t cdc2buffin [VIRTUAL_COM_PORT_IN_DATA_SIZE] USBALIGN_END;
static RAMDTCM uint_fast16_t cdc2buffinlevel;

static USBALIGN_BEGIN uint8_t cdc_epXdatabuffout [USB_OTG_MAX_EP0_SIZE] USBALIGN_END;

static RAMDTCM uint_fast32_t dwDTERate [INTERFACE_count];

/* хранимое значение после получения CDC_SET_CONTROL_LINE_STATE */
/* Биты: RTS = 0x02, DTR = 0x01 */

// Обычно используется для переключения на передачу (PTT)
uint_fast8_t usbd_cdc_getrts(void)
{
	return
		((usb_cdc_control_state [INTERFACE_CDC_CONTROL_3a] & CDC_ACTIVATE_CARRIER) != 0) ||
		((usb_cdc_control_state [INTERFACE_CDC_CONTROL_3b] & CDC_ACTIVATE_CARRIER) != 0) ||
		0;
}

// Обычно используется для телеграфной манипуляции (KEYDOWN)
uint_fast8_t usbd_cdc_getdtr(void)
{
	return
		((usb_cdc_control_state [INTERFACE_CDC_CONTROL_3a] & CDC_DTE_PRESENT) != 0) ||
		((usb_cdc_control_state [INTERFACE_CDC_CONTROL_3b] & CDC_DTE_PRESENT) != 0) ||
		0;
}

static RAMDTCM volatile uint_fast8_t usbd_cdc_txenabled = 0;	/* виртуальный флаг разрешения прерывания по готовности передатчика - HARDWARE_CDC_ONTXCHAR*/

/* Разрешение/запрещение прерывания по передаче символа */
void usbd_cdc_enabletx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	usbd_cdc_txenabled = state;
}

static RAMDTCM volatile uint_fast8_t usbd_cdc_rxenabled = 0;	/* виртуальный флаг разрешения прерывания по приёму символа - HARDWARE_CDC_ONRXCHAR */

/* вызывается из обработчика прерываний или при запрещённых прерываниях. */
/* Разрешение/запрещение прерываний про приёму символа */
void usbd_cdc_enablerx(uint_fast8_t state)	/* вызывается из обработчика прерываний */
{
	usbd_cdc_rxenabled = state;
}

/* передача символа после прерывания о готовности передатчика - вызывается из HARDWARE_CDC_ONTXCHAR */
void
usbd_cdc_tx(void * ctx, uint_fast8_t c)
{
	//USBD_HandleTypeDef * const pdev = ctx;
	ASSERT(cdc1buffinlevel < VIRTUAL_COM_PORT_IN_DATA_SIZE);
	cdc1buffin [cdc1buffinlevel ++] = c;
}

/* использование буфера принятых данных */
static void cdc1out_buffer_save(
	const uint8_t * data,
	uint_fast16_t length
	)
{
	unsigned i;

	for (i = 0; usbd_cdc_rxenabled && i < length; ++ i)
	{
		HARDWARE_CDC_ONRXCHAR(data [i]);
	}
}

/* использование буфера принятых данных */
static void cdc2out_buffer_save(
	const uint8_t * data,
	uint_fast16_t length
	)
{
	unsigned i;

	for (i = 0; usbd_cdc_rxenabled && i < length; ++ i)
	{
		//HARDWARE_CDC_ONRXCHAR(data [i]);
	}
}


static USBD_StatusTypeDef USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	switch (epnum)
	{
	case USBD_EP_CDC_OUT:
		/* CDC EP OUT */
		// use CDC data
		//PRINTF(PSTR("0:%u "), USBD_LL_GetRxDataSize(pdev, epnum));
		cdc1out_buffer_save(cdc1buffout, USBD_LL_GetRxDataSize(pdev, epnum));	/* использование буфера принятых данных */
		//memcpy(cdc1buffin, cdc1buffout, cdc1buffinlevel = USBD_LL_GetRxDataSize(pdev, epnum));
		/* Prepare Out endpoint to receive next cdc data packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(epnum), cdc1buffout, VIRTUAL_COM_PORT_OUT_DATA_SIZE);
		break;

	case USBD_EP_CDC_OUTb:
		/* CDC EP OUT */
		// use CDC data
		//PRINTF(PSTR("1:%u "), USBD_LL_GetRxDataSize(pdev, epnum));
		cdc2out_buffer_save(cdc2buffout, USBD_LL_GetRxDataSize(pdev, epnum));	/* использование буфера принятых данных */
		//memcpy(cdc2buffin, cdc2buffout, cdc2buffinlevel = USBD_LL_GetRxDataSize(pdev, epnum));
		/* Prepare Out endpoint to receive next cdc data packet */
		USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(epnum), cdc2buffout, VIRTUAL_COM_PORT_OUT_DATA_SIZE);
		break;

	default:
		break;
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	const USBD_SetupReqTypedef * const req = & pdev->request;

	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	//PRINTF(PSTR("1 USBD_CDC_EP0_RxReady: interfacev=%u: bRequest=%u, wLength=%u\n"), interfacev, req->bRequest, req->wLength);
	switch (interfacev)
	{
	case INTERFACE_CDC_CONTROL_3a:	// CDC interface
	case INTERFACE_CDC_CONTROL_3b:	// CDC interface
		{
			switch (req->bRequest)
			{
			case CDC_SET_LINE_CODING:
				{
					const uint_fast8_t interfacev = LO_BYTE(req->wIndex);
					ASSERT(req->wLength == 7);
					dwDTERate [interfacev] = USBD_peek_u32(& cdc_epXdatabuffout [0]);
					//PRINTF(PSTR("USBD_CDC_EP0_RxReady: CDC_SET_LINE_CODING: interfacev=%u, dwDTERate=%lu, bits=%u\n"), interfacev, dwDTERate [interfacev], cdc_epXdatabuffout [6]);
				}
				break;
			default:
				// непонятно, для чего эти данные?
				TP();
				break;
			}
		}
		break;

	default:
		break;
	}
	return USBD_OK;
}
static USBD_StatusTypeDef USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint_fast8_t epnum)
{
	switch (epnum)
	{
	case (USBD_EP_CDC_IN & 0x7F):
#if 0
		// test usb tx fifo initialization
		#define TLENNNN (VIRTUAL_COM_PORT_IN_DATA_SIZE - 0)
		memset(cdc1buffin, '$', TLENNNN);
		USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), cdc1buffin, TLENNNN);
		break;
#endif
		while (usbd_cdc_txenabled && (cdc1buffinlevel < ARRAY_SIZE(cdc1buffin)))
		{
			HARDWARE_CDC_ONTXCHAR(pdev);	// при отсутствии данных usbd_cdc_txenabled устанавливается в 0
		}
		USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), cdc1buffin, cdc1buffinlevel);
		cdc1buffinlevel = 0;
		break;

	case (USBD_EP_CDC_INb & 0x7F):
		//while (usbd_cdc_txenabled && (cdc2buffinlevel < ARRAY_SIZE(cdc2buffin)))
		//{
		//	HARDWARE_CDC_ONTXCHAR(pdev);	// при отсутствии данных usbd_cdc_txenabled устанавливается в 0
		//}
		USBD_LL_Transmit(pdev, USB_ENDPOINT_IN(epnum), cdc2buffin, cdc2buffinlevel);
		cdc2buffinlevel = 0;
		break;

	case (USBD_EP_CDC_INT & 0x7F):
		break;

	case (USBD_EP_CDC_INTb & 0x7F):
		break;
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDC_Setup(USBD_HandleTypeDef *pdev, const USBD_SetupReqTypedef *req)
{
	static USBALIGN_BEGIN uint8_t buff [32] USBALIGN_END;	// was: 7
	const uint_fast8_t interfacev = LO_BYTE(req->wIndex);

	// WCID devices support
	// В документе от Микрософт по другому расположены данные в запросе: LO_BYTE(req->wValue) это результат запуска и тестирования
	if (req->bRequest == DFU_VENDOR_CODE &&
			(
					LO_BYTE(req->wValue) == INTERFACE_CDC_CONTROL_3a ||
					LO_BYTE(req->wValue) == INTERFACE_CDC_CONTROL_3b ||
					0)
			&& req->wIndex == 0x05)
	{
		PRINTF(PSTR("MS USBD_CDC_Setup: bmRequest=%04X, bRequest=%04X, wValue=%04X, wIndex=%04X, wLength=%04X\n"), req->bmRequest, req->bRequest, req->wValue, req->wIndex, req->wLength);
		//USBD_CtlError(pdev, req);
		return USBD_OK;
	}

	if ((req->bmRequest & USB_REQ_TYPE_DIR) != 0)
	{
		// IN direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			switch (interfacev)
			{

			case INTERFACE_CDC_CONTROL_3a:	// CDC interface
			case INTERFACE_CDC_CONTROL_3b:	// CDC interface
				{
					switch (req->bRequest)
					{
					case CDC_GET_LINE_CODING:
						//PRINTF(PSTR("USBD_CDC_Setup IN: CDC_GET_LINE_CODING, dwDTERate=%lu\n"), (unsigned long) dwDTERate [interfacev]);
						USBD_poke_u32(& buff [0], dwDTERate [interfacev]); // dwDTERate
						buff [4] = 0;	// 1 stop bit
						buff [5] = 0;	// parity=none
						buff [6] = 8;	// bDataBits

						USBD_CtlSendData(pdev, buff, ulmin16(7, req->wLength));
						break;

					default:
						TP();
						USBD_CtlError(pdev, req);
						break;
					}
				}
				break;

			default:
				break;
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (interfacev)
			{

			case INTERFACE_CDC_CONTROL_3a:	// CDC interface
			case INTERFACE_CDC_CONTROL_3b:	// CDC interface
			case INTERFACE_CDC_DATA_4a:	// CDC interface
			case INTERFACE_CDC_DATA_4b:	// CDC interface
				{
					case USB_REQ_GET_INTERFACE:
					{
						static USBALIGN_BEGIN uint8_t buff [64] USBALIGN_END;
						//PRINTF(PSTR("USBD_CDC_Setup: USB_REQ_TYPE_STANDARD USB_REQ_GET_INTERFACE dir=%02X interfacev=%d, req->wLength=%d\n"), req->bmRequest & 0x80, interfacev, (int) req->wLength);
						buff [0] = 0;
						USBD_CtlSendData(pdev, buff, ulmin16(ARRAY_SIZE(buff), req->wLength));
					}
					break;
				}
			}
			break;
		}
	}
	else
	{
		// OUT direction
		switch (req->bmRequest & USB_REQ_TYPE_MASK)
		{
		case USB_REQ_TYPE_CLASS:
			switch (interfacev)
			{
			case INTERFACE_CDC_CONTROL_3a:	// CDC interface
			case INTERFACE_CDC_CONTROL_3b:	// CDC interface
				switch (req->bRequest)
				{
				case CDC_SET_CONTROL_LINE_STATE:
					// Выполнение этого запроса не требует дополнительного чтения данных
					//PRINTF(PSTR("USBD_CDC_Setup OUT: CDC_SET_CONTROL_LINE_STATE, wValue=%04X\n"), req->wValue);
					usb_cdc_control_state [interfacev] = req->wValue;
					ASSERT(req->wLength == 0);
					break;

				default:
					break;
				}
				/* все запросы этого класса устройств */
				if (req->wLength != 0)
				{
					USBD_CtlPrepareRx(pdev, cdc_epXdatabuffout, ulmin16(ARRAY_SIZE(cdc_epXdatabuffout), req->wLength));
				}
				else
				{
					USBD_CtlSendStatus(pdev);
				}
				break;

			default:
				break;
			}
			break;

		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{
			case USB_REQ_SET_INTERFACE:
				switch (interfacev)
				{
				case INTERFACE_CDC_CONTROL_3a:	// CDC interface
				case INTERFACE_CDC_DATA_4a:	// CDC interface
				case INTERFACE_CDC_CONTROL_3b:	// CDC interface
				case INTERFACE_CDC_DATA_4b:	// CDC interface
					// Only zero value here
					//altinterfaces [interfacev] = LO_BYTE(req->wValue);
					//PRINTF("USBD_CDC_Setup: CDC interface %d set to %d\n", (int) interfacev, (int) altinterfaces [interfacev]);
					//bufers_set_cdcalt(altinterfaces [interfacev]);
					USBD_CtlSendStatus(pdev);
					break;
				}
			}
			break;

		default:
			break;
		}
	}
	return USBD_OK;
}

static USBD_StatusTypeDef USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	uint8_t offset;

    /* cdc Open EP IN */
 	for (offset = 0; offset < WITHUSBHWCDC_N; ++ offset)
	{
	   USBD_LL_OpenEP(pdev,
					   USBD_EP_CDC_IN + offset,
					   USBD_EP_TYPE_BULK,
					   VIRTUAL_COM_PORT_IN_DATA_SIZE);

 		USBD_LL_Transmit(pdev, USBD_EP_CDC_IN + offset, NULL, 0);
	     /* cdc Open EP OUT */
		USBD_LL_OpenEP(pdev, USBD_EP_CDC_OUT + offset, USBD_EP_TYPE_BULK, VIRTUAL_COM_PORT_OUT_DATA_SIZE);
		/* CDC Open EP interrupt */
		USBD_LL_OpenEP(pdev, USBD_EP_CDC_INT + offset, USBD_EP_TYPE_INTR, VIRTUAL_COM_PORT_INT_SIZE);
	}

    /* CDC Prepare Out endpoint to receive 1st packet */
    USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(USBD_EP_CDC_OUT), cdc1buffout,  VIRTUAL_COM_PORT_OUT_DATA_SIZE);
    USBD_LL_PrepareReceive(pdev, USB_ENDPOINT_OUT(USBD_EP_CDC_OUTb), cdc2buffout,  VIRTUAL_COM_PORT_OUT_DATA_SIZE);
 	//USBD_LL_StallEP(pdev, USBD_EP_CDC_OUTb); // нельзя

	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3a] = 0;
	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3b] = 0;
	dwDTERate [INTERFACE_CDC_CONTROL_3a] = 115200;
	dwDTERate [INTERFACE_CDC_CONTROL_3b] = 115200;

	return USBD_OK;

}

static USBD_StatusTypeDef USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint_fast8_t cfgidx)
{
	uint_fast8_t offset;

#if WITHUSBCDC

 	for (offset = 0; offset < WITHUSBHWCDC_N; ++ offset)
	{
		USBD_LL_CloseEP(pdev, USBD_EP_CDC_IN + offset);
		USBD_LL_CloseEP(pdev, USBD_EP_CDC_OUT + offset);
		USBD_LL_CloseEP(pdev, USBD_EP_CDC_INT + offset);
	}

	HARDWARE_CDC_ONDISCONNECT();
	/* при потере связи с host снять запрос на передачу */
	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3a] = 0;
	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3b] = 0;

#endif /* WITHUSBCDC */
	return USBD_OK;
}

static void USBD_CDC_ColdInit(void)
{
	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3a] = 0;
	usb_cdc_control_state [INTERFACE_CDC_CONTROL_3b] = 0;
	//memset(altinterfaces, 0, sizeof altinterfaces);
}

const USBD_ClassTypeDef USBD_CLASS_CDC =
{
	USBD_CDC_ColdInit,
	USBD_CDC_Init,	// Init
	USBD_CDC_DeInit,	// DeInit
	USBD_CDC_Setup,		// Setup
	NULL,	// EP0_TxSent
	USBD_CDC_EP0_RxReady,	// EP0_RxReady
	USBD_CDC_DataIn,	// DataIn
	USBD_CDC_DataOut,	// DataOut
	NULL,	//USBD_XXX_SOF,	// SOF
	NULL,	//USBD_XXX_IsoINIncomplete,	// IsoINIncomplete
	NULL,	//USBD_XXX_IsoOUTIncomplete,	// IsoOUTIncomplete
};

#endif /* WITHUSBHW && WITHUSBCDC */
