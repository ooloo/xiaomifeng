#include <assert.h>
#include "Hoofs.h"

FILE *lfs_open(const char *path, const char *mode, size_t fileLen)
{
	char *tmp;
	FILE *stream;

	if(fileLen == 0)
	{
		stream = fopen(path, "w+b");
		assert(stream);

		tmp = (char*)calloc(1, MIN_FILE_BLK);
		assert(tmp);

		assert(fwrite(tmp, MIN_FILE_BLK, 1, stream) == 1);

		//assert(fflush(stream) == 0);

		fprintf(stderr,"creat file block '%s'",path);

		free(tmp);
	}
	else
	{
		stream = fopen(path, "r+b");
		assert(stream);
	}

	assert(fseek(stream, fileLen, SEEK_SET) == 0);

	return stream;
}

int lfs_write(const void *ptr, size_t size, size_t nmemb, size_t fileLen, FILE *stream)
{
	int appendNum;
	char *tmp;

	if((fileLen+size*nmemb)/MIN_FILE_BLK != fileLen/MIN_FILE_BLK)
	{
		assert(fseek(stream, 0L, SEEK_END) == 0);

		appendNum = (size*nmemb)/MIN_FILE_BLK + 1;

		tmp = (char*)calloc(appendNum, MIN_FILE_BLK);
		assert(tmp);

		assert(fwrite(tmp, MIN_FILE_BLK, appendNum, stream) == appendNum);

		//assert(fflush(stream) == 0);

		assert(fseek(stream, fileLen, SEEK_SET) == 0);

		free(tmp);
	}

	assert(fwrite(ptr, size, nmemb, stream) == nmemb);

	return nmemb;
}

int lfs_close(FILE *stream)
{
	assert(stream);

	fclose(stream);

	return 0;
}

