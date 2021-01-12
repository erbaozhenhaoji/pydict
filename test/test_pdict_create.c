#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <py_dict.h>

int main(int argc, char* argv[])
{
	py_dict_t* pydict = NULL;
	
	pydict = pydict_create(100, 555);
	assert(pydict);

  pydict_add(pydict, "hongkong1", 9, 111, 1111);
  pydict_add(pydict, "hongkong2", 9, 222, 2222);
  pydict_add(pydict, "hongkong3", 9, 333, 3333);
  
  pydict_save(pydict, "./", "dictbin");

	return 0;
}
