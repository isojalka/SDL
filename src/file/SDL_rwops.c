/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* We won't get fseeko64 on QNX if _LARGEFILE64_SOURCE is defined, but the
   configure script knows the C runtime has it and enables it. */
#ifndef __QNXNTO__
/* Need this so Linux systems define fseek64o, ftell64o and off64_t */
#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
#endif

#include "../SDL_internal.h"

#if defined(__WIN32__)
#include "../core/windows/SDL_windows.h"
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

/* This file provides a general interface for SDL to read and write
   data sources.  It can easily be extended to files, memory, etc.
*/

#include "SDL_endian.h"
#include "SDL_rwops.h"

#ifdef __APPLE__
#include "cocoa/SDL_rwopsbundlesupport.h"
#endif /* __APPLE__ */

#ifdef __ANDROID__
#include "../core/android/SDL_android.h"
#include "SDL_system.h"
#endif

#if __NACL__
#include "nacl_io/nacl_io.h"
#endif

#ifdef __MORPHOS__
#include "../core/morphos/SDL_library.h"
#include "../core/morphos/SDL_misc.h"
#include <proto/dos.h>

#undef SDLCALL
#define SDLCALL __saveds

/* This function must preserve all registers except r13 */
asm
("\n"
"	.section \".text\"\n"
"	.align 2\n"
"	.type __restore_r13, @function\n"
"__restore_r13:\n"
"	lwz 13, 24(3)\n"
"	blr\n"
"__end__restore_r13:\n"
"	.size __restore_r13, __end__restore_r13 - __restore_r13\n"
);
#endif

#ifdef __VITA__

#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>

#define READAHEAD_BUFFER_SIZE   1024
static int SDLCALL
vita_file_open(SDL_RWops * context, const char *filename, const char *mode)
{
    int h;
    int open_flags;
    SDL_bool has_r;
    SDL_bool has_w;
    SDL_bool has_a;
    SDL_bool has_plus;

    if (!context)
        return -1;              /* failed (invalid call) */

    context->hidden.vitaio.h = -1;   /* mark this as unusable */
    context->hidden.vitaio.buffer.data = NULL;
    context->hidden.vitaio.buffer.size = 0;
    context->hidden.vitaio.buffer.left = 0;

    open_flags = 0;

    /* "r" = reading, file must exist */
    /* "w" = writing, truncate existing, file may not exist */
    /* "r+"= reading or writing, file must exist            */
    /* "a" = writing, append file may not exist             */
    /* "a+"= append + read, file may not exist              */
    /* "w+" = read, write, truncate. file may not exist    */

    has_r = SDL_strchr(mode, 'r') != NULL;
    has_w = SDL_strchr(mode, 'w') != NULL;
    has_a = SDL_strchr(mode, 'a') != NULL;
    has_plus = SDL_strchr(mode, '+') != NULL;

    if (has_plus)
    {
        if (has_r || has_w || has_a)
        {
            open_flags |= SCE_O_RDWR;
        }
    }
    else
    {
        if (has_r)
        {
            open_flags |= SCE_O_RDONLY;
        }
        if (has_w || has_a)
        {
            open_flags |= SCE_O_WRONLY;
        }
    }
    if (has_w || has_a)
    {
        open_flags |= SCE_O_CREAT;
    }
    if (has_w)
    {
        open_flags |= SCE_O_TRUNC;
    }
    if (has_a)
    {
        open_flags |= SCE_O_APPEND;
    }

    context->hidden.vitaio.buffer.data =
        (char *) SDL_malloc(READAHEAD_BUFFER_SIZE);
    if (!context->hidden.vitaio.buffer.data) {
        return SDL_OutOfMemory();
    }

    /* Try to open the file on the filesystem first */
    h = sceIoOpen(filename, open_flags, 0777);

    if (h < 0) {
        /* Try opening it from app0:/ container if it's a relative path */
        char path[4096];
        SDL_snprintf(path, 4096, "app0:/%s", filename);
        h = sceIoOpen(path, open_flags, 0777);
    }

    if (h < 0) {
        SDL_free(context->hidden.vitaio.buffer.data);
        context->hidden.vitaio.buffer.data = NULL;
        SDL_SetError("Couldn't open %s", filename);
        return -2;              /* failed (sceIoOpen) */
    }
    context->hidden.vitaio.h = h;

    return 0;                   /* ok */
}

static Sint64 SDLCALL
vita_file_size(SDL_RWops * context)
{
    SceIoStat st;
    if (!context || context->hidden.vitaio.h < 0) {
        return SDL_SetError("vita_file_size: invalid context/file not opened");
    }

    if (sceIoGetstatByFd(context->hidden.vitaio.h, &st) < 0) {
        return SDL_SetError("vita_file_size: could not get file size");
    }
    return st.st_size;
}

static Sint64 SDLCALL
vita_file_seek(SDL_RWops * context, Sint64 offset, int whence)
{
    int vitawhence;

    if (!context || context->hidden.vitaio.h < 0) {
        return SDL_SetError("vita_file_seek: invalid context/file not opened");
    }

    /* FIXME: We may be able to satisfy the seek within buffered data */
    if (whence == RW_SEEK_CUR && context->hidden.vitaio.buffer.left) {
        offset -= (long)context->hidden.vitaio.buffer.left;
    }
    context->hidden.vitaio.buffer.left = 0;

    switch (whence) {
    case RW_SEEK_SET:
        vitawhence = SCE_SEEK_SET;
        break;
    case RW_SEEK_CUR:
        vitawhence = SCE_SEEK_CUR;
        break;
    case RW_SEEK_END:
        vitawhence = SCE_SEEK_END;
        break;
    default:
        return SDL_SetError("vita_file_seek: Unknown value for 'whence'");
    }

    return sceIoLseek(context->hidden.vitaio.h, offset, vitawhence);
}

static size_t SDLCALL
vita_file_read(SDL_RWops * context, void *ptr, size_t size, size_t maxnum)
{
    size_t total_need;
    size_t total_read = 0;
    size_t read_ahead;
    size_t byte_read;

    total_need = size * maxnum;

    if (!context || context->hidden.vitaio.h < 0 || !total_need) {
        return 0;
    }

    if (context->hidden.vitaio.buffer.left > 0) {
        void *data = (char *) context->hidden.vitaio.buffer.data +
            context->hidden.vitaio.buffer.size -
            context->hidden.vitaio.buffer.left;
        read_ahead =
            SDL_min(total_need, context->hidden.vitaio.buffer.left);
        SDL_memcpy(ptr, data, read_ahead);
        context->hidden.vitaio.buffer.left -= read_ahead;

        if (read_ahead == total_need) {
            return maxnum;
        }
        ptr = (char *) ptr + read_ahead;
        total_need -= read_ahead;
        total_read += read_ahead;
    }

    if (total_need < READAHEAD_BUFFER_SIZE) {
        byte_read = sceIoRead(context->hidden.vitaio.h, context->hidden.vitaio.buffer.data, READAHEAD_BUFFER_SIZE);
        read_ahead = SDL_min(total_need, (int) byte_read);
        SDL_memcpy(ptr, context->hidden.vitaio.buffer.data, read_ahead);
        context->hidden.vitaio.buffer.size = byte_read;
        context->hidden.vitaio.buffer.left = byte_read - read_ahead;
        total_read += read_ahead;
    } else {
        byte_read = sceIoRead(context->hidden.vitaio.h, ptr, total_need);
        total_read += byte_read;
    }
    return (total_read / size);
}

static size_t SDLCALL
vita_file_write(SDL_RWops * context, const void *ptr, size_t size,
                 size_t num)
{

    size_t total_bytes;
    size_t byte_written;
    size_t nwritten;

    total_bytes = size * num;

    if (!context || context->hidden.vitaio.h < 0 || !size || !total_bytes) {
        return 0;
    }

    if (context->hidden.vitaio.buffer.left) {
        sceIoLseek(context->hidden.vitaio.h, -(SceOff)context->hidden.vitaio.buffer.left, SCE_SEEK_CUR);
        context->hidden.vitaio.buffer.left = 0;
    }

    byte_written = sceIoWrite(context->hidden.vitaio.h, ptr, total_bytes);

    nwritten = byte_written / size;
    return nwritten;
}

static int SDLCALL
vita_file_close(SDL_RWops * context)
{
    if (context) {
        if (context->hidden.vitaio.h >= 0) {
            sceIoClose(context->hidden.vitaio.h);
            context->hidden.vitaio.h = -1;   /* to be sure */
        }
        SDL_free(context->hidden.vitaio.buffer.data);
        context->hidden.vitaio.buffer.data = NULL;
        SDL_FreeRW(context);
    }
    return 0;
}
#endif

#ifdef __WIN32__

/* Functions to read/write Win32 API file pointers */

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER 0xFFFFFFFF
#endif

#define READAHEAD_BUFFER_SIZE   1024

static int SDLCALL
windows_file_open(SDL_RWops * context, const char *filename, const char *mode)
{
    UINT old_error_mode;
    HANDLE h;
    DWORD r_right, w_right;
    DWORD must_exist, truncate;
    int a_mode;

    if (!context)
        return -1;              /* failed (invalid call) */

    context->hidden.windowsio.h = INVALID_HANDLE_VALUE;   /* mark this as unusable */
    context->hidden.windowsio.buffer.data = NULL;
    context->hidden.windowsio.buffer.size = 0;
    context->hidden.windowsio.buffer.left = 0;

    /* "r" = reading, file must exist */
    /* "w" = writing, truncate existing, file may not exist */
    /* "r+"= reading or writing, file must exist            */
    /* "a" = writing, append file may not exist             */
    /* "a+"= append + read, file may not exist              */
    /* "w+" = read, write, truncate. file may not exist    */

    must_exist = (SDL_strchr(mode, 'r') != NULL) ? OPEN_EXISTING : 0;
    truncate = (SDL_strchr(mode, 'w') != NULL) ? CREATE_ALWAYS : 0;
    r_right = (SDL_strchr(mode, '+') != NULL
               || must_exist) ? GENERIC_READ : 0;
    a_mode = (SDL_strchr(mode, 'a') != NULL) ? OPEN_ALWAYS : 0;
    w_right = (a_mode || SDL_strchr(mode, '+')
               || truncate) ? GENERIC_WRITE : 0;

    if (!r_right && !w_right)   /* inconsistent mode */
        return -1;              /* failed (invalid call) */

    context->hidden.windowsio.buffer.data =
        (char *) SDL_malloc(READAHEAD_BUFFER_SIZE);
    if (!context->hidden.windowsio.buffer.data) {
        return SDL_OutOfMemory();
    }
    /* Do not open a dialog box if failure */
    old_error_mode =
        SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

    {
        LPTSTR tstr = WIN_UTF8ToString(filename);
        h = CreateFile(tstr, (w_right | r_right),
                       (w_right) ? 0 : FILE_SHARE_READ, NULL,
                       (must_exist | truncate | a_mode),
                       FILE_ATTRIBUTE_NORMAL, NULL);
        SDL_free(tstr);
    }

    /* restore old behavior */
    SetErrorMode(old_error_mode);

    if (h == INVALID_HANDLE_VALUE) {
        SDL_free(context->hidden.windowsio.buffer.data);
        context->hidden.windowsio.buffer.data = NULL;
        SDL_SetError("Couldn't open %s", filename);
        return -2;              /* failed (CreateFile) */
    }
    context->hidden.windowsio.h = h;
    context->hidden.windowsio.append = a_mode ? SDL_TRUE : SDL_FALSE;

    return 0;                   /* ok */
}

static Sint64 SDLCALL
windows_file_size(SDL_RWops * context)
{
    LARGE_INTEGER size;

    if (!context || context->hidden.windowsio.h == INVALID_HANDLE_VALUE) {
        return SDL_SetError("windows_file_size: invalid context/file not opened");
    }

    if (!GetFileSizeEx(context->hidden.windowsio.h, &size)) {
        return WIN_SetError("windows_file_size");
    }

    return size.QuadPart;
}

static Sint64 SDLCALL
windows_file_seek(SDL_RWops * context, Sint64 offset, int whence)
{
    DWORD windowswhence;
    LARGE_INTEGER windowsoffset;

    if (!context || context->hidden.windowsio.h == INVALID_HANDLE_VALUE) {
        return SDL_SetError("windows_file_seek: invalid context/file not opened");
    }

    /* FIXME: We may be able to satisfy the seek within buffered data */
    if (whence == RW_SEEK_CUR && context->hidden.windowsio.buffer.left) {
        offset -= (long)context->hidden.windowsio.buffer.left;
    }
    context->hidden.windowsio.buffer.left = 0;

    switch (whence) {
    case RW_SEEK_SET:
        windowswhence = FILE_BEGIN;
        break;
    case RW_SEEK_CUR:
        windowswhence = FILE_CURRENT;
        break;
    case RW_SEEK_END:
        windowswhence = FILE_END;
        break;
    default:
        return SDL_SetError("windows_file_seek: Unknown value for 'whence'");
    }

    windowsoffset.QuadPart = offset;
    if (!SetFilePointerEx(context->hidden.windowsio.h, windowsoffset, &windowsoffset, windowswhence)) {
        return WIN_SetError("windows_file_seek");
    }
    return windowsoffset.QuadPart;
}

static size_t SDLCALL
windows_file_read(SDL_RWops * context, void *ptr, size_t size, size_t maxnum)
{
    size_t total_need;
    size_t total_read = 0;
    size_t read_ahead;
    DWORD byte_read;

    total_need = size * maxnum;

    if (!context || context->hidden.windowsio.h == INVALID_HANDLE_VALUE || !total_need) {
        return 0;
    }

    if (context->hidden.windowsio.buffer.left > 0) {
        void *data = (char *) context->hidden.windowsio.buffer.data +
            context->hidden.windowsio.buffer.size -
            context->hidden.windowsio.buffer.left;
        read_ahead =
            SDL_min(total_need, context->hidden.windowsio.buffer.left);
        SDL_memcpy(ptr, data, read_ahead);
        context->hidden.windowsio.buffer.left -= read_ahead;

        if (read_ahead == total_need) {
            return maxnum;
        }
        ptr = (char *) ptr + read_ahead;
        total_need -= read_ahead;
        total_read += read_ahead;
    }

    if (total_need < READAHEAD_BUFFER_SIZE) {
        if (!ReadFile
            (context->hidden.windowsio.h, context->hidden.windowsio.buffer.data,
             READAHEAD_BUFFER_SIZE, &byte_read, NULL)) {
            SDL_Error(SDL_EFREAD);
            return 0;
        }
        read_ahead = SDL_min(total_need, (int) byte_read);
        SDL_memcpy(ptr, context->hidden.windowsio.buffer.data, read_ahead);
        context->hidden.windowsio.buffer.size = byte_read;
        context->hidden.windowsio.buffer.left = byte_read - read_ahead;
        total_read += read_ahead;
    } else {
        if (!ReadFile
            (context->hidden.windowsio.h, ptr, (DWORD)total_need, &byte_read, NULL)) {
            SDL_Error(SDL_EFREAD);
            return 0;
        }
        total_read += byte_read;
    }
    return (total_read / size);
}

static size_t SDLCALL
windows_file_write(SDL_RWops * context, const void *ptr, size_t size,
                 size_t num)
{

    size_t total_bytes;
    DWORD byte_written;
    size_t nwritten;

    total_bytes = size * num;

    if (!context || context->hidden.windowsio.h == INVALID_HANDLE_VALUE || !size || !total_bytes) {
        return 0;
    }

    if (context->hidden.windowsio.buffer.left) {
        SetFilePointer(context->hidden.windowsio.h,
                       -(LONG)context->hidden.windowsio.buffer.left, NULL,
                       FILE_CURRENT);
        context->hidden.windowsio.buffer.left = 0;
    }

    /* if in append mode, we must go to the EOF before write */
    if (context->hidden.windowsio.append) {
        if (SetFilePointer(context->hidden.windowsio.h, 0L, NULL, FILE_END) ==
            INVALID_SET_FILE_POINTER) {
            SDL_Error(SDL_EFWRITE);
            return 0;
        }
    }

    if (!WriteFile
        (context->hidden.windowsio.h, ptr, (DWORD)total_bytes, &byte_written, NULL)) {
        SDL_Error(SDL_EFWRITE);
        return 0;
    }

    nwritten = byte_written / size;
    return nwritten;
}

static int SDLCALL
windows_file_close(SDL_RWops * context)
{

    if (context) {
        if (context->hidden.windowsio.h != INVALID_HANDLE_VALUE) {
            CloseHandle(context->hidden.windowsio.h);
            context->hidden.windowsio.h = INVALID_HANDLE_VALUE;   /* to be sure */
        }
        SDL_free(context->hidden.windowsio.buffer.data);
        context->hidden.windowsio.buffer.data = NULL;
        SDL_FreeRW(context);
    }
    return 0;
}
#endif /* __WIN32__ */

#ifdef __MORPHOS__
static int
amiga_file_open(SDL_RWops *context, const char *filename, const char *mode)
{
    int flag_r, flag_w, flag_p, flag_a;
    int rc = -1;

    /* "r" = reading, file must exist */
    /* "w" = writing, truncate existing, file may not exist */
    /* "r+"= reading or writing, file must exist            */
    /* "a" = writing, append file may not exist             */
    /* "a+"= append + read, file may not exist              */
    /* "w+" = read, write, truncate. file may not exist    */

    flag_r = strchr(mode, 'r') ? 1 : 0;
    flag_w = strchr(mode, 'w') ? 1 : 0;
    flag_p = strchr(mode, '+') ? 1 : 0;
    flag_a = strchr(mode, 'a') ? 1 : 0;

    if (flag_r || flag_w || flag_a)
    {
        size_t mode = MODE_OLDFILE;
        BPTR fh;

        if (flag_a)
        {
            mode = MODE_READWRITE;
        }
        else if (flag_w)
        {
            mode = MODE_NEWFILE;
        }

        fh = Open(filename, mode);

        context->hidden.amigaio.Writable = (flag_w || flag_a || flag_p) ? 1 : 0;
        context->hidden.amigaio.Readable = (flag_r || flag_p) ? 1 : 0;

        context->hidden.amigaio.autoclose = 1;
        context->hidden.amigaio.fp.dos = fh;

        if (fh)
        {
            rc = 0;

            context->hidden.amigaio.AppendMode = 0;
            context->hidden.amigaio.NoSeek = 0;
            context->hidden.amigaio.IsAtEnd = 0;

            if (flag_a)
            {
                context->hidden.amigaio.AppendMode = 1;

                if (!flag_p)
                {
                    context->hidden.amigaio.NoSeek = 1;
                    context->hidden.amigaio.IsAtEnd = 1;
                    Seek(fh, 0, OFFSET_END);
                }
            }
        }
    }

    return rc;
}

static Sint64 SDLCALL
amiga_file_size(SDL_RWops * context)
{
    struct FileInfoBlock fib;

    if (ExamineFH64(context->hidden.amigaio.fp.dos, &fib, TAG_DONE))
        return fib.fib_Size64;

    return -1;
}

static Sint64 SDLCALL
amiga_file_seek(SDL_RWops *context, Sint64 offset, int whence)
{
    Sint64 rc = -1;

    if (!context->hidden.amigaio.NoSeek)
    {
        LONG how = OFFSET_BEGINNING;

        switch (whence)
        {
				case RW_SEEK_SET: how = OFFSET_BEGINNING; break;
            case RW_SEEK_CUR: how = OFFSET_CURRENT; break;
            case RW_SEEK_END: how = OFFSET_END; break;

				default: return SDL_SetError("Unknown value for 'whence'");
        }

        context->hidden.amigaio.IsAtEnd = 0;

        if (Seek64(context->hidden.amigaio.fp.dos, offset, how) == -1)
        {
            SDL_Error(SDL_EFSEEK);
        }
        else
        {
            if (how == OFFSET_END && offset == 0)
                context->hidden.amigaio.IsAtEnd = 1;

            rc = Seek64(context->hidden.amigaio.fp.dos, 0, OFFSET_CURRENT);
        }
    }

    return rc;
}

static size_t SDLCALL
amiga_file_read(SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
{
    size_t rsize = size * maxnum, result;
    //D("[%s]\n", __FUNCTION__);

    if (context->hidden.amigaio.Readable)
    {
        if ((result = Read(context->hidden.amigaio.fp.dos, ptr, rsize)) != rsize)
        {
            SDL_Error(SDL_EFWRITE);
        }
    }
    else
    {
        result = 0;
    }

    return result / size;
}

static size_t SDLCALL
amiga_file_write(SDL_RWops *context, const void *ptr, size_t size, size_t num)
{
    size_t wnum = 0;
    //D("[%s]\n", __FUNCTION__);

    if (context->hidden.amigaio.Writable)
    {
        size_t wsize, result;

        if (context->hidden.amigaio.AppendMode && !context->hidden.amigaio.IsAtEnd)
        {
            if (Seek(context->hidden.amigaio.fp.dos, 0, OFFSET_END) == -1)
            {
                SDL_Error(SDL_EFWRITE);
                return 0;
            }

            context->hidden.amigaio.IsAtEnd = 1;
        }

        wsize = size * num;

        if ((result = Write(context->hidden.amigaio.fp.dos, (APTR)ptr, wsize)) != wsize)
        {
            SDL_Error(SDL_EFWRITE);
        }

        wnum = result / size;
    }

    return wnum;
}

static int SDLCALL
amiga_file_close(SDL_RWops *context)
{
    if (context->hidden.amigaio.fp.dos != 0)
    {
        if (context->hidden.amigaio.autoclose)
            Close(context->hidden.amigaio.fp.dos);

        SDL_FreeRW(context);
    }

    return(0);
}

SDL_RWops * SDL_RWFromFP_clib_REAL(void *fp,
                             int autoclose,
                             Sint64 (*size)(struct SDL_RWops *),
                             Sint64 (*seek)(struct SDL_RWops *, Sint64, int),
                             size_t (*read)(struct SDL_RWops *, void *, size_t, size_t),
                             size_t (*write)(struct SDL_RWops *, const void *, size_t, size_t),
                             int (*close)(struct SDL_RWops *))
{
    SDL_RWops *rwops;
    D("[%s]\n", __FUNCTION__);

    rwops = SDL_AllocRW();
    if ( rwops != NULL ) {
        rwops->size = size;
        rwops->seek = seek;
        rwops->read = read;
        rwops->write = write;
        rwops->close = close;
        rwops->type = SDL_RWOPS_STDFILE;
        rwops->hidden.amigaio.fp.libc = fp;
        rwops->hidden.amigaio.autoclose = autoclose;
    }
    return(rwops);
}
#elif defined(HAVE_STDIO_H)

#ifdef HAVE_FOPEN64
#define fopen   fopen64
#endif
#ifdef HAVE_FSEEKO64
#define fseek_off_t off64_t
#define fseek   fseeko64
#define ftell   ftello64
#elif defined(HAVE_FSEEKO)
#if defined(OFF_MIN) && defined(OFF_MAX)
#define FSEEK_OFF_MIN OFF_MIN
#define FSEEK_OFF_MAX OFF_MAX
#elif defined(HAVE_LIMITS_H)
/* POSIX doesn't specify the minimum and maximum macros for off_t so
 * we have to improvise and dance around implementation-defined
 * behavior. This may fail if the off_t type has padding bits or
 * is not a two's-complement representation. The compilers will detect
 * and eliminate the dead code if off_t has 64 bits.
 */
#define FSEEK_OFF_MAX (((((off_t)1 << (sizeof(off_t) * CHAR_BIT - 2)) - 1) << 1) + 1)
#define FSEEK_OFF_MIN (-(FSEEK_OFF_MAX) - 1)
#endif
#define fseek_off_t off_t
#define fseek   fseeko
#define ftell   ftello
#elif defined(HAVE__FSEEKI64)
#define fseek_off_t __int64
#define fseek   _fseeki64
#define ftell   _ftelli64
#else
#ifdef HAVE_LIMITS_H
#define FSEEK_OFF_MIN LONG_MIN
#define FSEEK_OFF_MAX LONG_MAX
#endif
#define fseek_off_t long
#endif

/* Functions to read/write stdio file pointers */

static Sint64 SDLCALL
stdio_size(SDL_RWops * context)
{
    Sint64 pos, size;

    pos = SDL_RWseek(context, 0, RW_SEEK_CUR);
    if (pos < 0) {
        return -1;
    }
    size = SDL_RWseek(context, 0, RW_SEEK_END);

    SDL_RWseek(context, pos, RW_SEEK_SET);
    return size;
}

static Sint64 SDLCALL
stdio_seek(SDL_RWops * context, Sint64 offset, int whence)
{
    int stdiowhence;

    switch (whence) {
    case RW_SEEK_SET:
        stdiowhence = SEEK_SET;
        break;
    case RW_SEEK_CUR:
        stdiowhence = SEEK_CUR;
        break;
    case RW_SEEK_END:
        stdiowhence = SEEK_END;
        break;
    default:
        return SDL_SetError("Unknown value for 'whence'");
    }

#if defined(FSEEK_OFF_MIN) && defined(FSEEK_OFF_MAX)
    if (offset < (Sint64)(FSEEK_OFF_MIN) || offset > (Sint64)(FSEEK_OFF_MAX)) {
        return SDL_SetError("Seek offset out of range");
    }
#endif

    if (fseek(context->hidden.stdio.fp, (fseek_off_t)offset, stdiowhence) == 0) {
        Sint64 pos = ftell(context->hidden.stdio.fp);
        if (pos < 0) {
            return SDL_SetError("Couldn't get stream offset");
        }
        return pos;
    }
    return SDL_Error(SDL_EFSEEK);
}

static size_t SDLCALL
stdio_read(SDL_RWops * context, void *ptr, size_t size, size_t maxnum)
{
    size_t nread;

    nread = fread(ptr, size, maxnum, context->hidden.stdio.fp);
    if (nread == 0 && ferror(context->hidden.stdio.fp)) {
        SDL_Error(SDL_EFREAD);
    }
    return nread;
}

static size_t SDLCALL
stdio_write(SDL_RWops * context, const void *ptr, size_t size, size_t num)
{
    size_t nwrote;

    nwrote = fwrite(ptr, size, num, context->hidden.stdio.fp);
    if (nwrote == 0 && ferror(context->hidden.stdio.fp)) {
        SDL_Error(SDL_EFWRITE);
    }
    return nwrote;
}

static int SDLCALL
stdio_close(SDL_RWops * context)
{
    int status = 0;
    if (context) {
        if (context->hidden.stdio.autoclose) {
            /* WARNING:  Check the return value here! */
            if (fclose(context->hidden.stdio.fp) != 0) {
                status = SDL_Error(SDL_EFWRITE);
            }
        }
        SDL_FreeRW(context);
    }
    return status;
}
#endif /* !HAVE_STDIO_H */

/* Functions to read/write memory pointers */

static Sint64 SDLCALL
mem_size(SDL_RWops * context)
{
    return (Sint64)(context->hidden.mem.stop - context->hidden.mem.base);
}

static Sint64 SDLCALL
mem_seek(SDL_RWops * context, Sint64 offset, int whence)
{
    Uint8 *newpos;

    switch (whence) {
    case RW_SEEK_SET:
        newpos = context->hidden.mem.base + offset;
        break;
    case RW_SEEK_CUR:
        newpos = context->hidden.mem.here + offset;
        break;
    case RW_SEEK_END:
        newpos = context->hidden.mem.stop + offset;
        break;
    default:
        return SDL_SetError("Unknown value for 'whence'");
    }
    if (newpos < context->hidden.mem.base) {
        newpos = context->hidden.mem.base;
    }
    if (newpos > context->hidden.mem.stop) {
        newpos = context->hidden.mem.stop;
    }
    context->hidden.mem.here = newpos;
    return (Sint64)(context->hidden.mem.here - context->hidden.mem.base);
}

static size_t SDLCALL
mem_read(SDL_RWops * context, void *ptr, size_t size, size_t maxnum)
{
    size_t total_bytes;
    size_t mem_available;

    total_bytes = (maxnum * size);
    if (!maxnum || !size || ((total_bytes / maxnum) != size)) {
        return 0;
    }

    mem_available = (context->hidden.mem.stop - context->hidden.mem.here);
    if (total_bytes > mem_available) {
        total_bytes = mem_available;
    }

    SDL_memcpy(ptr, context->hidden.mem.here, total_bytes);
    context->hidden.mem.here += total_bytes;

    return (total_bytes / size);
}

static size_t SDLCALL
mem_write(SDL_RWops * context, const void *ptr, size_t size, size_t num)
{
    if ((context->hidden.mem.here + (num * size)) > context->hidden.mem.stop) {
        num = (context->hidden.mem.stop - context->hidden.mem.here) / size;
    }
    SDL_memcpy(context->hidden.mem.here, ptr, num * size);
    context->hidden.mem.here += num * size;
    return num;
}

static size_t SDLCALL
mem_writeconst(SDL_RWops * context, const void *ptr, size_t size, size_t num)
{
    SDL_SetError("Can't write to read-only memory");
    return 0;
}

static int SDLCALL
mem_close(SDL_RWops * context)
{
    if (context) {
        SDL_FreeRW(context);
    }
    return 0;
}


/* Functions to create SDL_RWops structures from various data sources */

SDL_RWops *
SDL_RWFromFile(const char *file, const char *mode)
{
    SDL_RWops *rwops = NULL;
    if (!file || !*file || !mode || !*mode) {
        SDL_SetError("SDL_RWFromFile(): No file or no mode specified");
        return NULL;
    }
#if defined(__ANDROID__)
#ifdef HAVE_STDIO_H
    /* Try to open the file on the filesystem first */
    if (*file == '/') {
        FILE *fp = fopen(file, mode);
        if (fp) {
            return SDL_RWFromFP(fp, 1);
        }
    } else {
        /* Try opening it from internal storage if it's a relative path */
        char *path;
        FILE *fp;

        /* !!! FIXME: why not just "char path[PATH_MAX];" ? */
        path = SDL_stack_alloc(char, PATH_MAX);
        if (path) {
            SDL_snprintf(path, PATH_MAX, "%s/%s",
                         SDL_AndroidGetInternalStoragePath(), file);
            fp = fopen(path, mode);
            SDL_stack_free(path);
            if (fp) {
                return SDL_RWFromFP(fp, 1);
            }
        }
    }
#endif /* HAVE_STDIO_H */

    /* Try to open the file from the asset system */
    rwops = SDL_AllocRW();
    if (!rwops)
        return NULL;            /* SDL_SetError already setup by SDL_AllocRW() */
    if (Android_JNI_FileOpen(rwops, file, mode) < 0) {
        SDL_FreeRW(rwops);
        return NULL;
    }
    rwops->size = Android_JNI_FileSize;
    rwops->seek = Android_JNI_FileSeek;
    rwops->read = Android_JNI_FileRead;
    rwops->write = Android_JNI_FileWrite;
    rwops->close = Android_JNI_FileClose;
    rwops->type = SDL_RWOPS_JNIFILE;

#elif defined(__WIN32__)
    rwops = SDL_AllocRW();
    if (!rwops)
        return NULL;            /* SDL_SetError already setup by SDL_AllocRW() */
    if (windows_file_open(rwops, file, mode) < 0) {
        SDL_FreeRW(rwops);
        return NULL;
    }
    rwops->size = windows_file_size;
    rwops->seek = windows_file_seek;
    rwops->read = windows_file_read;
    rwops->write = windows_file_write;
    rwops->close = windows_file_close;
    rwops->type = SDL_RWOPS_WINFILE;

#elif defined(__MORPHOS__)
    rwops = SDL_AllocRW();
    if (!rwops)
        return NULL; /* SDL_SetError already setup by SDL_AllocRW() */

    rwops->size  = amiga_file_size;
    rwops->seek  = amiga_file_seek;
    rwops->read  = amiga_file_read;
    rwops->write = amiga_file_write;
    rwops->close = amiga_file_close;
    rwops->type = SDL_RWOPS_AMIGAFILE;

    char *mpath = AMIGA_ConvertPath(file);
    int rc = -1;

    if (mpath)
    {
        rc = amiga_file_open(rwops,file,mode);
        SDL_free(mpath);
    }

    if (rc < 0)
    {
        SDL_FreeRW(rwops);
        return NULL;
    }
#elif defined(__VITA__)
        rwops = SDL_AllocRW();
        if (!rwops)
            return NULL;            /* SDL_SetError already setup by SDL_AllocRW() */
        if (vita_file_open(rwops, file, mode) < 0) {
            SDL_FreeRW(rwops);
            return NULL;
        }
        rwops->size = vita_file_size;
        rwops->seek = vita_file_seek;
        rwops->read = vita_file_read;
        rwops->write = vita_file_write;
        rwops->close = vita_file_close;
        rwops->type = SDL_RWOPS_VITAFILE;
#elif HAVE_STDIO_H
    {
        #ifdef __APPLE__
        FILE *fp = SDL_OpenFPFromBundleOrFallback(file, mode);
        #elif __WINRT__
        FILE *fp = NULL;
        fopen_s(&fp, file, mode);
        #else
        FILE *fp = fopen(file, mode);
        #endif
        if (fp == NULL) {
            SDL_SetError("Couldn't open %s", file);
        } else {
            rwops = SDL_RWFromFP(fp, SDL_TRUE);
        }
    }
#else
    SDL_SetError("SDL not compiled with stdio support");
#endif /* !HAVE_STDIO_H */

    return rwops;
}

#if defined(HAVE_STDIO_H) && !defined(__MORPHOS__)
SDL_RWops *
SDL_RWFromFP(FILE * fp, SDL_bool autoclose)
{
    SDL_RWops *rwops = NULL;

    rwops = SDL_AllocRW();
    if (rwops != NULL) {
        rwops->size = stdio_size;
        rwops->seek = stdio_seek;
        rwops->read = stdio_read;
        rwops->write = stdio_write;
        rwops->close = stdio_close;
        rwops->hidden.stdio.fp = fp;
        rwops->hidden.stdio.autoclose = autoclose;
        rwops->type = SDL_RWOPS_STDFILE;
    }
    return rwops;
}
#else
SDL_RWops *
SDL_RWFromFP(void * fp, SDL_bool autoclose)
{
    SDL_SetError("SDL not compiled with stdio support");
    return NULL;
}
#endif /* HAVE_STDIO_H */

SDL_RWops *
SDL_RWFromMem(void *mem, int size)
{
    SDL_RWops *rwops = NULL;
    if (!mem) {
      SDL_InvalidParamError("mem");
      return rwops;
    }
    if (!size) {
      SDL_InvalidParamError("size");
      return rwops;
    }

    rwops = SDL_AllocRW();
    if (rwops != NULL) {
        rwops->size = mem_size;
        rwops->seek = mem_seek;
        rwops->read = mem_read;
        rwops->write = mem_write;
        rwops->close = mem_close;
        rwops->hidden.mem.base = (Uint8 *) mem;
        rwops->hidden.mem.here = rwops->hidden.mem.base;
        rwops->hidden.mem.stop = rwops->hidden.mem.base + size;
        rwops->type = SDL_RWOPS_MEMORY;
    }
    return rwops;
}

SDL_RWops *
SDL_RWFromConstMem(const void *mem, int size)
{
    SDL_RWops *rwops = NULL;
    if (!mem) {
      SDL_InvalidParamError("mem");
      return rwops;
    }
    if (!size) {
      SDL_InvalidParamError("size");
      return rwops;
    }

    rwops = SDL_AllocRW();
    if (rwops != NULL) {
        rwops->size = mem_size;
        rwops->seek = mem_seek;
        rwops->read = mem_read;
        rwops->write = mem_writeconst;
        rwops->close = mem_close;
        rwops->hidden.mem.base = (Uint8 *) mem;
        rwops->hidden.mem.here = rwops->hidden.mem.base;
        rwops->hidden.mem.stop = rwops->hidden.mem.base + size;
        rwops->type = SDL_RWOPS_MEMORY_RO;
    }
    return rwops;
}

SDL_RWops *
SDL_AllocRW(void)
{
    SDL_RWops *area;

    area = (SDL_RWops *) SDL_malloc(sizeof *area);
    if (area == NULL) {
        SDL_OutOfMemory();
    } else {
#if defined(__MORPHOS__)
        register APTR DataSeg __asm("r13");
        area->r13 = DataSeg;
#endif
        area->type = SDL_RWOPS_UNKNOWN;
    }
    return area;
}

void
SDL_FreeRW(SDL_RWops * area)
{
    SDL_free(area);
}

/* Load all the data from an SDL data stream */
void *
SDL_LoadFile_RW(SDL_RWops * src, size_t *datasize, int freesrc)
{
    const int FILE_CHUNK_SIZE = 1024;
    Sint64 size;
    size_t size_read, size_total;
    void *data = NULL, *newdata;

    if (!src) {
        SDL_InvalidParamError("src");
        return NULL;
    }

    size = SDL_RWsize(src);
    if (size < 0) {
        size = FILE_CHUNK_SIZE;
    }
    data = SDL_malloc((size_t)(size + 1));

    size_total = 0;
    for (;;) {
        if ((((Sint64)size_total) + FILE_CHUNK_SIZE) > size) {
            size = (size_total + FILE_CHUNK_SIZE);
            newdata = SDL_realloc(data, (size_t)(size + 1));
            if (!newdata) {
                SDL_free(data);
                data = NULL;
                SDL_OutOfMemory();
                goto done;
            }
            data = newdata;
        }

        size_read = SDL_RWread(src, (char *)data+size_total, 1, (size_t)(size-size_total));
        if (size_read == 0) {
            break;
        }
        size_total += size_read;
    }

    if (datasize) {
        *datasize = size_total;
    }
    ((char *)data)[size_total] = '\0';

done:
    if (freesrc && src) {
        SDL_RWclose(src);
    }
    return data;
}

void *
SDL_LoadFile(const char *file, size_t *datasize)
{
   return SDL_LoadFile_RW(SDL_RWFromFile(file, "rb"), datasize, 1);
}

Sint64
SDL_RWsize(SDL_RWops *context)
{
    return context->size(context);
}

Sint64
SDL_RWseek(SDL_RWops *context, Sint64 offset, int whence)
{
    return context->seek(context, offset, whence);
}

Sint64
SDL_RWtell(SDL_RWops *context)
{
    return context->seek(context, 0, RW_SEEK_CUR);
}

size_t
SDL_RWread(SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
{
    return context->read(context, ptr, size, maxnum);
}

size_t
SDL_RWwrite(SDL_RWops *context, const void *ptr, size_t size, size_t num)
{
    return context->write(context, ptr, size, num);
}

int
SDL_RWclose(SDL_RWops *context)
{
    return context->close(context);
}

/* Functions for dynamically reading and writing endian-specific values */

Uint8
SDL_ReadU8(SDL_RWops * src)
{
    Uint8 value = 0;

    SDL_RWread(src, &value, sizeof (value), 1);
    return value;
}

Uint16
SDL_ReadLE16(SDL_RWops * src)
{
    Uint16 value = 0;

    SDL_RWread(src, &value, sizeof (value), 1);
    return SDL_SwapLE16(value);
}

Uint16
SDL_ReadBE16(SDL_RWops * src)
{
    Uint16 value = 0;

    SDL_RWread(src, &value, sizeof (value), 1);
    return SDL_SwapBE16(value);
}

Uint32
SDL_ReadLE32(SDL_RWops * src)
{
    Uint32 value = 0;

    SDL_RWread(src, &value, sizeof (value), 1);
    return SDL_SwapLE32(value);
}

Uint32
SDL_ReadBE32(SDL_RWops * src)
{
    Uint32 value = 0;

    SDL_RWread(src, &value, sizeof (value), 1);
    return SDL_SwapBE32(value);
}

Uint64
SDL_ReadLE64(SDL_RWops * src)
{
    Uint64 value = 0;

    SDL_RWread(src, &value, sizeof (value), 1);
    return SDL_SwapLE64(value);
}

Uint64
SDL_ReadBE64(SDL_RWops * src)
{
    Uint64 value = 0;

    SDL_RWread(src, &value, sizeof (value), 1);
    return SDL_SwapBE64(value);
}

size_t
SDL_WriteU8(SDL_RWops * dst, Uint8 value)
{
    return SDL_RWwrite(dst, &value, sizeof (value), 1);
}

size_t
SDL_WriteLE16(SDL_RWops * dst, Uint16 value)
{
    const Uint16 swapped = SDL_SwapLE16(value);
    return SDL_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
SDL_WriteBE16(SDL_RWops * dst, Uint16 value)
{
    const Uint16 swapped = SDL_SwapBE16(value);
    return SDL_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
SDL_WriteLE32(SDL_RWops * dst, Uint32 value)
{
    const Uint32 swapped = SDL_SwapLE32(value);
    return SDL_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
SDL_WriteBE32(SDL_RWops * dst, Uint32 value)
{
    const Uint32 swapped = SDL_SwapBE32(value);
    return SDL_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
SDL_WriteLE64(SDL_RWops * dst, Uint64 value)
{
    const Uint64 swapped = SDL_SwapLE64(value);
    return SDL_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

size_t
SDL_WriteBE64(SDL_RWops * dst, Uint64 value)
{
    const Uint64 swapped = SDL_SwapBE64(value);
    return SDL_RWwrite(dst, &swapped, sizeof (swapped), 1);
}

/* vi: set ts=4 sw=4 expandtab: */
