/********************************************************************************
 * Describe : a string hash table, with integer values. 
 *          : surpport 'ADD', 'DELETE', 'MODIFY', 'SAVE', 'LOAD' operation
 *          : surpport dynamic increse 
 *          : using 64 bit signature to stand for string, witch increase speed and
 *          : decrease memory usage.
 * 
 * Author   : Paul Yang, zhenahoji@gmail.com
 * 
 * Create   : 2008-10-15
 * 
 * Modify   : 2008-10-15
 *******************************************************************************/
#ifndef _PDICT_H
#define _PDICT_H

#include <py_sign.h>


// macros defined here
//
#define COMMON_NULL 0xFFFFFFFE


// data structure define here
//
typedef struct _pnode{
	unsigned int sign1;
	unsigned int sign2;
	int          code;
	int          value;
	unsigned int next;
}PNODE;


typedef struct _int_dict{
	unsigned int*     hashtab;
	unsigned int      hashsize;

	PNODE*            block;
	unsigned int      block_pos;
	unsigned int      block_size;
}PDICT;


// functions defined here
//

/*
 * func : create an PDICT struct
 *
 * args : hashsize, the hash table size
 *      : nodesize, the node array size
 *
 * ret  : NULL, error;
 *      : else, pointer the the PDICT struct
 */ 
PDICT*   pdict_create(const int hashsize, const int nodesize);


/*
 * func : load PDICT from disk file
 *
 * args : path, file
 *
 * ret  : NULL, error
 *      : else, pointer to PDICT struct
 */
PDICT*   pdict_load(const char* path, const char* file);



/*
 * func : load PDICT from disk file
 *
 * args : full_path
 *
 * ret  : NULL, error
 *      : else, pointer to PDICT struct
 */
PDICT*   pdict_load(const char* full_path);



/*
 * func : free a PDICT struct
 */
void     pdict_free(PDICT* pdict);

/*
 * func : reset the hash table;
 */
void     pdict_reset(PDICT* pdict);

/*
 * func : save PDICT to disk file
 *
 * args : pdict, the PDICT pointer 
 *      : path, file, dest path and file
 *
 * ret  : 0, succeed; 
 *        -1, error.
 */
int      pdict_save(PDICT* pdict, const char* path, const char* file);

/*
 * func : add a value pair to the hash table;
 * 
 * args : pdict, the pointer to PDICT 
 *      : key, value, the input pair
 *
 * ret  : 1,  find a same key, value changed;
 *      : 0,  find NO same key, new node added,
 *      : -1, error;
 */
int      pdict_add(PDICT* pdict, const char* key, const int len, const int code, const int value);

/*
 * func : add a node to the hash table;
 * 
 * args : pdict, pointer to PDICT 
 *      : node , pointer to input node
 *
 * ret  : 1,  find a same key, value changed;
 *      : 0,  find NO same key, new node added,
 *      : -1, error;
 */
int      pdict_add(PDICT* pdict, PNODE* node);

/*
 * func : delete a node in the hash table
 *
 * args : pdict, pointer to hash table;
 *      : key, the tobe delete node key
 *
 * ret  : 0, NOT found; 1 founded.
 *
 * node : just mark delete, set pnode->code to -1 mean delete.
 */
int      pdict_del(PDICT* pdict, const char* key, const int len);

/*
 * func : find in the hash table
 *
 * args : pdict, pointer to hash table
 *      : key, a value to search by
 *      : value, search result
 *
 * ret  : 0, NOT found; 1, founded
 */
int      pdict_find(PDICT* pdict, const char* key, const int len, int* code, int* value);

/*
 * func : find node in hash table by signature
 *
 * args : pdict, pointer to hash table
 *      : sign,  64 bit string signature
 *
 * ret  : NULL, not found
 *      : else, pointer to the founded node
 */
PNODE*   pdict_find(PDICT* pdict, SIGN64* sign);

/*
 * func : find int the hash table, return the founed node
 *
 * args : pdict, pointer to the hash table
 *      : key, the key to be searched.
 *
 * ret  : NULL, NOT found
 *      : else, pointer to the founed node
 */
PNODE*   pdict_find(PDICT* pdict, const char* key, const int len);

/*
 * func : get the first node in hash table
 *
 * args : pos, return the position of the first node
 *
 * ret  : NULL, can NOT get first node, hash table empty
 *      : else, pointer to the first node
 */
PNODE*   pdict_first(PDICT* pdict, int* pos);

/*
 * func : get next node from the hash table
 *
 * args : pos, the start position, get node after pos, 
 *
 * ret  : NULL, can NOT get next NODE, reach the end.
 *      : else, pointer to the next NODE
 */
PNODE*   pdict_next(PDICT* pdict, int* pos);

#endif
