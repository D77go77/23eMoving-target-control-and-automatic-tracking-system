#ifndef __STREAM_HANDLER__
#define __STREAM_HANDLER__

#include "common_inc.h"
#include<stdlib.h>

/**
 * @brief Config whether to use double other than float for "f" type.
 */
#define USE_DOUBLE (1)
/**
 * @brief Config whether to use string parse function.
 * @note String parse may cause addtional memory use through "malloc" in stdlib.
 * @warning !!! Remember to free string memory after using it !!!
 */
#define USE_STRING_PARSE (1)
/**
 * @brief Just the size of the buffer that you give us.
 * 
 */
#define STRING_BUFFER_SIZE (64)

#if USE_DOUBLE
typedef double floatTypdef;
#else
typedef float floatTypdef;
#endif

/**
 * @brief Define metadata for return value to use.
 *
 */
typedef union MetaDataTypedef {
    void *strPtr;
    floatTypdef flt;
    int intenger;
} metaData_t, *pMetaData_t;

typedef struct StreamParser {
    char *headStr;
    char *divStr;
    char *tailStr;

    /**
     * @note "typeList" should be input as follow:
     * " " for ignore
     * "s" for string
     * "d" for intenger
     * "f" for float
     */
    char *typeList;
    char *typePtr;

    char *chPtr[2];

    /**
     * @brief parser buffer refer as:
     * | sign | intenger | decimal | flags |
     */
    size_t buff[4];
    metaData_t temp;

    enum {
        FindHead,
        Parse
    } mState;

    uint8_t headStrLen;
    uint8_t divStrLen;
    uint8_t TailStrLen;
} sParser_t, *pSParser_t;

/**
 * @brief Init parser parameters.
 * 
 * @param parser parser handle
 * @param headStr The header string before all of the data.
 * @param tailStr The tail string after all of the data.
 * @param divStr The separator string between each data.
 * @param typeList A list of the data type between head and tail.
 * 
 * @note For "headStr", "tailStr" and "divStr":
 * You can use not only one character but also a lot of characters 
 * as the parameter string.But no matter how much the character is,
 * it SHOULD be a "string".
 * 
 * @note For "typeList" you can use as follow: \n
 * * " " to ignore; \n
 * * "s" for string; \n
 * * "f" for float; \n
 * * "d" for intenger. \n
 * 
 * @example Considering you want to parse data from the following GPS NMEA protocol:
 *     $GPGGA,092204.999,4250.5589,S,14718.5084,E,1,04,24.4,19.7,M,,,,0000*1F
 * You can use "$GP" as the headStr, "," as the divStr and "*" as the tailStr, and 
 * use the " ffsfsddffs" as the typeList, we will ignore all the data which is not 
 * included in your typeList. Just look like this:
 *     SParser_Init(&parser, "$GP", "*", ",?", strList[0]);
 */
void SParser_Init(pSParser_t parser, char *headStr, char *tailStr, char *divStr, char *typeList);

/**
 * @brief Parse data
 * 
 * @param parser Parser handle
 * @param dataArray A meatData array which used as a return value list for parsed data
 * @param ch Each character in the byte stream
 * @return int Return whether the parse is finished refer as follow: 
 * * 0: unfinished;
 * * 1: finished.
 * 
 * @note For parameter "dataArray":
 * If you set the macro USE_STRING_PARSE as 1 which means you are using the string 
 * parse function, you should ensure that the element in dataArray which corresponding
 * the "s" in typeList is in your control.You should set it as follow: \n
 * * NULL           : if you do not provide a buffer for string to restore; \n
 * * BUFFER ADDRESS : if you can privide a buffer for string to restore. \n
 * By the way, your buffer should preferably be greater than or equal to the macro: \n
 *      STRING_BUFFER_SIZE \n
 * which is the buffer size we will creat for your string if you give us NULL. 
 */
int SParser_Parse(pSParser_t parser, pMetaData_t dataArray, const char ch);


//********************USER.H*******************************************************

extern uint8_t UART2_Rx;
extern char UART2_Rx_Buf[128];
extern uint8_t UART2_Rx_CNT ;
extern sParser_t parser;
extern char buffList[4][STRING_BUFFER_SIZE];
extern metaData_t databuff[20];
extern char *fmtStr;

//********************USER.H*******************************************************


#endif // __STREAM_HANDLER__
