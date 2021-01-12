#include <py_sign.h>
#include <assert.h>

static inline unsigned long murmur_hash_64 ( const void * key, int len, unsigned int seed );
static inline unsigned int murmur_hash_32 ( const void * key, int len, unsigned int seed );
/*
 * func : make a 64 bit string signature
 *
 * args : str, len, input string and its length
 *      : sign1, sign2, the result signature
 *
 * ret  :
 */
void py_sign64_double_int(const char* str, int len, unsigned int* iSign1, unsigned int* iSign2)
{
	unsigned long val64 = 0;
	unsigned int sign1 = 0;
	unsigned int sign2 = 0;

	val64 = murmur_hash_64(str,len,0);

//	(*iSign1) = (unsigned int) (val64>>32);
	sign1 = (unsigned int) (val64>>32);
	*iSign1 = sign1;
	//*iSign2 = (unsigned int) val64;
	sign2  = (unsigned int) val64;
	*iSign2 = sign2;
	
}

/*
 * func : make a 64 bit string signature
 *
 * args : str, len, input string and its length
 *      : sign, pointer to a SIGN64 struct
 *
 * ret  :
 */
void py_sign64_struct(const char* str, int len, SIGN64* sign)
{
	unsigned long val64 = 0;

	val64 = murmur_hash_64(str,len,0);

	sign->sign = val64;
}

/*
 * func : make a 64 bit string signature
 *
 * args : str, len, input string and its length
 *      : sign, pointer to a SIGN64 struct
 *
 * ret  :
 */
void py_sign64(const char* str, int len, unsigned long* sign)
{
	unsigned long val64;
	
	val64 = murmur_hash_64(str,len,0);

	*sign = val64;

}

/*
 * func : make a 128 bit signature
 *
 * args : str, len, input string and its length
 *      : sign, pointer to a SIGN128 struct
 * 
 * ret  :
 */
void py_sign128(const char* str, const int len, SIGN128* sign)
{

	assert(str);

	if(len == 0){
		sign->sign1 = 0;
		sign->sign2 = 0;
	}
	else{
		sign->sign1 = murmur_hash_64(str,len,0);
		sign->sign2 = murmur_hash_64(str,len,37);
	}
}


/*
 * func : make a 32 bit string signature
 *
 * args : str, len, input string and its length
 *      : sign, the result signature
 *
 * ret  : 
 */
void py_sign32(const char* str, const int len, unsigned int* sign)
{

	*sign = murmur_hash_32(str, len, 0);

}


unsigned long murmur_hash_64 ( const void * key, int len, unsigned int seed )
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	unsigned int h1 = seed ^ len;
	unsigned int h2 = 0;

	const unsigned int * data = (const unsigned int *)key;

	while(len >= 8)
	{
		unsigned int k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;

		unsigned int k2 = *data++;
		k2 *= m; k2 ^= k2 >> r; k2 *= m;
		h2 *= m; h2 ^= k2;
		len -= 4;
	}

	if(len >= 4)
	{
		unsigned int k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;
	}

	switch(len)
	{
		case 3: h2 ^= ((unsigned char*)data)[2] << 16;
		case 2: h2 ^= ((unsigned char*)data)[1] << 8;
		case 1: h2 ^= ((unsigned char*)data)[0];
			h2 *= m;
	};

	h1 ^= h2 >> 18; h1 *= m;
	h2 ^= h1 >> 22; h2 *= m;
	h1 ^= h2 >> 17; h1 *= m;
	h2 ^= h1 >> 19; h2 *= m;

	unsigned long long h = h1;

	h = (h << 32) | h2;

	return h;
}

unsigned int murmur_hash_32 ( const void * key, int len, unsigned int seed )
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	unsigned int h1 = seed ^ len;
	unsigned int h2 = 0;

	const unsigned int * data = (const unsigned int *)key;

	while(len >= 8)
	{
		unsigned int k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;

		unsigned int k2 = *data++;
		k2 *= m; k2 ^= k2 >> r; k2 *= m;
		h2 *= m; h2 ^= k2;
		len -= 4;
	}

	if(len >= 4)
	{
		unsigned int k1 = *data++;
		k1 *= m; k1 ^= k1 >> r; k1 *= m;
		h1 *= m; h1 ^= k1;
		len -= 4;
	}

	switch(len)
	{
		case 3: h2 ^= ((unsigned char*)data)[2] << 16;
		case 2: h2 ^= ((unsigned char*)data)[1] << 8;
		case 1: h2 ^= ((unsigned char*)data)[0];
			h2 *= m;
	};

	h1 ^= h2 >> 18; h1 *= m;
	h2 ^= h1 >> 22; h2 *= m;
	h1 ^= h2 >> 17; h1 *= m;
	h2 ^= h1 >> 19; h2 *= m;

	unsigned int h = 0;

	h = h1 ^ h2;

	return h;
}

