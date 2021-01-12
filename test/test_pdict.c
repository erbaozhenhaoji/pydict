#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <py_dict.h>

int main(int argc, char* argv[])
{
	int    len   = 0;
	int    code  = 0;
	int    value = 0;
	py_dict_t* pydict = NULL;
	char   buff[1024];

	pydict = pydict_load("./", "dictbin");
	assert(pydict);

	fprintf(stdout, "hassize=%d, block_pos=%d\n",pydict->hashsize, pydict->block_pos);

	while(fgets(buff, sizeof(buff), stdin)){
		len = strlen(buff);
		while(buff[len-1]=='\n'){
			buff[--len] = 0;
		}
		if(pydict_find(pydict, buff, len, &code, &value)==1){
			//fprintf(stdout, "%s : code=%d, value=%d\n", buff, code, value);
		}
	}

	return 0;
}
