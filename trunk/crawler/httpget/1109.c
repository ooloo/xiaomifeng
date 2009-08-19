#include <stdio.h>
#include <zlib.h>

int my_uncompress(dest, destLen, source, sourceLen)
	Bytef *dest;
	uLongf *destLen;
	const Bytef *source;
	uLong sourceLen;
{
	z_stream stream;
	int err;
	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;
	if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;
	// gzip zlib
	err = inflateInit2(&stream, 15 + 32);
	if (err != Z_OK) return err;
	err = inflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) {
		inflateEnd(&stream);
		// raw deflate
		stream.next_in = (Bytef*)source;
		stream.avail_in = (uInt)sourceLen;
		/* Check for source > 64K on 16-bit machine: */
		if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
		stream.next_out = dest;
		stream.avail_out = (uInt)*destLen;
		if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;
		stream.zalloc = (alloc_func)0;
		stream.zfree = (free_func)0;
		stream.opaque = (voidpf)0;
		err = inflateInit2(&stream, -15);
		if (err != Z_OK) return err;
		err = inflate(&stream, Z_FINISH);
		if (err != Z_STREAM_END) {
			inflateEnd(&stream);
			/* Check for source > 64K on 16-bit machine: */
			if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
				return Z_DATA_ERROR;
			return err;
		}
	}
	*destLen = stream.total_out;
	err = inflateEnd(&stream);
	return err;
}

