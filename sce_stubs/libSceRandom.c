/*
 This file was generated by trampgen.py, do not edit manually!
*/

int sprx_dlsym(unsigned int handle, const char *nid, void *addr);
int sprx_dlopen(const char* libname, unsigned short *handle);
int sprx_dlclose(unsigned short handle);

static __attribute__ ((used)) void* __ptr_sceRandomGetRandomNumber;
asm(".intel_syntax noprefix\n"
    ".global sceRandomGetRandomNumber\n"
    ".type sceRandomGetRandomNumber @function\n"
    "sceRandomGetRandomNumber:\n"
    "jmp qword ptr [rip + __ptr_sceRandomGetRandomNumber]\n");

static unsigned short __handle = 0;
static void __attribute__((constructor(104)))
__constructor(void) {
  if(sprx_dlopen("libSceRandom", &__handle)) return;
  if(sprx_dlsym(__handle, "PI7jIZj4pcE", &__ptr_sceRandomGetRandomNumber)) return;
}

static void __attribute__((destructor(104)))
__destructor(void) {
  sprx_dlclose(__handle);
}
