#pragma once
#include <sys/stat.h>
#include <sys/sysctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#ifndef NVMTL_DESKTOP_JOURNAL_PATH
#define NVMTL_DESKTOP_JOURNAL_PATH "/Library/Logs/LatitudeDesktopAttempt.trace"
#endif
#define NVMTL_DESKTOP_JOURNAL_MAX (256u*1024u)
static inline int nvmtl_journal_flush(int fd)
{
    if(fcntl(fd,F_FULLFSYNC)==0)return 0;
    return fsync(fd);
}
static inline int nvmtl_journal_append(const char *path,const char *phase,const char *detail,int result)
{
    int fd=open(path,O_WRONLY|O_APPEND|O_NOFOLLOW|O_CLOEXEC);
    if(fd<0)return errno;
    struct stat st;
    if(fstat(fd,&st)||!S_ISREG(st.st_mode)||st.st_nlink!=1||(st.st_mode&0022)||
        (geteuid()!=0&&st.st_uid!=geteuid())||st.st_size>=NVMTL_DESKTOP_JOURNAL_MAX){close(fd);return EPERM;}
    char boot[96]="unavailable";size_t size=sizeof boot;
    if(sysctlbyname("kern.bootsessionuuid",boot,&size,NULL,0))strcpy(boot,"unavailable");
    boot[sizeof boot-1]=0;
    uint64_t tid=0;pthread_threadid_np(NULL,&tid);
    char line[896];
    int n=snprintf(line,sizeof line,"epoch=%lld mono=%llu boot=%s pid=%d tid=%llu phase=%.64s result=%d detail=%.480s\n",
        (long long)time(NULL),(unsigned long long)clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW),boot,getpid(),
        (unsigned long long)tid,phase?phase:"?",result,detail?detail:"");
    if(n<0){close(fd);return EINVAL;}
    for(int i=0;i<n-1 && i<(int)sizeof line-1;i++)if(line[i]=='\n'||line[i]=='\r')line[i]=' ';
    size_t bytes=(size_t)n<sizeof line?(size_t)n:sizeof line-1;
    ssize_t wrote;
    do{wrote=write(fd,line,bytes);}while(wrote<0&&errno==EINTR);
    int error=wrote==(ssize_t)bytes?0:(wrote<0?errno:EIO);
    if(!error&&nvmtl_journal_flush(fd))error=errno;
    close(fd);return error;
}
