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
#include <py_dict.h>


#define BLOCK_STEP 50000

/*
 * func : create an py_dict_t struct
 *
 * args : hashsize, the hash table size
 *      : nodesize, the node array size
 *
 * ret  : NULL, error;
 *      : else, pointer the the py_dict_t struct
 */ 
py_dict_t*  pydict_create(const int hashsize, const int nodesize)
{
	py_dict_t*          pydict   = NULL;
	PNODE*          block   = NULL;
	unsigned int*   hashtab = NULL;
	int             i       = 0;
	

	// create the struct
	pydict = (py_dict_t*)calloc(1, sizeof(py_dict_t));
	if(!pydict){
		goto failed;
	}
	
	// create hash table
	hashtab = (unsigned int*)calloc(hashsize, sizeof(unsigned int));
	if(!hashtab){
		goto failed;
	}
	for(i=0;i<hashsize;i++){
		hashtab[i] = COMMON_NULL;
	}

	block = (PNODE*)calloc(nodesize, sizeof(PNODE));
	if(!block){
		goto failed;
	}
	for(i=0;i<nodesize;i++){
		block[i].next = COMMON_NULL;
	}

	
	pydict->hashtab      = hashtab;
	pydict->hashsize     = hashsize;
	pydict->block        = block;
	pydict->block_size   = nodesize;
	pydict->block_pos    = 0;

	return pydict;

failed:
	if(pydict){
		free(pydict);
		pydict = NULL;
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
 * func : free a py_dict_t struct
 */
void pydict_free(py_dict_t* pydict)
{
	if(!pydict){
		return;
	}
	if(pydict->hashtab){
		free(pydict->hashtab);
		pydict->hashtab=NULL;
	}
	if(pydict->block){
		free(pydict->block);
		pydict->block = NULL;
	}
	free(pydict);
	pydict = NULL;
}

/*
 * func : add a value pair to the hash table;
 * 
 * args : pydict, the pointer to py_dict_t 
 *      : key, value, the input pair
 *
 * ret  : 0,  find a same key, value changed;
 *      : 1,  find NO same key, new node added,
 *      : -1, error;
 */
int pydict_add(py_dict_t* pydict, const char* key, const int keylen, const int code, const int value)
{
	unsigned int   sign1      = 0;
	unsigned int   sign2      = 0;
	PNODE          node;

	py_sign64_double_int(key, keylen, &sign1, &sign2);
	node.sign1 = sign1;
	node.sign2 = sign2;
	node.code  = code;
	node.value = value;

	return pydict_add_node(pydict, &node);
}

/*
 * func : add a node to the hash table;
 * 
 * args : pydict, pointer to py_dict_t 
 *      : node , pointer to input node
 *
 * ret  : 0,  find a same key, value changed;
 *      : 1,  find NO same key, new node added,
 *      : -1, error;
 */
int pydict_add_node(py_dict_t* pydict, PNODE* node) 
{
	unsigned int   pos        = 0;
	unsigned int   hashval    = 0;
	unsigned int   hashsize   = 0;
	unsigned int*  hashtab    = NULL;
	PNODE*         curnode    = NULL;

	hashval  = node->sign1+node->sign2;
	hashtab  = pydict->hashtab;
	hashsize = pydict->hashsize;
	pos = hashval % hashsize;

	if(hashtab[pos]==COMMON_NULL){ // can not find in hash table
		unsigned int block_size = pydict->block_size;
		unsigned int block_pos  = pydict->block_pos;
		if(block_pos==block_size){ // if block array is full, realloc block array
			PNODE* block = pydict->block;
			block = (PNODE*)realloc(block, sizeof(PNODE)*(block_size+BLOCK_STEP));
			if(!block){
				assert(0);
			}
			block_size += BLOCK_STEP;
			pydict->block = block;
			pydict->block_size = block_size;
		}

		curnode = pydict->block+block_pos;
		curnode->sign1 = node->sign1;
		curnode->sign2 = node->sign2;
		curnode->code  = node->code;
		curnode->value = node->value;
		curnode->next  = COMMON_NULL;
		hashtab[pos]   = block_pos; 

		block_pos++;
		pydict->block_pos = block_pos;
		return 0;
	}
	else{
		unsigned int nodepos = hashtab[pos];
		PNODE*       pnode   = pydict->block+nodepos;
		while((unsigned int)pnode->next!=COMMON_NULL){
			if(pnode->sign1==node->sign1&&pnode->sign2==node->sign2) {
				break;
			}
			pnode     = pydict->block+pnode->next;
		}
		if(pnode->sign1==node->sign1 && pnode->sign2==node->sign2){ // find same key node
			pnode->code  = node->code;
			pnode->value = node->value;
			return 1;
		}

		// can not find same key node, add a new node
		unsigned int block_size = pydict->block_size;
		unsigned int block_pos = pydict->block_pos;
		if(block_pos==block_size){
			PNODE* block = pydict->block;
			block = (PNODE*)realloc(block, sizeof(PNODE)*(block_size+BLOCK_STEP));
			if(!block){
				assert(0);
			}
			block_size += BLOCK_STEP;
			pydict->block = block;
			pydict->block_size = block_size;
		}
		
		PNODE* curnode = pydict->block+block_pos;
		curnode->sign1 = node->sign1;
		curnode->sign2 = node->sign2;
		curnode->code  = node->code;
		curnode->value = node->value;
		curnode->next  = hashtab[pos];  // front insert
		hashtab[pos] = block_pos;

		block_pos++;
		pydict->block_pos = block_pos;
		return 0;
	}

}

/*
 * func : reset the hash table;
 */
void pydict_reset(py_dict_t* pydict)
{
	unsigned int i = 0;

	for(i=0;i<pydict->block_pos;i++){
		pydict->block[i].next = COMMON_NULL;
	}
	pydict->block_pos = 0;

	for(i=0;i<pydict->hashsize;i++){
		pydict->hashtab[i] = COMMON_NULL;
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
PNODE* pydict_first(py_dict_t* pydict, unsigned int* pos)
{
	PNODE*        pnode = NULL;
	unsigned int  i     = 0;

	for(i=0;i<pydict->block_pos;i++){
		pnode = pydict->block+i;
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
PNODE* pydict_next(py_dict_t* pydict, int* pos)
{
	PNODE*        pnode = NULL;
	unsigned int  i     = 0;

	for(i=*pos+1;i<pydict->block_pos;i++){
		pnode = pydict->block+i;
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
 * args : pydict, pointer to hash table;
 *      : key, the tobe delete node key
 *
 * ret  : 0, NOT found; 1 founded.
 *
 * node : just mark delete, set pnode->code to -1 mean delete.
 */
int pydict_del(py_dict_t* pydict, const char* key, const int keylen)
{
	PNODE* pnode = NULL;

	pnode = pydict_find_node_str(pydict, key, keylen);
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
 * args : pydict, pointer to hash table
 *      : key, a value to search by
 *      : value, search result
 *
 * ret  : 0, NOT found; 1, founded
 */
int pydict_find(py_dict_t* pydict, const char* key, const int keylen, int* code, int* value)
{
	PNODE* pnode = NULL;

	pnode = pydict_find_node_str(pydict, key, keylen);
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
 * args : pydict, pointer to the hash table
 *      : key, the key to be searched.
 *
 * ret  : NULL, NOT found
 *      : else, pointer to the founed node
 */
PNODE* pydict_find_node_str(py_dict_t* pydict, const char* key, const int keylen)
{
	PNODE*         pnode      = NULL;
	SIGN64         sign;

	py_sign64_struct(key, keylen, &sign);

	pnode = pydict_find_node(pydict, &sign);

	return pnode;
		
}

/*
 * func : find node in hash table by signature
 *
 * args : pydict, pointer to hash table
 *      : sign,  64 bit string signature
 *
 * ret  : NULL, not found
 *      : else, pointer to the founded node
 */
PNODE* pydict_find_node(py_dict_t* pydict, SIGN64* sign)
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
	hashtab    = pydict->hashtab;
	hashsize   = pydict->hashsize;
	pos = hashval % hashsize;

	if(hashtab[pos]==COMMON_NULL){ // can not find in hash table
		return NULL;
	}
	else{
		unsigned int nodepos = hashtab[pos];
		pnode = pydict->block+nodepos;
		while(pnode->next!=COMMON_NULL){
			if(pnode->sign1==sign1&&pnode->sign2==sign2){
				break;
			}
			pnode = pydict->block + pnode->next;
		}
		if(pnode->sign1==sign1&&pnode->sign2==sign2){ // find same key node
			return pnode;
		}

		return NULL;

	}
	
}

/*
 * func : save py_dict_t to disk file
 *
 * args : pydict, the py_dict_t pointer 
 *      : path, file, dest path and file
 *
 * ret  : 0, succeed; 
 *        -1, error.
 */
int pydict_save(py_dict_t* pydict, const char* path, const char* file)
{
	FILE*  fp = NULL;
	unsigned int hashsize   = 0;
	unsigned int block_pos  = 0;
	char fullpath[256];

	hashsize   = pydict->hashsize;
	block_pos  = pydict->block_pos;

	cmps_path(fullpath, sizeof(fullpath), path, file);
	if((fp=fopen(fullpath, "wb"))==NULL){
		goto failed;
	}
	
	// save integer values of py_dict_t
	if(fwrite(&hashsize, sizeof(unsigned int), 1, fp)!=1){
		goto failed;
	}
	if(fwrite(&block_pos, sizeof(unsigned int), 1, fp)!=1){
		goto failed;
	}

	// save hashtable
	if(fwrite(pydict->hashtab, sizeof(unsigned int), hashsize, fp)!=hashsize){
		goto failed;
	}
	
	// save blocks
	if(fwrite(pydict->block, sizeof(PNODE), block_pos, fp)!=block_pos){
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
 * func : load py_dict_t from disk file
 *
 * args : path, file
 *
 * ret  : NULL, error
 *      : else, pointer to py_dict_t struct
 */
py_dict_t* pydict_load(const char* path, const char* file)
{
	char  fullpath[512] = {0};

	// open dict file
	cmps_path(fullpath, sizeof(fullpath), path, file);

	return pydict_load_fullpath(fullpath);
}



/*
 * func : load py_dict_t from disk file
 *
 * args : full_path
 *
 * ret  : NULL, error
 *      : else, pointer to py_dict_t struct
 */
py_dict_t*   pydict_load_fullpath(const char* full_path)
{
	unsigned int hashsize   = 0;
	unsigned int block_pos  = 0;
	FILE*        fp         = NULL;
	py_dict_t*       pydict      = NULL;

	// open dict file
	if ((fp = fopen(full_path, "rb")) == NULL)
	{
	    goto failed;
	}

	// load integer values of py_dict_t
	if (fread(&hashsize, sizeof(unsigned int), 1, fp) != 1){
		goto failed;
	}
	
	if (fread(&block_pos, sizeof(unsigned int), 1, fp) != 1){
		goto failed;
	}
	
	// create py_dict_t struct
	if ((pydict = pydict_create(hashsize, block_pos+BLOCK_STEP)) == NULL){
		goto failed;
	}
	
	// load hash tabel
	if (fread(pydict->hashtab, sizeof(unsigned int), hashsize, fp) != hashsize){
		goto failed;
	}

	// load blocks
	if (fread(pydict->block, sizeof(PNODE), block_pos, fp) != block_pos){
		goto failed;
	}

	pydict->block_pos  = block_pos;
	pydict->hashsize   = hashsize;

	fclose(fp);
	return pydict;

failed:
	if (fp){
		fclose(fp);
		fp = NULL;
	}
	
	if (pydict){
		pydict_free(pydict);
		pydict = NULL;
	}
	return NULL;
	
}

