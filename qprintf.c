/* $Id: qprintf.c,v 1.2 2004/04/08 10:48:25 reinelt Exp $
 *
 * simple but quick snprintf() replacement
 *
 * Copyright 2004 Michael Reinelt <reinelt@eunet.at>
 * Copyright 2004 The LCD4Linux Team <lcd4linux-devel@users.sourceforge.net>
 *
 * derived from a patch from Martin Hejl which is
 * Copyright 2003 Martin Hejl (martin@hejl.de)
 *
 * This file is part of LCD4Linux.
 *
 * LCD4Linux is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * LCD4Linux is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * $Log: qprintf.c,v $
 * Revision 1.2  2004/04/08 10:48:25  reinelt
 * finished plugin_exec
 * modified thread handling
 * added '%x' format to qprintf (hexadecimal)
 *
 * Revision 1.1  2004/02/27 07:06:26  reinelt
 * new function 'qprintf()' (simple but quick snprintf() replacement)
 *
 */

/* 
 * exported functions:
 * 
 * int qprintf(char *str, size_t size, const char *format, ...)
 *   works like snprintf(), but format only knows about %d and %s
 */


#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static char *itoa(char* buffer, size_t size, int value)
{
  char *p;
  int sign;
 
  // sanity checks
  if (buffer==NULL || size<2) return (NULL);
  
  // remember sign of value
  sign = 0;
  if (value < 0) {
    sign  = 1;
    value = -value;
  }
  
  // p points to last char
  p = buffer+size-1;
  
  // set terminating zero
  *p='\0';
  
  do {
    *--p = value%10 + '0';
    value = value/10;
  } while (value!=0 && p>buffer);
  
  if (sign && p>buffer)
    *--p = '-';
  
  return p;
} 


static char *itox(char* buffer, size_t size, unsigned int value)
{
  char *p;
  int digit;
 
  // sanity checks
  if (buffer==NULL || size<2) return (NULL);
  
  // p points to last char
  p = buffer+size-1;
  
  // set terminating zero
  *p='\0';
  
  do {
    digit = value%16;
    value = value/16;
    *--p = (digit < 10 ? '0' : 'a'-10) + digit;
  } while (value!=0 && p>buffer);
  
  return p;
} 


int qprintf(char *str, size_t size, const char *format, ...) {

  va_list ap;
  const char *src;
  char *dst;
  int len;
  
  src = format;
  dst = str;
  len = 0;
  
  // leave room for terminating zero 
  size--;
  
  va_start(ap, format);
  
  while (len < size) {
    
    if (*src=='%') {
      char buf[12], *s;
      int d;
      unsigned int u;
      switch (*++src) {
      case 's':
	src++;
	s = va_arg(ap, char *);
	while (len < size && *s != '\0') {
	  len++;
	  *dst++ = *s++;
	}
	break;
      case 'd':
	src++;
	d = va_arg(ap, int);
	s = itoa (buf, sizeof(buf), d);
	while (len < size && *s != '\0') {
	  len++;
	  *dst++ = *s++;
	}
	break;
      case 'x':
	src++;
	u = va_arg(ap, unsigned int);
	s = itox (buf, sizeof(buf), u);
	while (len < size && *s != '\0') {
	  len++;
	  *dst++ = *s++;
	}
	break;
      default:
	len++;
	*dst++ = '%';
      }
    } else {
      len++;
      *dst++ = *src;
      if (*src++ == '\0') break;
    }
  }
  
  va_end(ap);
  
  // enforce terminating zero
  if (len>=size && *(dst-1)!='\0') {
    len++;
    *dst='\0';
  }
  
  // do not count terminating zero
  return len-1;
}

