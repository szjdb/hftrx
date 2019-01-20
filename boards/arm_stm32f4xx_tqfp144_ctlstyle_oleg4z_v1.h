/* $Id$ */
/* board-specific CPU attached signals */
//
// ������ HF Dream Receiver (�� ������� �����)
// ����� ���� ����������� mgs2001@mail.ru
// UA1ARN
//
// ��������� � DSP ���������� "���������-2" �� ���������� 
// STM32F746ZGT6 ������� NAU8822L � FPGA EP4CE22E22I7N
// � �������� DUAL WATCH
// 2xUSB
// Rmainunit_v5em.pcb

#ifndef ARM_STM32F4XX_TQFP144_CTLSTYLE_STORCH_V3_H_INCLUDED
#define ARM_STM32F4XX_TQFP144_CTLSTYLE_STORCH_V3_H_INCLUDED 1

	#if ! defined(STM32H743xx)
		//#error Wrong CPU selected. STM32H743xx expected
	#endif /* ! defined(STM32F767xx) */
	#if ! defined(STM32F767xx)
		#error Wrong CPU selected. STM32F767xx expected
	#endif /* ! defined(STM32F767xx) */

	//#define WITHSAICLOCKFROMI2S 1	/* ���� SAI1 ����������� �� PLL I2S */
	#define WITHI2SCLOCKFROMPIN 1	// �������� ������� �� SPI2 (I2S) �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������
	#define WITHSAICLOCKFROMPIN 1	// �������� ������� �� SAI1 �������� � �������� ����������, � ��������� �������� ����� MCK ������ ����������

	#define WITHUSEPLL		1	/* ������� PLL	*/
	//#define WITHUSESAIPLL	1	/* SAI PLL	*/
	//#define WITHUSESAII2S	1	/* I2S PLL	*/

	#if 1
		// ��� ������� �������� ���������� ����������
		#define WITHCPUXTAL 12000000uL	/* �� ���������� ���������� ����� 12.000 ��� */
		#define REF1_DIV 6			// ref freq = 2.0000 MHz

		#if defined(STM32F767xx)
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS	// overvlocking
		#elif CPUSTYLE_STM32H7XX
			// normal operation frequency
			#define REF1_MUL 384		// 2*384.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#endif

	#elif 0
		// ��� ������� �������� ���������� ����������
		#define WITHCPUXTAL 18432000uL	/* �� ���������� ���������� ����� 18.432 ��� */
		#define REF1_DIV 18			// ref freq = 1.024 MHz

		#if CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 421		// 2*xxx.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#endif

	#else
		// ������������ �� ����������� RC ���������� 16 ���
		#define REF1_DIV 8			// ref freq = 2.000 MHz

		#if defined(STM32F767xx)
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_6WS	// overvlocking
		#elif CPUSTYLE_STM32H7XX
			// normal operation frequency
			#define REF1_MUL 384		// 2*384.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#elif CPUSTYLE_STM32F7XX
			// normal operation frequency
			#define REF1_MUL 216		// 2*216.000 MHz (192 <= PLLN <= 432)
			#define HARDWARE_FLASH_LATENCY FLASH_ACR_LATENCY_7WS	// overvlocking
		#endif

	#endif


	#if WITHI2SCLOCKFROMPIN
		#define FPGADECIMATION 2560
		#define FPGADIVIDERATIO 5
		#define EXTI2S_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO)
		#define EXTSAI_FREQ (REFERENCE_FREQ * DDS1_CLK_MUL / FPGADIVIDERATIO)

		#define ARMI2SMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / (FPGADECIMATION / 256))
		#define ARMSAIMCLK	(REFERENCE_FREQ * DDS1_CLK_MUL / (FPGADECIMATION / 256))
	#else /* WITHI2SCLOCKFROMPIN */
		#define PLLI2SN_MUL 336		// 344.064 (192 <= PLLI2SN <= 432)
		#define SAIREF1_MUL 240		// 245.76 / 1.024 = 240 (49 <= PLLSAIN <= 432)
		// ������� ����������� �����������
		#define ARMI2SMCLK	(12288000UL)
		#define ARMSAIMCLK	(12288000UL)
	#endif /* WITHI2SCLOCKFROMPIN */

	/* ������ ������������ - ����� ������������������ */

	/* ������ ��������� ���� - � ��������������� "������" */
	#define DIRECT_122M88_X1	1	/* �������� ��������� 122.880 ��� */	
	#define FQMODEL_FPGA		1	// FPGA + IQ over I2S

	// +++ �������� ��������, ������������� ��� ������ ������ �������� ����������
	#if 0
		#define BANDSELSTYLE_OLEG4Z	1	/* Up-conversion with working band .030..56 MHz */
		#define CTLREGMODE_OLEG4Z_V1	1	/* "���������" � DSP � FPGA, SD-CARD, dual watch & PA on board */
	#else
		#define BANDSELSTYLE_OLEG4Z_V2	1
		#define CTLREGMODE_OLEG4Z_V2	1	/* "���������" � DSP � FPGA, SD-CARD, dual watch & PA on board */
	#endif

	// --- �������� ��������, ������������� ��� ������ ������

	#define WITHPOWERTRIM		1	// ������� ���������� ���������
	#define WITHPOWERTRIMMIN	10	// ������ ������ ����������� (������������ �� �������)
	#define WITHPOWERTRIMMAX	100	// ������� ������ ����������� (������������ �� �������)

	#define WITHLCDBACKLIGHT	1	// ������� ���������� ���������� ������� 
	#define WITHLCDBACKLIGHTMIN	0	// ������ ������ ����������� (������������ �� �������)
	#define WITHLCDBACKLIGHTMAX	3	// ������� ������ ����������� (������������ �� �������)
	#define WITHKBDBACKLIGHT	1	// ������� ���������� ���������� ���������� 

	//#define WITHPABIASTRIM		1	// ������� ���������� ����� ���������� ������� ��������� �������� �����������
	#define	WITHPABIASMIN		0
	#define	WITHPABIASMAX		255

	/* ���� ������ ����������� ���������� ������� ��� ��� �������� */
	#define BOARD_DETECTOR_SSB 	0		// ��������

	// +++ �������� ��� ���� � DSP ����������
	#define	BOARD_AGCCODE_ON	0x00
	#define	BOARD_AGCCODE_OFF	0x01

	/* ���� �������� ������ ��, ���������� �� ����������� */
	#define BOARD_FILTER_0P5		1	/* 0.5 or 0.3 kHz filter */
	#define BOARD_FILTER_3P1		0	/* 3.1 or 2.75 kHz filter */
	#define BOARD_FILTER_6P0		0	/* 6.0 kHz filter */
	#define BOARD_FILTER_8P0		0	/* 6.0 kHz filter */
	// --- �������� ��� ���� � DSP ����������

	#define WITHPREAMPATT2_6DB 1	// LTC2208 ���������� ��� � ������������� ������������ � ����������� 0 - 6 - 12 - 18 dB */
	//#define WITHATT2_6DB	1		// LTC2217 ���������� ������������� ������������ � ����������� 0 - 6 - 12 - 18 dB ��� ���

	#define WITHAGCMODEONOFF	1	// ��� ���/����
	#define	WITHMIC1LEVEL		1	// ��������� �������� ���������

	#define DSTYLE_UR3LMZMOD	1	// ������������ ��������� ������ � ����������� UR3LMZ
	#define	FONTSTYLE_ITALIC	1	// ������������ �������������� �����

	// +++ ������ �������� ������������ ������ �� ����������
	//#define KEYB_RAVEN20_V5	1		/* 5 ����� ����������: ������������ ������ ��� ������� � DSP ���������� */
	#define KEYB_FPAMEL20_V0A	1	/* 20 ������ �� 5 ����� - ����� rfrontpanel_v0 + LCDMODE_UC1608 � ���������� ������������ � ����� ������������� */
	// --- ������ �������� ������������ ������ �� ����������

	// +++ ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������
	//#define LCDMODE_HARD_SPI	1	/* LCD over SPI line */
	//#define LCDMODE_LTDC	1		/* Use STM32F4xxx with LCD-TFT Controller (LTDC), also need LCDMODE_ILI9341 */
	//#define LCDMODE_LTDC_L8	1	/* ������������ 8 ��� �� ������� ������������� ������. ����� - 16 ���. */
	//#define LCDMODE_WH2002	1	/* ��� ������������ ���������� 20*2, �������� ������ � LCDMODE_HARD_SPI */
	//#define LCDMODE_WH1602	1	/* ��� ������������ ���������� 16*2 */
	//#define LCDMODE_WH1604	1	/* ��� ������������ ���������� 16*4 */
	//#define LCDMODE_WH2004	1	/* ��� ������������ ���������� 20*4 */
	//#define LCDMODE_RDX0077	1	/* ��������� 128*64 � ������������ UC1601.  */
	//#define LCDMODE_RDX0154	1	/* ��������� 132*64 � ������������ UC1601. */
	//#define LCDMODE_RDX0120	1	/* ��������� 64*32 � ������������ UC1601.  */
	//#define LCDMODE_UC1601S_XMIRROR	1	/* ��������� 132*64 � ������������ UC1601.  */
	//#define LCDMODE_UC1601S_TOPDOWN	1	/* LCDMODE_RDX0154 - ����������� ����������� */
	//#define LCDMODE_UC1601S_EXTPOWER	1	/* LCDMODE_RDX0154 - ��� ����������� ��������������� */
	//#define LCDMODE_TIC154	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_TIC218	1	/* ��������� 133*65 � ������������ PCF8535 */
	//#define LCDMODE_PCF8535_TOPDOWN	1	/* ���������� PCF8535 - ����������� ����������� */
	//#define LCDMODE_LS020 	1	/* ��������� 176*132 Sharp LS020B8UD06 � ������������ LR38826 */
	//#define LCDMODE_LS020_TOPDOWN	1	/* LCDMODE_LS020 - ����������� ����������� */
	//#define LCDMODE_LPH88		1	/* ��������� 176*132 LPH8836-2 � ������������ Hitachi HD66773 */
	//#define LCDMODE_LPH88_TOPDOWN	1	/* LCDMODE_LPH88 - ����������� ����������� */
	//#define LCDMODE_ILI9163	1	/* ��������� LPH9157-2 176*132 � ������������ ILITEK ILI9163 - ����� ������� ������, � ������ �����. */
	//#define LCDMODE_ILI9163_TOPDOWN	1	/* LCDMODE_ILI9163 - ����������� ����������� (��� ������� ������, ����� ������� �����) */
	//#define LCDMODE_L2F50	1	/* ��������� 176*132 � ������������ Epson L2F50126 */
	//#define LCDMODE_L2F50_TOPDOWN	1	/* ��������� ���������� � � ������ LCDMODE_L2F50 */
	//#define LCDMODE_S1D13781_NHWACCEL 1	/* �������������� ���������� ������������ EPSON S1D13781 ��� ������ ������� */
	//#define LCDMODE_S1D13781	1	/* ���������� 480*272 � ������������ Epson S1D13781 */
	//#define LCDMODE_S1D13781_TOPDOWN	1	/* LCDMODE_S1D13781 - ����������� ����������� */
	//#define LCDMODE_ILI9225	1	/* ��������� 220*176 SF-TC220H-9223A-N_IC_ILI9225C_2011-01-15 � ������������ ILI9225� */
	//#define LCDMODE_ILI9225_TOPDOWN	1	/* LCDMODE_ILI9225 - ����������� ����������� (��� ������� ����� �� ������) */
	#define LCDMODE_UC1608	1		/* ��������� 240*128 � ������������ UC1608.- ����������� */
	//#define LCDMODE_UC1608_TOPDOWN	1	/* LCDMODE_UC1608 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7735	1	/* ��������� 160*128 � ������������ Sitronix ST7735 - TFT ������ 160 * 128 HY-1.8-SPI */
	//#define LCDMODE_ST7735_TOPDOWN	1	/* LCDMODE_ST7735 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ST7565S	1	/* ��������� WO12864C2-TFH# 128*64 � ������������ Sitronix ST7565S */
	//#define LCDMODE_ST7565S_TOPDOWN	1	/* LCDMODE_ST7565S - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_ILI9320	1	/* ��������� 248*320 � ������������ ILI9320 */
	//#define LCDMODE_ILI9341	1	/* 320*240 SF-TC240T-9370-T � ������������ ILI9341 - STM32F4DISCO */
	//#define LCDMODE_ILI9341_TOPDOWN	1	/* LCDMODE_ILI9341 - ����������� ����������� (��� ������� ������) */
	//#define LCDMODE_LQ043T3DX02K 1	/* LQ043T3DX02K panel (272*480) - SONY PSP-1000 display */
	// --- ���� �� ���� ����� ���������� ��� �������, ��� �������� ������������� ��������

	#define ENCRES_DEFAULT ENCRES_128
	//#define ENCRES_DEFAULT ENCRES_24
	//#define WITHDIRECTFREQENER	1 //(! CTLSTYLE_SW2011ALL && ! CTLSTYLE_UA3DKC)
	//#define	WITHENCODER	1	/* ��� ��������� ������� ������� ������� */

	/* Board hardware configuration */
	//#define CODEC1_TYPE CODEC_TYPE_TLV320AIC23B
	//#define CODEC_TYPE_TLV320AIC23B_USE_SPI	1
	//#define CODEC_TYPE_TLV320AIC23B_USE_8KS	1	/* ����� �������� � sample rate 8 kHz */

	//#define CODEC_TYPE_WM8731_USE_SPI	1
	//#define CODEC_TYPE_WM8731_USE_8KS	1	/* ����� �������� � sample rate 8 kHz */

	//#define CODEC1_TYPE CODEC_TYPE_NAU8822L
	//#define CODEC_TYPE_NAU8822_USE_SPI	1
	//#define CODEC_TYPE_NAU8822_USE_8KS	1	/* ����� �������� � sample rate 8 kHz */

	//#define WITHDTMFPROCESSING 1
	//#define WITHBBOXMIKESRC BOARD_TXAUDIO_LINE

	#define CODEC2_TYPE	CODEC_TYPE_FPGAV1	/* ���������� �������� �� FPGA */
	//#define CODEC_TYPE_CS4272_USE_SPI	1		// codecboard v2.0
	//#define CODEC_TYPE_CS4272_STANDALONE	1		// codecboard v3.0

	#define WITHSAI1_FRAMEBITS 256	// ������ ������ ������
	#define WITHSAI1_FORMATI2S_PHILIPS 1	// ��������� ��� ��������� ������ �� FPGA

	#define WITHSAI2_FRAMEBITS 256	// ������ ������ ������ ��� ���� ��������� �� 24 ���� - ����� ������������������
	#define WITHSAI2_FORMATI2S_PHILIPS 1	// ��������� ��� ��������� ������ �� FPGA

	#define WITHI2S_FORMATI2S_PHILIPS 1	// �������� ������������� ��� �������� ������ � �����, ������������ � ��������� � ���������
	#define WITHI2SHWRXSLAVE	1		// ������� ����� I2S (��������) ����������� � SLAVE MODE
	#define WITHI2SHWTXSLAVE	1		// ���������� ����� I2S (��������) ����������� � SLAVE MODE
	#define WITHNESTEDINTERRUPTS	1	/* ������������ ��� ������� real-time �����. */
	#define WITHINTEGRATEDDSP		1	/* � ��������� �������� ������������� � ������ DSP �����. */
	#define WITHIFDACWIDTH	32		// 1 ��� ���� � 31 ��� ��������
	#define WITHIFADCWIDTH	32		// 1 ��� ���� � 31 ��� ��������
	#define WITHAFADCWIDTH	16		// 1 ��� ���� � 15 ��� ��������
	#define WITHAFDACWIDTH	16		// 1 ��� ���� � 15 ��� ��������
	//#define WITHDACOUTDSPAGC		1	/* ��� ����������� ��� ����� ��� �� ���������� �����. */
	//#define WITHEXTERNALDDSP		1	/* ������� ���������� ������� DSP ������. */
	//#define WITHLOOPBACKTEST	1	/* ������������� ������������ �����, ����������� */
	#define WITHDSPEXTDDC 1			/* ���������� ���������� ������� ����������� */
	#define WITHDSPEXTFIR 1			/* ���������� ��������� �������������� ������� ����������� */
	//#define WITHDSPLOCALFIR 1		/* test: ���������� ��������� �������������� ����������� */
	#define WITHIF4DSP	1	// "�����"
	//#define WITHUSESDCARD		1	// ��������� ��������� SD CARD
	//#define WITHUSEUSBFLASH		1	// ��������� ��������� USB memory stick
	//#define WITHUSEAUDIOREC		1	// ������ ����� �� SD CARD
	//#define WITHUSEDUALWATCH	1	// ������ ��������
	//#define WITHUSEAUDIOREC2CH	1	// ������ ����� �� SD CARD � ������
	#define WITHDACSTRAIGHT 1	/* ��������� ������������ ���� ��� ��� � ������ ������������ ���� */
	#define WITHRTS96 1		/* ������ ��������� ������������ �������� ��������� �� USB */
	//#define WITHFQMETER	1	/* ���� ����� ��������� ������� �������, �� �������� PPS */
	#define WITHUSBNOUNIQUE	1	/* �� ������������ �������������� SERIAL NUMBER */

	#if 0
		#define WITHUSBHEADSET 1	/* ��������� �������� USB ���������� ��� ��������� - ����� ������������ */
		#define WITHBBOX	1	// Black Box mode - ���������� ��� ������� ����������
		#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_USB
	#elif 0
		#define WITHBBOX	1	// Black Box mode - ���������� ��� ������� ����������
		#define	WITHBBOXFREQ	255000000uL		// ������� ����� ���������
		#define	WITHBBOXSUBMODE	SUBMODE_USB	// ������������ ����� ������
	#elif 0
		#define WITHBBOX	1	// Black Box mode - ���������� ��� ������� ����������
		#define	WITHBBOXFREQ	26985000L		// ������� ����� ���������
		//#define	WITHBBOXFREQ	(26985000L - 260)		// ������� ����� ��������� - 135 ��������� ������� ����� �  122.88 ��� ��������� � ���� ����� � 100 ��� ����������� ��� ���������
		//#define	WITHBBOXFREQ	(26985000L - 1600)		// ������� ����� ���������
		//#define	WITHBBOXFREQ	(14070000L - 1000)		// ������������� BPSK ������
		//#define	WITHBBOXFREQ	(14065000L - 135)		// ������� ����� ��������� - 135 ��������� ������� ����� �  122.88 ��� ��������� � ���� ����� � 100 ��� ����������� ��� ���������
		//#define	WITHBBOXFREQ	14065000L		// ������� ����� ���������
		//#define	WITHBBOXFREQ	(14065000L - 1000)		// ������� ����� ���������
		//#define	WITHBBOXSUBMODE	SUBMODE_USB	// ������������ ����� ������
		#define	WITHBBOXSUBMODE	SUBMODE_BPSK	// ������������ ����� ������
		//#define	WITHBBOXFREQ	27100000L		// ������� ����� ���������
		//#define	WITHBBOXSUBMODE	SUBMODE_CW	// ������������ ����� ������
		//#define	WITHBBOXTX		1		// �������������� ������� �� ��������
		//#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_2TONE
	#elif 0
		#define WITHBBOX	1	// Black Box mode - ���������� ��� ������� ����������
		#define	WITHBBOXFREQ	136000L		// ������� ����� ���������
		#define	WITHBBOXSUBMODE	SUBMODE_USB	// ������������ ����� ������
		#define WITHBBOXREC	1		// �������������� ��������� ������ ������ ����� ������ �������
	#elif 0
		#define WITHBBOX	1	// Black Box mode - ���������� ��� ������� ����������
		#define	WITHBBOXFREQ	7030000L		// ������� ����� ���������
		#define	WITHBBOXSUBMODE	SUBMODE_LSB	// ������������ ����� ������
		#define	WITHBBOXTX		1		// �������������� ������� �� ��������
		#define	WITHBBOXMIKESRC	BOARD_TXAUDIO_2TONE
	#elif 0
		#define WITHBBOX	1	// Black Box mode - ���������� ��� ������� ����������
		#define	WITHBBOXFREQ	7030000L		// ������� ����� ���������
		#define	WITHBBOXSUBMODE	SUBMODE_CWZ	// ������������ ����� ������
		#define	WITHBBOXTX		1		// �������������� ������� �� ��������
		//#define	WITHBBOXMIKESRC BOARD_TXAUDIO_2TONE
	#endif
	//#define WITHMODEMIQLOOPBACK	1	/* ����� �������� ����������� ������������ ���������� */


	// FPGA section
	//#define	WITHFPGAWAIT_AS	1	/* FPGA ����������� �� ����������� ���������� ���������� - ��������� ��������� �������� ����� �������������� SPI � ���������� */
	#define	WITHFPGALOAD_PS	1	/* FPGA ����������� ����������� � ������� SPI */

	// +++ ��� ������ ����� ���������, �������� ���������������� �������� �������
	//#define WITHRFSG	1	/* �������� ���������� �� ������-�����������. */
	//#define WITHTX		1	/* �������� ���������� ������������ - ���������, ����������� ����. */
	#if 0
		#define WITHAUTOTUNER	1	/* ���� ������� ���������� */
		#define SHORTSET7	1	
	#endif
	#define WITHIFSHIFT	1	/* ������������ IF SHIFT */
	//#define WITHIFSHIFTOFFSET	(-250)	/* ��������� �������� IF SHIFT */
	//#define WITHPBT		1	/* ������������ PBT (���� LO3 ����) */
	#define WITHCAT		1	/* ������������ CAT */
	//#define WITHDEBUG		1	/* ���������� ������ ����� COM-����. ��� CAT (WITHCAT) */
	//#define WITHMODEM		1	/* ���������� �������� ��� ���������� � ���������������� ����������� */
	//#define WITHFREEDV	1	/* ��������� ������ FreeDV - http://freedv.org/ */ 
	//#define WITHNMEA		1	/* ������������ NMEA parser */
	//#define WITHBEACON	1	/* ������������ ����� ����� */
	//#define WITHVOX			1	/* ������������ VOX */
	//#define WITHSHOWSWRPWR 1	/* �� ������� ������������ ������������ SWR-meter � PWR-meter */
	//#define WITHSWRMTR	1		/* ���������� ��� */
	//#define WITHPWRMTR	1	/* ��������� �������� �������� ��� */
	//#define WITHPWRLIN	1	/* ��������� �������� �������� ���������� ���������� � �� �������� */
	//#define WITHBARS		1	/* ����������� S-����� � SWR-����� */
	//#define WITHVOLTLEVEL	1	/* ����������� ���������� ��� */
	//#define WITHCURRLEVEL	1	/* ����������� ���� ���������� ������� */
	//#define WITHSWLMODE	1	/* ��������� ����������� ��������� ������ � swl-mode */
	//#define WITHVIBROPLEX	1	/* ����������� �������� �������� ������������ */
	//#define WITHSPKMUTE		1	/* ���������� ����������� �������� */
	// ���� �� ����������� ���������� ���������������
	////#define WITHPOTWPM		1	/* ������������ ����������� �������� �������� � ��������� �������������� */
	//#define WITHPOTGAIN		1	/* ���������� �������� �� �������������� */
	//#define WITHPOTPOWER	1	/* ��������� �������� �� ������������� */
	//#define WITHANTSELECT	1	// ���������� ������������� ������

	//#define WITHMENU 	1	/* ���������������� ���� ����� ���� ��������� - ���� ����������� ������ */

	//#define WITHONLYBANDS 1		/* ����������� ����� ���� ���������� ������������� ����������� */
	//#define WITHBCBANDS	1		/* � ������� ���������� ������������ ����������� ��������� */
	#define WITHWARCBANDS	1	/* � ������� ���������� ������������ HF WARC ��������� */
	//#define WITHLO1LEVELADJ		1	/* �������� ���������� ������� (����������) LO1 */
	//#define WITHLFM		1	/* LFM MODE */
	//#define WITHTEMPSENSOR	1	/* ����������� ������ � ������� ����������� */
	#define WITHREFSENSOR	1		/* ��������� �� ����������� ������ ��� �������� ���������� */
	#define WITHDIRECTBANDS 1	/* ������ ������� � ���������� �� �������� �� ���������� */
	// --- ��� ������ ����� ���������, �������� ���������������� �������� �������

	//#define LO1PHASES	1		/* ������ ������ ������� ���������� ����� DDS � ��������������� ������� ���� */
	#define DEFPREAMPSTATE 	0	/* ��� �� ��������� ������� (1) ��� �������� (0) */
	//#define WITHFANTIMER	1	/* ������������� �� ������� ���������� � ��������� �������� */

	/* ��� �� ������ �������� � ������ ������������ � ����������� */
	//#define NVRAM_TYPE NVRAM_TYPE_FM25XXXX	// SERIAL FRAM AUTODETECT
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L04	// ��� �� ��� ������������� FM25040A - 5 �����, 512 ����
	#define NVRAM_TYPE NVRAM_TYPE_FM25L16
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L64
	//#define NVRAM_TYPE NVRAM_TYPE_FM25L256	// FM25L256, FM25W256
	//#define NVRAM_TYPE NVRAM_TYPE_CPUEEPROM

	//#define NVRAM_TYPE NVRAM_TYPE_AT25040A
	//#define NVRAM_TYPE NVRAM_TYPE_AT25L16		// demo board with atxmega128a4u
	//#define NVRAM_TYPE NVRAM_TYPE_AT25256A
	//#define NVRAM_TYPE NVRAM_TYPE_BKPSRAM	// ������� ������ � ���������� ��������
	//#define HARDWARE_IGNORENONVRAM	1		// ������� �� ������ ��� ��� �������� NVRAM

	// End of NVRAM definitions section
	#define FTW_RESOLUTION 32	/* ����������� FTW ���������� DDS */

	#define MODEL_DIRECT	1	/* ������������ ������ ������, � �� ��������� */
	/* Board hardware configuration */
	#define DDS1_TYPE DDS_TYPE_FPGAV1
	//#define PLL1_TYPE PLL_TYPE_SI570
	//#define PLL1_FRACTIONAL_LENGTH	28	/* Si570: lower 28 bits is a fractional part */
	//#define DDS1_TYPE DDS_TYPE_AD9951
	//#define PLL1_TYPE PLL_TYPE_ADF4001
	//#define DDS2_TYPE DDS_TYPE_AD9834
	//#define RTC1_TYPE RTC_TYPE_M41T81	/* ST M41T81M6 RTC clock chip with I2C interface */
	//#define RTC1_TYPE RTC_TYPE_STM32F4xx	/* STM32F4xx/STM32F7xx internal RTC peripherial */
	//#define TSC1_TYPE TSC_TYPE_STMPE811	/* touch screen controller */
	//#define DAC1_TYPE	99999		/* ������� ��� ��� ���������� �������� ������� */
	#define	XVTR1_TYPE XVTR_TYPE_R820T	/* R820T chip */

	#define DDS1_CLK_DIV	1		/* �������� ������� ������� ����� ������� � DDS1 */

	/* ���������� ������� �� SPI ���� */
	//#define targetdds1	SPI_CSEL_PG0 	/* FPGA NCO1 */
	#define targetext1	SPI_CSEL_PG15 	/* external devices control */
	#define targetctl1	SPI_CSEL_PG7 	/* control register as a chain of registers */
	#define targetfpga1	SPI_CSEL_PG1 	/* control register in FPGA */

	#define targetlcd	targetext1 	/* LCD over SPI line devices control */ 
	#define targetuc1608 targetext1	/* LCD with positive chip select signal	*/

	#define targetnvram SPI_CSEL_PG8  	/* serial nvram */
	//#define targetcodec1 SPI_CSEL_PG6 	/* NAU8822L */

	#define WITHMODESETFULLNFMWFM 1

	#define WITHWFM	1			/* ������������ WFM */
	/* ��� ��������� � ������ ������������ ������� */
	#define IF3_FMASK	(IF3_FMASK_0P5 | IF3_FMASK_3P1 /* | IF3_FMASK_6P0 | IF3_FMASK_8P0*/)
	/* ��� ��������� � ������ ������������ ������� ��� �������� */
	#define IF3_FMASKTX	(IF3_FMASK_3P1 /*| IF3_FMASK_6P0 */)
	/* �������, ��� ������� ����� ������� HAVE */
	#define IF3_FHAVE	( IF3_FMASK_0P5 | IF3_FMASK_3P1 /*| IF3_FMASK_6P0 | IF3_FMASK_8P0*/)

	#define WITHSPLIT	1	/* ���������� �������� ���������� ����� ������� */
	//#define WITHSPLITEX	1	/* ������������� ���������� �������� ���������� */
	#define WITHCATEXT	1	/* ����������� ����� ������ CAT */
	//#define WITHELKEY	1
	//#define WITHKBDENCODER 1	// ����������� ������� ��������
	//#define WITHKEYBOARD 1	/* � ������ ���������� ���� ���������� */
	//#define KEYBOARD_USE_ADC	1	/* �� ����� ����� �����������  ������  �������. �� vref - 6.8K, ����� 2.2�, 4.7� � 13K. */

	#define VOLTLEVEL_UPPER		47	// 4.7 kOhm - ������� �������� �������� ������� ����������
	#define VOLTLEVEL_LOWER		10	// 1.0 kOhm - ������ ��������


	// ���������� ������ ��� ����������.
	enum 
	{ 
	#if WITHREFSENSOR
		VREFIX = 17,		// Reference voltage
	#endif /* WITHREFSENSOR */
	#if WITHTEMPSENSOR
		TEMPIX = 16,
	#endif /* WITHTEMPSENSOR */
	};

#endif /* ARM_STM32F4XX_TQFP144_CTLSTYLE_STORCH_V3_H_INCLUDED */
