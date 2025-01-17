#include "StreamParser.h"
#include "string.h"

//********************USER.C*******************************************************
uint8_t UART2_Rx = 0;
char UART2_Rx_Buf[128] = {0};
uint8_t UART2_Rx_CNT = 0;
sParser_t parser;

char buffList[4][STRING_BUFFER_SIZE];

metaData_t databuff[20] = {
    [0] = {buffList[0]},
    [3] = {buffList[1]},
    [5] = {buffList[2]},
    [10] = {buffList[3]}
};

// 定义格式化字符串，用于数据输出
char *fmtStr = "ffff";
//********************USER.C*******************************************************
/**
 * @brief UART发送函数//putchar重定义
 */
int UART_printf(UART_HandleTypeDef *huart, const char *fmt, ...) 
{
    va_list ap;
    va_start(ap, fmt);
    int length;
    char buffer[128];
    length = vsnprintf(buffer, 128, fmt, ap);
    HAL_UART_Transmit(huart, (uint8_t *) buffer, length, HAL_MAX_DELAY);//串口发送
    va_end(ap);
    return length;
}

#if USE_STRING_PARSE
#include "stdlib.h"
#endif

#define IS_NUMBER(ch) (ch >= '0' && ch <= '9')

static char* errorMsg[] = {
    "Error: malloc Failed.",
    "Error: String parser disabled.",
    "Error: Type list error."
};

/**
 * 初始化解析器对象
 * 
 * @param parser 解析器对象指针
 * @param headStr 标记字符串的起始标识，用于标识解析的起始位置
 * @param tailStr 标记字符串的结束标识，用于标识解析的结束位置
 * @param divStr 分隔符字符串，用于分隔不同的数据项
 * @param typeList 类型列表字符串，用于指示数据项的类型
 * 
 * @note 本函数初始化解析器对象的各种成员变量，为解析过程做准备。
 *       对于空指针传入的字符串，相应长度被设置为0。
 */
void SParser_Init(pSParser_t parser, char* headStr, char* tailStr, char* divStr, char* typeList)
{
    parser->headStr = headStr;
    parser->headStrLen = headStr == NULL ? 0 : strlen(headStr);
    parser->tailStr = tailStr;
    parser->TailStrLen = tailStr == NULL ? 0 : strlen(tailStr);
    parser->divStr = divStr;
    parser->divStrLen = divStr == NULL ? 0 : strlen(divStr);
    parser->typeList = typeList;

    parser->chPtr[0] = headStr;
    parser->typePtr = parser->typeList;

    parser->buff[0] = parser->buff[1] = parser->buff[2] = parser->buff[3] = 0;

    parser->mState = FindHead;
}

static metaData_t Parse_Intenger(pSParser_t parser, const char ch)
{
    metaData_t temp = { 0 };

    if (parser->buff[0] == 0 && ch == '-') {
        parser->buff[0] = -1;
    } else if (IS_NUMBER(ch)) {
        parser->buff[2] = parser->buff[2] * 10 + ch - '0';
    } else if (ch == *parser->tailStr || ch == *parser->divStr || ch == '.') {
        temp.intenger = parser->buff[0] == 0 ? parser->buff[2] : parser->buff[2] * -1;
        parser->buff[0] = parser->buff[2] = 0;
        return temp;
    }

    return temp;
}



static metaData_t Parse_Float(pSParser_t parser, const char ch)
{
    metaData_t temp = { 0 };

    if ((temp = Parse_Intenger(parser, ch)).intenger) {
        parser->buff[1 + parser->buff[3]] = temp.intenger;
    }
    if (ch == '.') {
        parser->buff[3] = 1;
    }

    if (ch == *parser->tailStr || ch == *parser->divStr) {
        temp.flt = parser->buff[2];

        while (temp.flt >= 1) {
            temp.flt /= 10;
        }
        temp.flt = (floatTypdef)parser->buff[1] + (parser->buff[1] >= 0 ? temp.flt : -temp.flt);
        memset(parser->buff, 0, 4 * sizeof(size_t));

        return temp;
    }

    return temp;
}

#if USE_STRING_PARSE
static char* Parse_String(pSParser_t parser, char* strBuff, const char ch)
{
    if (parser->buff[3] == 0) {
        if (strBuff == NULL) {
            strBuff = malloc(STRING_BUFFER_SIZE * sizeof(char));

            if (NULL == strBuff) {
                return errorMsg[0];
            }
        }

        parser->buff[3] = 1;
        parser->buff[0] = parser->buff[1] = (size_t)strBuff;
    }

    if (ch == *parser->tailStr || ch == *parser->divStr) {
        char* temp = (char*)parser->buff[1];

        *(char*)parser->buff[0] = '\0';
        parser->buff[0] = parser->buff[3] = parser->buff[1] = 0;
        return temp;
    }

    if (parser->buff[0] == 0 || ((char*)parser->buff[0] - (char*)parser->buff[1]) >= STRING_BUFFER_SIZE)
        return NULL;

    *(char*)parser->buff[0] = ch;
    parser->buff[0] += sizeof(char);

    return NULL;
}
#endif

/**
 * @return return value as follow:
 * 1: compared equal
 * 0: compared not equal
 */
static int CompareString(pSParser_t parser, const char* str, const int strLen, const char ch, const int pSelect)
{
    if (parser->chPtr[pSelect] == NULL) {
        return 1;
    } else if (ch == *parser->chPtr[pSelect]) {
    EqualHook:
        if (strLen == 1) {
            return 1;
        } else {
            parser->chPtr[pSelect] = parser->chPtr[pSelect] + 1;
            if (*parser->chPtr[pSelect] == '\0')
                return 1;
        }
    } else {
        parser->chPtr[pSelect] = (char*)str;
        if (ch == *parser->chPtr[pSelect]) {
            goto EqualHook;
        }
    }
    return 0;
}

int SParser_Parse(pSParser_t parser, pMetaData_t dataArray, const char ch)
{
switch (parser->mState) {
	 case FindHead:
		  if (CompareString(parser, parser->headStr, parser->headStrLen, ch, 0)) {
				parser->chPtr[0] = parser->divStr;
				parser->chPtr[1] = parser->tailStr;
				parser->mState = Parse;
		  }
		  break;
	 case Parse:
		  switch (*parser->typePtr) {
				metaData_t temp;
		  case ' ':
		  case '\0':
				break;
		  case 'd':
				if ((temp = Parse_Intenger(parser, ch)).intenger)
					 parser->temp = temp;
				break;
		  case 'f':
				if ((temp = Parse_Float(parser, ch)).flt)
					 parser->temp = temp;
				break;

        case 's':
#if USE_STRING_PARSE
            parser->temp.strPtr = Parse_String(parser, dataArray[parser->typePtr - parser->typeList].strPtr, ch);
#else
            parser->temp.strPtr = errorMsg[1];
#endif
            break;

        default:
            parser->temp.strPtr = errorMsg[2];
            break;
        }

        if (CompareString(parser, parser->divStr, parser->divStrLen, ch, 0)) {
            parser->chPtr[0] = parser->divStr;

            if (*parser->typePtr != ' ' && *parser->typePtr != '\0') {
                dataArray[parser->typePtr - parser->typeList] = parser->temp;
            }

            if (*(parser->typePtr) == '\0') {
                goto EndOfParse;
            } else {
                parser->typePtr++;
            }
        }

        if (CompareString(parser, parser->tailStr, parser->TailStrLen, ch, 1)) {
            if (*parser->typePtr != ' ')
                dataArray[parser->typePtr - parser->typeList] = parser->temp;

        EndOfParse:
            parser->chPtr[0] = parser->headStr;
            parser->typePtr = parser->typeList;
            parser->mState = FindHead;
            return 1;
        }
    default:
        break;
    }

    return 0;
}
