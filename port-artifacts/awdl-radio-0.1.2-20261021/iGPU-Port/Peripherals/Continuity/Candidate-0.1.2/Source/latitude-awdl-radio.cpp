#include <IOKit/IOKitLib.h>
#include <mach/mach.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <ctime>
#include "LatitudeAWDLEngineBridge.h"
using namespace LatitudeAWDLRadio;
struct RadioConnection {
 io_connect_t handle=IO_OBJECT_NULL;
 ~RadioConnection(){if(handle)IOServiceClose(handle);}
 bool open(){
  io_iterator_t iterator=IO_OBJECT_NULL;auto matching=IOServiceMatching("AirportItlwm");
  auto rc=IOServiceGetMatchingServices(kIOMainPortDefault,matching,&iterator);if(rc)return false;
  unsigned matches=0;io_service_t found=0,service=0;
  while((service=IOIteratorNext(iterator))){++matches;if(!found)found=service;else IOObjectRelease(service);}
  IOObjectRelease(iterator);if(matches!=1){if(found)IOObjectRelease(found);fprintf(stderr,"Expected exactly one AirportItlwm controller; found %u.\n",matches);return false;}
  rc=IOServiceOpen(found,mach_task_self(),ConnectionType(),&handle);IOObjectRelease(found);
  if(rc){fprintf(stderr,"Radio bridge open failed: 0x%08x. Candidate 0.1.2 must be loaded; administrator access is required.\n",rc);return false;}return true;
 }
 static uint32_t ConnectionType(){return LatitudeAWDLRadio::Connection;}
 kern_return_t query(Status &s){size_t n=sizeof(s);auto rc=IOConnectCallStructMethod(handle,Query,nullptr,0,&s,&n);return rc?rc:(n==sizeof(s)&&s.version==Version?KERN_SUCCESS:kIOReturnBadArgument);}
 kern_return_t receive(Received &f){size_t n=sizeof(f);auto rc=IOConnectCallStructMethod(handle,ReceiveAction,nullptr,0,&f,&n);return rc?rc:(n==sizeof(f)&&f.version==Version?KERN_SUCCESS:kIOReturnBadArgument);}
 kern_return_t transmit(const uint8_t*frame,size_t n,uint64_t &error){uint32_t count=1;auto rc=IOConnectCallMethod(handle,TransmitAction,nullptr,0,frame,n,&error,&count,nullptr,nullptr);return rc?rc:(count==1?KERN_SUCCESS:kIOReturnBadArgument);}
};
static void print(const Status&s){
 printf("protocol=0x%x device=0x%04x associated=%u txArmed=%u frequency=%u\n",s.version,s.device,!!(s.flags&Associated),!!(s.flags&TransmitArmed),s.frequency);
 printf("submitted=%llu responses=%llu firmwareSuccess=%llu firmwareFailure=%llu reclaimed=%llu resetAborted=%llu missingResponse=%llu lastCookie=%llu lastStatus=%u\n",s.submitted,s.responses,s.firmwareSuccess,s.firmwareFailure,s.reclaimed,s.aborted,s.missingResponse,s.lastCookie,s.lastStatus);
 printf("rxAccepted=%llu rxDropped=%llu rxQueued=%u\n",s.received,s.dropped,s.queued);fflush(stdout);
}
static void usage(){fprintf(stderr,"Usage: latitude-awdl-radio status | listen SECONDS | send-psf --transmit-current-channel | send-mif --transmit-current-channel\n");}
int main(int argc,char**argv){
 if(argc<2){usage();return 2;}
 bool status=!strcmp(argv[1],"status"),listen=!strcmp(argv[1],"listen"),psf=!strcmp(argv[1],"send-psf"),mif=!strcmp(argv[1],"send-mif");
 if(!status&&!listen&&!psf&&!mif){usage();return 2;}
 unsigned seconds=0;
 if(listen){char*end=nullptr;errno=0;unsigned long value=argc==3?strtoul(argv[2],&end,10):0;if(errno||!end||*end||value<1||value>300){usage();return 2;}seconds=(unsigned)value;}
 else if(status?argc!=2:(argc!=3||strcmp(argv[2],"--transmit-current-channel"))){usage();return 2;}
 RadioConnection connection;if(!connection.open())return 1;Status s;
 auto rc=connection.query(s);if(rc){fprintf(stderr,"Status failed: 0x%08x\n",rc);return 1;}print(s);if(status)return 0;
 LatitudeAWDLEngineBridge engine;if(!engine.configure(s)){fprintf(stderr,"Unsupported hardware, missing association or unsupported current channel.\n");return 1;}
 if(psf||mif){
  if(!(s.flags&TransmitArmed)){fprintf(stderr,"Transmit boot gate is disabled. No frame sent.\n");return 1;}
  uint8_t frame[12+MaxFrame]={};size_t n=engine.action(frame,sizeof(frame),mif);if(!n){fprintf(stderr,"Frame validation failed.\n");return 1;}
  uint64_t error=0;rc=connection.transmit(frame,n,error);
  printf("submitIOReturn=0x%08x submitErrno=%llu\n",rc,error);
  if(rc||error)return 1;
  // Poll correlated firmware outcome for up to two seconds; queue acceptance
  // alone is never labelled RF success. No automatic retransmission.
  auto before=s;for(unsigned i=0;i<100;i++){usleep(20000);rc=connection.query(s);if(rc)break;if(s.responses>before.responses||s.aborted>before.aborted)break;}
  if(rc){fprintf(stderr,"Completion query failed: 0x%08x\n",rc);return 1;}print(s);
  if(s.responses==before.responses){fprintf(stderr,"No firmware completion observed; transmission unverified.\n");return 3;}
  printf("Firmware response observed; remote reception and AirDrop are not established.\n");return s.firmwareFailure>before.firmwareFailure?1:0;
 }
 timespec start{};clock_gettime(CLOCK_MONOTONIC,&start);uint64_t nextQuery=0;
 for(;;){
  timespec now{};clock_gettime(CLOCK_MONOTONIC,&now);uint64_t elapsed=(uint64_t)(now.tv_sec-start.tv_sec)*1000+(now.tv_nsec-start.tv_nsec)/1000000;
  if(elapsed>=(uint64_t)seconds*1000)break;
  if(elapsed>=nextQuery){rc=connection.query(s);if(rc||!engine.configure(s)){fprintf(stderr,"Radio identity or association changed; stopping.\n");return 1;}nextQuery=elapsed+1000;}
  Received f;rc=connection.receive(f);
  if(rc==kIOReturnNotFound){usleep(20000);continue;}
  if(rc){fprintf(stderr,"Receive failed: 0x%08x\n",rc);return 1;}
  auto parsed=engine.receive(f);printf("rxSequence=%llu bytes=%u frequency=%u parse=%d peers=%d\n",f.sequence,f.length,f.frequency,parsed,awdl_peers_length(engine.state.peers.peers));fflush(stdout);
 }
 if(!connection.query(s))print(s);printf("Passive listener finished; no radio frames were transmitted.\n");return 0;
}
