#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pdict.h>

int main(int argc, char* argv[])
{
	PDICT* pdict = NULL;
	
	pdict = pdict_create(100, 555);
	assert(pdict);

  pdict_add(pdict, "hongkong1", 9, 111, 1111);
  pdict_add(pdict, "hongkong2", 9, 222, 2222);
  pdict_add(pdict, "hongkong3", 9, 333, 3333);
  
  pdict_save(pdict, "./", "dictbin");

	return 0;
}
