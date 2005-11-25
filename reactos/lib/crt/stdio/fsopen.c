/*
 * COPYRIGHT:   See COPYING in the top level directory
 * PROJECT:     ReactOS system libraries
 * FILE:        lib/msvcrt/stdio/fsopen.c
 * PURPOSE:     Checks for keyboard hits
 * PROGRAMER:   Boudewijn Dekker
 * UPDATE HISTORY:
 *              28/12/98: Created
 */
/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */

#include "precomp.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <share.h>
#include <tchar.h>


/*
 * @implemented
 */
FILE* _tfsopen(const _TCHAR *file, const _TCHAR *mode, int shflag)
{
  FILE *f;
  int fd, rw, oflags = 0;
  _TCHAR tbchar;

  int shf;

  if (file == 0)
    return 0;
  if (mode == 0)
    return 0;

  f = __alloc_file();
  if (f == NULL)
    return NULL;

  rw = (mode[1] == '+') || (mode[1] && (mode[2] == '+'));

  switch (*mode)
  {
  case 'a':
    oflags = O_CREAT | (rw ? O_RDWR : O_WRONLY);
    break;
  case 'r':
    oflags = rw ? O_RDWR : O_RDONLY;
    break;
  case 'w':
    oflags = O_TRUNC | O_CREAT | (rw ? O_RDWR : O_WRONLY);
    break;
  default:
    return (NULL);
  }
  if (mode[1] == '+')
    tbchar = mode[2];
  else
    tbchar = mode[1];
  if (tbchar == 't')
    oflags |= O_TEXT;
  else if (tbchar == 'b')
    oflags |= O_BINARY;
  else
    oflags |= (_fmode& (O_TEXT|O_BINARY));

  if ( shflag == _SH_DENYNO )
    shf = _S_IREAD | _S_IWRITE;
  else if( shflag == _SH_DENYRD )
    shf =  _S_IWRITE;
  else if( shflag == _SH_DENYRW )
    shf =  0;
  else if( shflag == _SH_DENYWR )
    shf =  _S_IREAD;
  else
    shf = _S_IREAD | _S_IWRITE;

  fd = _topen(file, oflags, shf);
  if (fd < 0)
    return NULL;

// msvcrt ensures that writes will end up at the end of file in append mode
// we just move the file pointer to the end of file initially
  if (*mode == 'a')
    _lseek(fd, 0, SEEK_END);

  f->_cnt = 0;
  f->_file = fd;
  f->_bufsiz = 0;
  if (rw)
    f->_flag = _IOREAD | _IOWRT;
  else if (*mode == 'r')
    f->_flag = _IOREAD;
  else
    f->_flag = _IOWRT;

  f->_base = f->_ptr = NULL;
  return f;
}

