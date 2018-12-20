#include <stdio.h>

int  get_int (unsigned char *buf, int location);
short get_short (unsigned char *buf, int location);
void err_exit (void);
void ascebc (char *inbuf, char *outbuf, int nchars);
void *calloc (size_t count, size_t size);
void free (void *ptr);
void usage (void);
float floatFlip (float *value);
float floatSwap (char *value);

/*
 *	Determine Endian
 */

#if defined (mc68000) || defined (sony) || defined(sgi) || defined(sun) || defined (_BIG_ENDIAN)
#define BIG 1
#else
#define BIG 0
#endif

#if defined  (ultrix) || defined (__alpha) || defined (INTEL) ||  defined (__APPLE__)
#define LITTLE 1
#else
#define LITTLE 0
#endif

#define PMODE	0666	/* Read/Write permissions */
#define ZERO	0	/* End of file on read */

/*
 *	jsf messages types from format 0004824_Rev_1.18
 */


/*
 *      Message Types (bytes 4 - 5) of 16 - Byte Message Header
 */


#define Sonar_Data_Msg  80      /* JSF Message type = Sonar data could be subbottom or sidescan */
#define Sidescan_Msg    82      /* JSF Message type = Side Scan Data Message */
#define SAS_Msg         86      /* JSF Message type = SAS processed data */
#define S_Information   182     /* JSF Message type = System Information */
#define F_TimeStamp     426     /* JSF Message type = File Timestamp */
#define F_Padding       428     /* JSF Message type = File Padding */
#define Pitch_Roll_Msg  2020    /* JSF Message type = Pitch/Roll from MRU */
#define NMEA_Msg        2002    /* JSF Message type = NMEA string (time stamped NMEA string) */
#define Press_Msg       2060    /* JSF Message type = Pressure sensor reading */
#define Analog_Msg      2040    /* JSF Message type = Misc. Analog Sensor (ignore) */
#define Doppler_Msg     2080    /* JSF Message type = Doppler Velocity log */
#define Situation_Msg   2090    /* JSF Message type = SAS only systems */
#define S_Comp          2091    /* JSF Message type = Situation Comprehensive */
#define C_Counter       2100    /* JSF Message type = Cable Counter Data */
#define P_Data          2101    /* JSF Message type = Kilometer of Pipe Data */
#define C_Timestamp     2111    /* JSF Message type = Container Timestamp */

/*
 *      End Message Types
 */

/*
 *      Begin Discover II Messages
 */

#define G_Prefix        9001    /* DII Message type = General Prefix */
#define S_Data          9002    /* DII Message type = Situation Data */
#define A_Prefix        9003    /* DII Message type = Acoustic Prefix */

/*
 *      End Discover II Messages
 */


/*
 *      Following are defined in 16 byte JSF header
 */

#define Start_Of_Message 0x1601 /* Marker for JSF 16 byte header */

/*
 *	JSF header Byte 7
 */

#define SubBottom       0       /* Subbottom data type identifier */
#define Low_Sidescan    20      /* Low frequency (75 or 120 KHZ) sidescan subsystem */
#define Hi_Sidescan     21      /* High Frequency (410 KHZ) sidescan subsystem */
#define Higher_Sidescan 22      /* Higher Frequency Sidescan */
/*
 *	End Byte 7
 */

/*
 *	JSF Header Byte 8
 */

#define Port_SS         0       /* Port sidescan channel identifier */
#define Stbd_SS         1       /* Starboard sidescan channel identifier */
#define Env_Data        0       /* Subbottom envelope data (1 short data element) */
#define Ana_Data        1       /* Subbottom analytic data (2 short data elements Real, Imaginary) */
#define Raw_Data        2       /* Subbottom raw unmatched filter data (1 short data element) */
#define Real_Data       3       /* Subbottom Real portion of analytic data (1 short data element) */
#define Pix_Data        4       /* Subbottom Pixel data (1 short data element) */

/*
 *	End Byte 8
 */

/*
 *	End 16 - Byte JSF header description
 */

/*
 *	Define System Type of Message Type 182 (Byte 0 - 3)
 */

#define ET_2xxx_SIB	1	/* 2xxx Sub-bottom/SS with SIB */
#define ET_2xxx_FSIC	2	/* 2xxx Sub-bottom/SS with FSIC */
#define ET_4300_MPX	4	/* 4300-MPX */
#define ET_3200_SB	7	/* 3200-XS Sub-bottom profiler */
#define ET_4200_LIMITED	11	/* 4200 Limited MP DF SS */
#define ET_3100_SB	14	/* 3100 Sub-bottom profiler */
#define ET_2xxx_DSS	16	/* 2xxx Dual SS with SIB */
#define ET_4200_MP	17	/* 4200 MP Dual Freq SS */
#define ET_4700_DF	18	/* 4700 Dynamic Focus */
#define ET_4200_DFSS	19	/* 4200 Dual Frequency SS */
#define ET_2200_MP	21	/* 2200 MP SB DF SS */
#define ET_4600_MP	23	/* 4600 MP Bathy */
#define ET_4200_SFDF	24	/* 4200 Single Frequency DF SS */
#define ET_4125_DFSS	25	/* 4125 DF SS */
#define ET_4600_MB	27	/* 4600 Monopulse Bathy */
#define ET_4200_MPX	30	/* 4200 MPX in MP mode */
#define ET_2205_SB	31	/* 2205 Sub-bottom */
#define ET_2205_DFSS	32	/* 2205 Dual Freq SS */
#define ET_2205_SS	33	/* 2205 SS */
#define ET_2205_SBSS	34	/* 2205 Sub-bottom and SS */
#define ET_2205_UN	35	/* 2205 needs a better description */
#define ET_2205_DFSSSBL	36	/* 2205 DF SS SB Bathy Low */
#define ET_2205_DFSSSBH	37	/* 2205 DF SS SB Bathy High */
#define ET_2205_DFSSBL	38	/* 2205 DF SS Bathy Low */
#define ET_2205_DFSSBH	39	/* 2205 DF SS Bathy High */
#define ET_OTHER	128	/* 4100 or 272 SS */

/*
 *	End System Type Defines
 */


