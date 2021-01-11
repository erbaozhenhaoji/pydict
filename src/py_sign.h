/********************************************************************************
 * Descri : string signature lib, this lib is a wrapper of MD5
 *
 * Author : Paul Yang, zhenhaoji@gmail.com
 *
 * Create : 2008-10-15
 *
 * Update :
 ********************************************************************************/
#ifndef PY_SIGN_H
#define PY_SIGN_H

typedef struct _sign64{
	unsigned long sign;
}SIGN64;

typedef struct _sign128{
	unsigned long sign1;
	unsigned long sign2;
}SIGN128;

/*
 * func : make a 32 bit string signature
 *
 * args : str, len, input string and its length
 *      : sign, the result signature
 *
 * ret  : 
 */
void py_sign32(const char* str, const int len, unsigned int* sign);

/*
 * func : make a 64 bit string signature
 *
 * args : str, len, input string and its length
 *      : sign1, sign2, the result signature
 *
 * ret  :
 */
void py_sign64(const char* str, const int len, unsigned int* sign1, unsigned int* sign2);

/*
 * func : make a 64 bit string signature
 *
 * args : str, len, input string and its length
 *      : sign, pointer to a SIGN64 struct
 *
 * ret  :
 */
void py_sign64(const char* str, const int len, SIGN64* sign);

/*
 * func : make a 64 bit string signature
 *
 * args : str, len, input string and its length
 *      : sign, pointer to a SIGN64 struct
 *
 * ret  :
 */
void py_sign64(const char* str, const int len, unsigned long* sign);



/*
 * func : make a 128 bit signature
 *
 * args : str, len, input string and its length
 *      : sign, pointer to a SIGN128 struct
 * 
 * ret  :
 */
void py_sign128(const char* str, const int len, SIGN128* sign);

#endif
