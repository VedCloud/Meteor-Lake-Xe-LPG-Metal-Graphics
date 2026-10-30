/* SPDX-License-Identifier: MIT */
#include "glue.h"
#include <Protocol/SimpleFileSystem.h>
typedef struct {EFI_FILE_PROTOCOL*file;uint64_t bytes;} UtilityFile;
EFI_STATUS utility_file_open(EFI_HANDLE,EFI_SYSTEM_TABLE*,UtilityFile*);
int utility_file_transfer(void*,uint64_t,void*,unsigned,bool);
void utility_file_close(UtilityFile*);
