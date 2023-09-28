/* Copyright (C) 2023 John Törnblom

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.  */

#include "payload.h"
#include "syscall.h"


/**
 * Dependencies provided by the ELF linker.
 **/
extern void (*__init_array_start[])(payload_args_t*) __attribute__((weak));
extern void (*__init_array_end[])(payload_args_t*) __attribute__((weak));

extern void (*__fini_array_start[])(void) __attribute__((weak));
extern void (*__fini_array_end[])(void) __attribute__((weak));

extern unsigned char __bss_start[] __attribute__((weak));
extern unsigned char __bss_end[] __attribute__((weak));


/**
 * Entry point to the main program.
 **/
extern int main(int argc, char* argv[], char *envp[]);


/**
 * For error reporting to /dev/klog and /dev/stdout
 **/
static char* (*strerror)(int) = 0;
static void  (*printf)(const char*, ...) = 0;
static void
kerror(const char *s, int error) {
  printf("%s: %s\n", s, strerror(error));

  syscall(0x259, 7, "<118>[homebrew] ", 0);
  syscall(0x259, 7, s, 0);
  syscall(0x259, 7, ": ", 0);
  syscall(0x259, 7, strerror(error), 0);
  syscall(0x259, 7, "\n", 0);
}


/**
 * Entry-point used by the ELF loader.
 **/
void
_start(payload_args_t *args) {
  unsigned long count;

  for(unsigned char* bss=__bss_start; bss<__bss_end; bss++) {
    *bss = 0;
  }

  if((*args->payloadout=args->sceKernelDlsym(0x2, "strerror", &strerror))) {
    return;
  }

  if((*args->payloadout=args->sceKernelDlsym(0x2, "printf", &printf))) {
    return;
  }

  // run module constructors
  count = __init_array_end - __init_array_start;
  for(int i=0; i<count; i++) {
    __init_array_start[i](args);
    if(*args->payloadout) {
      kerror("Unable to initialize payload", *args->payloadout);
      break;
    }
  }

  if(!*args->payloadout) {
    *args->payloadout = main(0, 0, 0);
  }

  // run module destructors
  count = __fini_array_end - __fini_array_start;
  for(int i=0; i<count; i++) {
    __fini_array_start[count-i-1]();
  }
}
