#include <monapi/messages.h>
#include "file.h"

int __mlibc_mona_file_is_valid(void *f, int fid)
{
	puts(__func__);
	_printf("valid? = %d\n", fid != MONA_FAILURE ? 1 : 0);
	return fid != MONA_FAILURE ? 1 : 0;
}

int __mlibc_mona_file_open(void *f, const char *file, int flags)
{
    uint32_t fid = 0;

    fid = monapi_file_open(file, MONAPI_FALSE);
    if( fid == MONA_FAILURE && flags & F_CREATE )
        fid = monapi_file_open(file, MONAPI_TRUE);
    _logprintf("fid = %x\n", fid);
    return (int)fid;
}

int __mlibc_mona_file_close(void *f, int fid)
{
	return monapi_file_close(fid) == MONA_FAILURE ? 0 : 1;
}

int __mlibc_mona_file_read(void *self, void *buf, size_t size)
{
    monapi_cmemoryinfo *cmi = NULL;
    FILE *f = NULL;
    uint32_t fid = 0;
    unsigned char *p = buf;
    int readsize = 0;
    int i = 0;
    f = (FILE*)self;
    fid = f->file;
//    _logprintf("fid = %x, buf = %x, size = %d\n", fid, buf, size);
    cmi = monapi_file_read(fid, (uint32_t)size);
    if( cmi == NULL )
    {
        return -1;
    }
    readsize = (int)cmi->Size;
    memcpy(p, cmi->Data, readsize);
//  monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);

//  monapi_file_seek((uint32_t)id, (uint32_t)readsize+f->_extra->offset, SEEK_SET);
    monapi_file_seek(fid, readsize, SEEK_CUR);
    return readsize;
}

int __mlibc_mona_file_write(void *self, void *buf, size_t size)
{
    uint32_t result = 0;
    FILE *f = NULL;
    monapi_cmemoryinfo* cmi = NULL;

    f = (FILE*)self;

    cmi = monapi_cmemoryinfo_new();
    if( !monapi_cmemoryinfo_create(cmi, size, 0) )
    {
        monapi_cmemoryinfo_delete(cmi);
        return -1;
    }
    memcpy(cmi->Data, buf, cmi->Size);

    result = monapi_file_write((uint32_t)f->file, cmi, cmi->Size);

    monapi_cmemoryinfo_dispose(cmi);
    monapi_cmemoryinfo_delete(cmi);

    monapi_file_seek((uint32_t)f->file, (uint32_t)size+f->offset, SEEK_SET);

    return (int)result;
}

int __mlibc_mona_file_seek(void *self, fpos_t pos, int whence)
{
    MONAPI_BOOL result = 0;
    FILE *f = (FILE*)self;

    result = monapi_file_seek((uint32_t)f->file, (uint32_t)pos, (uint32_t)whence);
    if( result == MONAPI_FALSE )
    {
        return -1;
    }

    return 0;
}

struct __mlibc_file_operators_ __file_ops =
{
	__mlibc_mona_file_open,
	__mlibc_mona_file_close,
	__mlibc_mona_file_read,
	__mlibc_mona_file_write,
	__mlibc_mona_file_seek,
	__mlibc_mona_file_is_valid
};