/***********************************************************************************
 * Describe : a simple hash table, interger key, interger value
 * 
 * Author   : Paul Yang, zhenahoji@gmail.com
 * 
 * Create   : 2008-10-15
 * 
 * Modify   : 2008-10-15
 **********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <py_sign.h>
#include <py_utils.h>
#include <pdict.h>


#define BLOCK_STEP 50000

/*
 * func : create an PDICT struct
 *
 * args : hashsize, the hash table size
 *      : nodesize, the node array size
 *
 * ret  : NULL, error;
 *      : else, pointer the the PDICT struct
 */ 
PDICT*  pdict_create(const int hashsize, const int nodesize)
{
	PDICT*        pdict = NULL;
	PNODE*        block = NULL;
	unsigned int*   hashtab = NULL;
	

	// create the struct
	pdict = (PDICT*)calloc(1, sizeof(PDICT));
	if(!pdict){
		goto failed;
	}
	
	// create hash table
	hashtab = (unsigned int*)calloc(hashsize, sizeof(unsigned int));
	if(!hashtab){
		goto failed;
	}
	for(int i=0;i<hashsize;i++){
		hashtab[i] = COMMON_NULL;
	}

	block = (PNODE*)calloc(nodesize, sizeof(PNODE));
	if(!block){
		goto failed;
	}
	for(int i=0;i<nodesize;i++){
		block[i].next = COMMON_NULL;
	}

	
	pdict->hashtab      = hashtab;
	pdict->hashsize     = hashsize;
	pdict->block        = block;
	pdict->block_size   = nodesize;
	pdict->block_pos    = 0;

	return pdict;

failed:
	if(pdict){
		free(pdict);
		pdict = NULL;
	}
	if(hashtab){
		free(hashtab);
		hashtab = NULL;
	}
	if(block){
		free(block);
		block = NULL;
	}
	return NULL;
}

/*
 * func : free a PDICT struct
 */
void pdict_free(PDICT* pdict)
{
	if(!pdict){
		return;
	}
	if(pdict->hashtab){
		free(pdict->hashtab);
		pdict->hashtab=NULL;
	}
	if(pdict->block){
		free(pdict->block);
		pdict->block = NULL;
	}
	free(pdict);
	pdict = NULL;
}

/*
 * func : add a value pair to the hash table;
 * 
 * args : pdict, the pointer to PDICT 
 *      : key, value, the input pair
 *
 * ret  : 0,  find a same key, value changed;
 *      : 1,  find NO same key, new node added,
 *      : -1, error;
 */
int pdict_add(PDICT* pdict, const char* key, const int keylen, const int code, const int value)
{
	unsigned int   sign1      = 0;
	unsigned int   sign2      = 0;
	PNODE          node;

	py_sign64(key, keylen, &sign1, &sign2);
	node.sign1 = sign1;
	node.sign2 = sign2;
	node.code  = code;
	node.value = value;

	return pdict_add(pdict, &node);
}

/*
 * func : add a node to the hash table;
 * 
 * args : pdict, pointer to PDICT 
 *      : node , pointer to input node
 *
 * ret  : 0,  find a same key, value changed;
 *      : 1,  find NO same key, new node added,
 *      : -1, error;
 */
int pdict_add(PDICT* pdict, PNODE* node) 
{
	unsigned int   pos        = 0;
	unsigned int   hashval    = 0;
	unsigned int   hashsize   = 0;
	unsigned int*  hashtab    = NULL;
	PNODE*         curnode    = NULL;

	hashval  = node->sign1+node->sign2;
	hashtab  = pdict->hashtab;
	hashsize = pdict->hashsize;
	pos = hashval % hashsize;

	if(hashtab[pos]==COMMON_NULL){ // can not find in hash table
		unsigned int block_size = pdict->block_size;
		unsigned int block_pos  = pdict->block_pos;
		if(block_pos==block_size){ // if block array is full, realloc block array
			PNODE* block = pdict->block;
			block = (PNODE*)realloc(block, sizeof(PNODE)*(block_size+BLOCK_STEP));
			if(!block){
				assert(0);
			}
			block_size += BLOCK_STEP;
			pdict->block = block;
			pdict->block_size = block_size;
		}

		curnode = pdict->block+block_pos;
		curnode->sign1 = node->sign1;
		curnode->sign2 = node->sign2;
		curnode->code  = node->code;
		curnode->value = node->value;
		curnode->next  = COMMON_NULL;
		hashtab[pos]   = block_pos; 

		block_pos++;
		pdict->block_pos = block_pos;
		return 0;
	}
	else{
		unsigned int nodepos = hashtab[pos];
		PNODE*       pnode   = pdict->block+nodepos;
		while((unsigned int)pnode->next!=COMMON_NULL){
			if(pnode->sign1==node->sign1&&pnode->sign2==node->sign2) {
				break;
			}
			pnode     = pdict->block+pnode->next;
		}
		if(pnode->sign1==node->sign1 && pnode->sign2==node->sign2){ // find same key node
			pnode->code  = node->code;
			pnode->value = node->value;
			return 1;
		}

		// can not find same key node, add a new node
		unsigned int block_size = pdict->block_size;
		unsigned int block_pos = pdict->block_pos;
		if(block_pos==block_size){
			PNODE* block = pdict->block;
			block = (PNODE*)realloc(block, sizeof(PNODE)*(block_size+BLOCK_STEP));
			if(!block){
				assert(0);
			}
			block_size += BLOCK_STEP;
			pdict->block = block;
			pdict->block_size = block_size;
		}
		
		PNODE* curnode = pdict->block+block_pos;
		curnode->sign1 = node->sign1;
		curnode->sign2 = node->sign2;
		curnode->code  = node->code;
		curnode->value = node->value;
		curnode->next  = hashtab[pos];  // front insert
		hashtab[pos] = block_pos;

		block_pos++;
		pdict->block_pos = block_pos;
		return 0;
	}

}

/*
 * func : reset the hash table;
 */
void pdict_reset(PDICT* pdict)
{
	for(unsigned int i=0;i<pdict->block_pos;i++){
		pdict->block[i].next = COMMON_NULL;
	}
	pdict->block_pos = 0;

	for(unsigned int i=0;i<pdict->hashsize;i++){
		pdict->hashtab[i] = COMMON_NULL;
	}
}

/*
 * func : get the first node in hash table
 *
 * args : pos, return the position of the first node
 *
 * ret  : NULL, can NOT get first node, hash table empty
 *      : else, pointer to the first node
 */
PNODE* pdict_first(PDICT* pdict, unsigned int* pos)
{
	PNODE* pnode = NULL;

	for(unsigned int i=0;i<pdict->block_pos;i++){
		pnode = pdict->block+i;
		if(pnode->code != -1){
			*pos = i;
			return pnode;
		}
	}
	
	return NULL;
}

/*
 * func : get next node from the hash table
 *
 * args : pos, the start position, get node after pos, 
 *
 * ret  : NULL, can NOT get next NODE, reach the end.
 *      : else, pointer to the next NODE
 */
PNODE* pdict_next(PDICT* pdict, int* pos)
{
	PNODE*     pnode = NULL;

	for(unsigned int i=*pos+1;i<pdict->block_pos;i++){
		pnode = pdict->block+i;
		if(pnode->code != -1){
			*pos = i;
			return pnode;
		}
	}

	return NULL;
}

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
int pdict_del(PDICT* pdict, const char* key, const int keylen)
{
	PNODE* pnode = NULL;

	pnode = pdict_find(pdict, key, keylen);
	if(!pnode){
		return 0;
	}
	else{
		pnode->code = -1;
		return 1;
	}

	
}

/*
 * func : find in the hash table
 *
 * args : pdict, pointer to hash table
 *      : key, a value to search by
 *      : value, search result
 *
 * ret  : 0, NOT found; 1, founded
 */
int pdict_find(PDICT* pdict, const char* key, const int keylen, int* code, int* value)
{
	PNODE* pnode = NULL;

	pnode = pdict_find(pdict, key, keylen);
	if(pnode==NULL){
		return 0;
	}else{
		*code  = pnode->code;
		*value = pnode->value;
		return 1;
	}
}

/*
 * func : find int the hash table, return the founed node
 *
 * args : pdict, pointer to the hash table
 *      : key, the key to be searched.
 *
 * ret  : NULL, NOT found
 *      : else, pointer to the founed node
 */
PNODE* pdict_find(PDICT* pdict, const char* key, const int keylen)
{
	PNODE*         pnode      = NULL;
	SIGN64         sign;

	py_sign64(key, keylen, &sign);

	pnode = pdict_find(pdict, &sign);

	return pnode;
		
}

/*
 * func : find node in hash table by signature
 *
 * args : pdict, pointer to hash table
 *      : sign,  64 bit string signature
 *
 * ret  : NULL, not found
 *      : else, pointer to the founded node
 */
PNODE* pdict_find(PDICT* pdict, SIGN64* sign)
{
	unsigned int   sign1      = 0;
	unsigned int   sign2      = 0;
	unsigned int   hashsize   = 0;
	unsigned int   pos        = 0;
	unsigned int   hashval    = 0;
	unsigned int*  hashtab    = NULL;
	PNODE*         pnode      = NULL;

	sign1      = sign->sign;
	hashval    = sign1;
	hashtab    = pdict->hashtab;
	hashsize   = pdict->hashsize;
	pos = hashval % hashsize;

	if(hashtab[pos]==COMMON_NULL){ // can not find in hash table
		return NULL;
	}
	else{
		unsigned int nodepos = hashtab[pos];
		pnode = pdict->block+nodepos;
		while(pnode->next!=COMMON_NULL){
			if(pnode->sign1==sign1&&pnode->sign2==sign2){
				break;
			}
			pnode = pdict->block + pnode->next;
		}
		if(pnode->sign1==sign1&&pnode->sign2==sign2){ // find same key node
			return pnode;
		}

		return NULL;

	}
	
}

/*
 * func : save PDICT to disk file
 *
 * args : pdict, the PDICT pointer 
 *      : path, file, dest path and file
 *
 * ret  : 0, succeed; 
 *        -1, error.
 */
int pdict_save(PDICT* pdict, const char* path, const char* file)
{
	FILE*  fp = NULL;
	unsigned int hashsize   = 0;
	unsigned int block_pos  = 0;
	char fullpath[256];

	hashsize   = pdict->hashsize;
	block_pos  = pdict->block_pos;

	cmps_path(fullpath, sizeof(fullpath), path, file);
	if((fp=fopen(fullpath, "wb"))==NULL){
		goto failed;
	}
	
	// save integer values of PDICT
	if(fwrite(&hashsize, sizeof(unsigned int), 1, fp)!=1){
		goto failed;
	}
	if(fwrite(&block_pos, sizeof(unsigned int), 1, fp)!=1){
		goto failed;
	}

	// save hashtable
	if(fwrite(pdict->hashtab, sizeof(unsigned int), hashsize, fp)!=hashsize){
		goto failed;
	}
	
	// save blocks
	if(fwrite(pdict->block, sizeof(PNODE), block_pos, fp)!=block_pos){
		goto failed;
	}

	fclose(fp);

	return 0;
failed:
	if(fp){
		fclose(fp);
		fp = NULL;
	}
	return -1;
}

/*
 * func : load PDICT from disk file
 *
 * args : path, file
 *
 * ret  : NULL, error
 *      : else, pointer to PDICT struct
 */
PDICT* pdict_load(const char* path, const char* file)
{
	char  fullpath[512] = {0};

	// open dict file
	cmps_path(fullpath, sizeof(fullpath), path, file);

	return pdict_load(fullpath);
}



/*
 * func : load PDICT from disk file
 *
 * args : full_path
 *
 * ret  : NULL, error
 *      : else, pointer to PDICT struct
 */
PDICT*   pdict_load(const char* full_path)
{
	unsigned int hashsize   = 0;
	unsigned int block_pos  = 0;
	FILE*        fp         = NULL;
	PDICT*       pdict      = NULL;

	// open dict file
	if ((fp = fopen(full_path, "rb")) == NULL)
	{
	    goto failed;
	}

	// load integer values of PDICT
	if (fread(&hashsize, sizeof(unsigned int), 1, fp) != 1){
		goto failed;
	}
	
	if (fread(&block_pos, sizeof(unsigned int), 1, fp) != 1){
		goto failed;
	}
	
	// create PDICT struct
	if ((pdict = pdict_create(hashsize, block_pos+BLOCK_STEP)) == NULL){
		goto failed;
	}
	
	// load hash tabel
	if (fread(pdict->hashtab, sizeof(unsigned int), hashsize, fp) != hashsize){
		goto failed;
	}

	// load blocks
	if (fread(pdict->block, sizeof(PNODE), block_pos, fp) != block_pos){
		goto failed;
	}

	pdict->block_pos  = block_pos;
	pdict->hashsize   = hashsize;

	fclose(fp);
	return pdict;

failed:
	if (fp){
		fclose(fp);
		fp = NULL;
	}
	
	if (pdict){
		pdict_free(pdict);
		pdict = NULL;
	}
	return NULL;
	
}

