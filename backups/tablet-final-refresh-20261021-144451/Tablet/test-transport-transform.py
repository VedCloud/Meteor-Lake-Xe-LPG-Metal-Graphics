from pathlib import Path
import tempfile,subprocess
root=Path(__file__).resolve().parent
source=r'''
#include "latitude_ishtp_frame.h"
#include "LatitudeTouchTransform.h"
#include <cassert>
#include <vector>
#include <climits>
using namespace LatitudeISH;
static std::vector<uint8_t> packet(unsigned n,bool done=true,unsigned host=2){std::vector<uint8_t>p(n+4);p[0]=1;p[1]=host;p[2]=n&255;p[3]=(n>>8)|(done?128:0);return p;}
int main(){
 Reassembly r;auto a=packet(100,false),b=packet(42,true);assert(r.feed(a.data(),a.size())==0);assert(r.feed(b.data(),b.size())==1&&r.size()==142);assert(r.feed(b.data(),b.size())==1&&r.size()==42);r.reset();
 a=packet(124,false);for(int i=0;i<66;i++)assert(r.feed(a.data(),a.size())==0);assert(r.feed(a.data(),a.size())==-1&&r.size()==0);
 a=packet(50,false);b=packet(50,true,3);assert(r.feed(a.data(),a.size())==0);assert(r.feed(b.data(),b.size())==-1&&r.size()==0);
 a=packet(1);a[3]|=2;assert(r.feed(a.data(),a.size())==-1);a=packet(1);a[2]=2;assert(r.feed(a.data(),a.size())==-1);
 uint8_t wire[136]={3};int32_t expected[3]={-30043,-708046,-712327},axes[3];
 for(unsigned axis=0;axis<3;axis++)for(unsigned byte=0;byte<4;byte++)wire[27+axis*4+byte]=((uint32_t)expected[axis]>>(8*byte))&255;
 assert(accelerationReport(wire,sizeof wire,axes));for(int i=0;i<3;i++)assert(axes[i]==expected[i]);assert(!accelerationReport(wire,135,axes));wire[0]=4;assert(!accelerationReport(wire,136,axes));
 uint8_t host[9]={5,6,0,0,3,0,1,2,3};HostMessage m;assert(hostMessage(host,9,&m)&&m.command==5&&m.length==3);host[4]=4;assert(!hostMessage(host,9,&m));
 using namespace LatitudeTablet;Point p;
 // Rectangular and nonzero-origin axes, all four corners in each rotation.
 for(unsigned rot=0;rot<4;rot++)for(int x:{10,1930})for(int y:{20,1100}){
  assert(touchTransform(x,y,10,1930,20,1100,rot,&p));assert((p.x==0||p.x==65535)&&(p.y==0||p.y==65535));
  Point back;assert(touchTransform(p.x,p.y,0,65535,0,65535,(4-rot)%4,&back));assert(back.x==(x==10?0u:65535u)&&back.y==(y==20?0u:65535u));
 }
 assert(touchTransform(10,20,10,1930,20,1100,1,&p)&&p.x==65535&&p.y==0);
 assert(!touchTransform(9,20,10,1930,20,1100,0,&p));assert(!touchTransform(0,0,0,0,0,1,0,&p));assert(!touchTransform(0,0,0,1,0,1,4,&p));
 assert(touchTransform(INT_MAX,INT_MIN,INT_MIN,INT_MAX,INT_MIN,INT_MAX,2,&p)&&p.x==0&&p.y==65535);
}
'''
with tempfile.TemporaryDirectory() as tmp:
 p=Path(tmp);(p/'test.cpp').write_text(source)
 subprocess.run(['xcrun','clang++','-std=c++17','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(root),str(p/'test.cpp'),'-o',str(p/'test')],check=True);subprocess.run([str(p/'test')],check=True)
print('PASS bounded ISHTP fragment framing/reassembly, invalid/overflow/cross-client rejection, descriptor-specific signed XYZ decoding and normalized translated touch transforms at all corners, nonzero origins and integer limits. No hardware writes or rotation.')
