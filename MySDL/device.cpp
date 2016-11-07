#include "device.h"
#include <assert.h>
size_t cstows(std::wstring&ws, const char *pc)
{
	if (NULL == pc)
	{
		return (size_t)(-1);
	}
	//size_t size_of_ch = strlen(pc)*sizeof(char);
	//size_t size_of_wc = get_wchar_size(pc);
	size_t size_of_wc;
	size_t destlen = mbstowcs(NULL, pc, 0);
	if (destlen == (size_t)(-1))
	{
		return destlen;
	}
	size_of_wc = destlen + 1;
	wchar_t * pw = new wchar_t[size_of_wc];
	mbstowcs(pw, pc, size_of_wc);
	ws = pw;
	delete pw;
	return destlen;
}
/*
wstring 转换为 string
*/
size_t wstocs(std::string&cs, const wchar_t * pw)
{
	if (!pw)
	{
		return (size_t)(-1);
	}
	size_t size = wcslen(pw) * sizeof(wchar_t);
	char *pc = NULL;
	if (!(pc = (char*)malloc(size)))
	{
		return (size_t)(-1);
	}
	size_t destlen = wcstombs(pc, pw, size);
	/*转换不为空时，返回值为-1。如果为空，返回值0*/
	if (destlen == (size_t)(0))
	{
		return destlen;
	}
	cs = pc;
	delete pc;
	return destlen;
}
int enc_unicode_to_utf8_one(unsigned long unic,char *pOutput)
{
	assert(pOutput != NULL);

	if (unic <= 0x0000007F)
	{
		// * U-00000000 - U-0000007F:  0xxxxxxx  
		*pOutput = (unic & 0x7F);
		return 1;
	}
	else if (unic >= 0x00000080 && unic <= 0x000007FF)
	{
		// * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx  
		*(pOutput + 1) = (unic & 0x3F) | 0x80;
		*pOutput = ((unic >> 6) & 0x1F) | 0xC0;
		return 2;
	}
	else if (unic >= 0x00000800 && unic <= 0x0000FFFF)
	{
		// * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx  
		*(pOutput + 2) = (unic & 0x3F) | 0x80;
		*(pOutput + 1) = ((unic >> 6) & 0x3F) | 0x80;
		*pOutput = ((unic >> 12) & 0x0F) | 0xE0;
		return 3;
	}
	else if (unic >= 0x00010000 && unic <= 0x001FFFFF)
	{
		// * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
		*(pOutput + 3) = (unic & 0x3F) | 0x80;
		*(pOutput + 2) = ((unic >> 6) & 0x3F) | 0x80;
		*(pOutput + 1) = ((unic >> 12) & 0x3F) | 0x80;
		*pOutput = ((unic >> 18) & 0x07) | 0xF0;
		return 4;
	}
	else if (unic >= 0x00200000 && unic <= 0x03FFFFFF)
	{
		// * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
		*(pOutput + 4) = (unic & 0x3F) | 0x80;
		*(pOutput + 3) = ((unic >> 6) & 0x3F) | 0x80;
		*(pOutput + 2) = ((unic >> 12) & 0x3F) | 0x80;
		*(pOutput + 1) = ((unic >> 18) & 0x3F) | 0x80;
		*pOutput = ((unic >> 24) & 0x03) | 0xF8;
		return 5;
	}
	else if (unic >= 0x04000000 && unic <= 0x7FFFFFFF)
	{
		// * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
		*(pOutput + 5) = (unic & 0x3F) | 0x80;
		*(pOutput + 4) = ((unic >> 6) & 0x3F) | 0x80;
		*(pOutput + 3) = ((unic >> 12) & 0x3F) | 0x80;
		*(pOutput + 2) = ((unic >> 18) & 0x3F) | 0x80;
		*(pOutput + 1) = ((unic >> 24) & 0x3F) | 0x80;
		*pOutput = ((unic >> 30) & 0x01) | 0xFC;
		return 6;
	}

	return 0;
}
// #c---end  
int enc_get_utf8_size(unsigned char c)
{
	if ((c & 0x80) == 0)
		return 1;
	else if ((c & 0xe0) == 0xc0)
		return 2;
	else if ((c & 0xf0) == 0xe0)
		return 3;
	else if ((c & 0xf8) == 0xf0)
		return 4;
	else if ((c & 0xfc) == 0xf8)
		return 5;
	else if ((c & 0xfe) == 0xfc)
		return 6;
	else return 0;
}

/*****************************************************************************
* 将一个字符的UTF8编码转换成Unicode(UCS-2和UCS-4)编码.
*
* 参数:
*    pInput      指向输入缓冲区, 以UTF-8编码
*    Unic        指向输出缓冲区, 其保存的数据即是Unicode编码值,
*                类型为unsigned long .
*
* 返回值:
*    成功则返回该字符的UTF8编码所占用的字节数; 失败则返回0.
*
* 注意:
*     1. UTF8没有字节序问题, 但是Unicode有字节序要求;
*        字节序分为大端(Big Endian)和小端(Little Endian)两种;
*        在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位)
****************************************************************************/
int enc_utf8_to_unicode_one(const char* pInput, unsigned long *Unic)
{
	assert(pInput != NULL && Unic != NULL);

	// b1 表示UTF-8编码的pInput中的高字节, b2 表示次高字节, ...  
	char b1, b2, b3, b4, b5, b6;

	*Unic = 0x0; // 把 *Unic 初始化为全零  
	int utfbytes = enc_get_utf8_size((unsigned char )*pInput);
	unsigned char *pOutput = (unsigned char *)Unic;

	switch (utfbytes)
	{
	case 1:
		*pOutput = *pInput;
		break;
	case 2:
		b1 = *pInput;
		b2 = *(pInput + 1);
		if ((b2 & 0xE0) != 0x80)
			return 0;
		*pOutput = (b1 << 6) + (b2 & 0x3F);
		*(pOutput + 1) = (b1 >> 2) & 0x07;
		break;
	case 3:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		if (((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80))
			return 0;
		*pOutput = (b2 << 6) + (b3 & 0x3F);
		*(pOutput + 1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
		break;
	case 4:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		b4 = *(pInput + 3);
		if (((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
			|| ((b4 & 0xC0) != 0x80))
			return 0;
		*pOutput = (b3 << 6) + (b4 & 0x3F);
		*(pOutput + 1) = (b2 << 4) + ((b3 >> 2) & 0x0F);
		*(pOutput + 2) = ((b1 << 2) & 0x1C) + ((b2 >> 4) & 0x03);
		break;
	case 5:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		b4 = *(pInput + 3);
		b5 = *(pInput + 4);
		if (((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
			|| ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80))
			return 0;
		*pOutput = (b4 << 6) + (b5 & 0x3F);
		*(pOutput + 1) = (b3 << 4) + ((b4 >> 2) & 0x0F);
		*(pOutput + 2) = (b2 << 2) + ((b3 >> 4) & 0x03);
		*(pOutput + 3) = (b1 << 6);
		break;
	case 6:
		b1 = *pInput;
		b2 = *(pInput + 1);
		b3 = *(pInput + 2);
		b4 = *(pInput + 3);
		b5 = *(pInput + 4);
		b6 = *(pInput + 5);
		if (((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
			|| ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)
			|| ((b6 & 0xC0) != 0x80))
			return 0;
		*pOutput = (b5 << 6) + (b6 & 0x3F);
		*(pOutput + 1) = (b5 << 4) + ((b6 >> 2) & 0x0F);
		*(pOutput + 2) = (b3 << 2) + ((b4 >> 4) & 0x03);
		*(pOutput + 3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);
		break;
	default:
		return 0;
		break;
	}

	return utfbytes;
}
size_t u8stows(std::wstring & ws, const char * pc)
{
	unsigned long wch;
	int len,s;

	ws = L"";
	len = 0;
	for (const char*p = pc; *p; p+=s)
	{
		s = enc_utf8_to_unicode_one(p, &wch);
		if (s == 0)
			return len;
		ws += (wchar_t)wch;
		len += 1;
	}
	return len;
}

size_t wstou8s(std::string & cs, const wchar_t * pw)
{
	char buf[7];
	int len,s;
	cs = "";
	len = 0;
	for (const wchar_t*p = pw; *p; p++)
	{
		s = enc_unicode_to_utf8_one(*p, buf);
		if (s == 0)
			return len;
		buf[s] = 0;
		cs += buf;
		len += s;
	}
	return len;
}
