#include "../../latitude_colour_math.h"
class IOAccelDisplayPipeGammaTable2 {
public:
 unsigned getTableCount() const;
 void getTableEntry(unsigned,float *,float *,float *) const;
};
struct LatitudeColourMatrix {uint32_t bits[3][4];};
extern "C" void _ZNK29IOAccelDisplayPipeGammaTable213getTableEntryEjPfS0_S0_(const void *,unsigned,float *,float *,float *);
static bool latitudeColourEntryABI(){
 static const unsigned char code[]={0x55,0x48,0x89,0xe5,0x48,0x8b,0x47,0x10,0x89,0xf6,0x48,0xc1,0xe6,0x04,0xf3,0x0f,0x10,0x44,0x30,0x10,0xf3,0x0f,0x11,0x02,0xf3,0x0f,0x10,0x44,0x30,0x14,0xf3,0x0f,0x11,0x01,0xf3,0x0f,0x10,0x44,0x30,0x18,0xf3,0x41,0x0f,0x11,0x00,0x5d,0xc3};
 auto actual=(const volatile unsigned char *)(uintptr_t)&_ZNK29IOAccelDisplayPipeGammaTable213getTableEntryEjPfS0_S0_;
 for(unsigned i=0;i<sizeof code;i++)if(actual[i]!=code[i])return false;return true;
}
static void latitudeColourEntry(const IOAccelDisplayPipeGammaTable2 *table,unsigned index,uint32_t rgb[3]){
 // The verified getter uses only legacy MOVSS/XMM0. Preserve legacy FP state
 // with interrupts off for this single nonblocking getter, including CR0.TS.
 alignas(16) unsigned char state[512];
 IOInterruptState irq=ml_set_interrupts_enabled(false);uintptr_t cr0;
 __asm__ __volatile__("mov %%cr0,%0":"=r"(cr0));
 if(cr0&8){uintptr_t ready=cr0&~(uintptr_t)8;__asm__ __volatile__("mov %0,%%cr0"::"r"(ready):"memory");}
 else __asm__ __volatile__("fxsave64 (%0)"::"r"(state):"memory");
 table->getTableEntry(index,(float *)&rgb[0],(float *)&rgb[1],(float *)&rgb[2]);
 if(cr0&8)__asm__ __volatile__("mov %0,%%cr0"::"r"(cr0):"memory");
 else __asm__ __volatile__("fxrstor64 (%0)"::"r"(state):"memory");
 ml_set_interrupts_enabled(irq);
}
static bool latitudeColourTable(const IOAccelDisplayPipeGammaTable2 *table,uint16_t out[3][256]){
 if(!table){for(unsigned c=0;c<3;c++)for(unsigned i=0;i<256;i++)out[c][i]=i*257;return true;}
 unsigned n=table->getTableCount();if(n<2||n>4096)return false;
 for(unsigned i=0;i<256;i++){
  unsigned pos=i*(n-1),lo=pos/255,hi=lo+1<n?lo+1:lo,frac=pos%255;uint32_t a[3],b[3];
  latitudeColourEntry(table,lo,a);latitudeColourEntry(table,hi,b);
  for(unsigned c=0;c<3;c++){int32_t x,y;if(!latitudeFloatQ16(a[c],&x)||!latitudeFloatQ16(b[c],&y))return false;
   out[c][i]=latitudeColourClamp(((int64_t)x*(255-frac)+(int64_t)y*frac+127)/255);}
 }
 return true;
}
static struct {
 IOAccelDisplayPipeGammaTable2 *pre,*post,*plane;LatitudeColourMatrix matrix;
 unsigned options,planeOptions;bool valid;void *owner;
} gLatitudeColourCache;
static void latitudeColourCacheClear(){
 IOAccelDisplayPipeGammaTable2 *tables[]={gLatitudeColourCache.pre,gLatitudeColourCache.post,gLatitudeColourCache.plane};for(auto t:tables)if(t)((OSObject *)t)->release();
 gLatitudeColourCache={};
}
static bool latitudePipeColour(IOService *fb,IOAccelDisplayPipeTransaction2 *txn,void *owner){
 if(!fb||!fb->getProperty("LatitudePanelValidated"))return true;
 unsigned options=txn->getTransactionOptions(),planeOptions=txn->getPlaneTransactionOptions(0);
 auto pre=(options&0x80)?nullptr:txn->getPipePreGammaTable();
 auto post=(options&0x04)?nullptr:txn->getPipePostGammaTable();
 auto plane=(planeOptions&0x04)?nullptr:txn->getPlaneGammaTable(0);
 LatitudeColourMatrix m={};if(!(options&0x40))m=txn->getPipeColorMatrix();
 bool nonzero=false;for(unsigned c=0;c<3;c++)for(unsigned j=0;j<4;j++)nonzero|=(m.bits[c][j]&0x7fffffff)!=0;
 if(!nonzero || (options&0x40)){m={};for(unsigned c=0;c<3;c++)m.bits[c][c]=0x3f800000;}
 if(gLatitudeColourCache.valid&&pre==gLatitudeColourCache.pre&&post==gLatitudeColourCache.post&&plane==gLatitudeColourCache.plane&&options==gLatitudeColourCache.options&&planeOptions==gLatitudeColourCache.planeOptions&&!memcmp(&m,&gLatitudeColourCache.matrix,sizeof m))return true;
 if((pre||post||plane)&&!latitudeColourEntryABI()){fb->setProperty("LatitudeColourDispatch","Gamma getter ABI differs; transform not applied");return false;}
 int32_t matrix[3][4];for(unsigned c=0;c<3;c++)for(unsigned j=0;j<4;j++){
  if(!latitudeFloatQ16(m.bits[c][j],&matrix[c][j])){fb->setProperty("LatitudeColourDispatch","Invalid CSC coefficient");return false;}
  if(j<3&&j!=c&&matrix[c][j]){fb->setProperty("LatitudeColourDispatch","Non-diagonal CSC not implemented; transform not applied");return false;}
 }
 uint16_t before[3][256],after[3][256],layer[3][256];
 if(!latitudeColourTable(pre,before)||!latitudeColourTable(post,after)||!latitudeColourTable(plane,layer)){fb->setProperty("LatitudeColourDispatch","Invalid gamma table");return false;}
 LatitudeGamma gamma={};latitudeGammaIdentity(&gamma);
 for(unsigned c=0;c<3;c++)for(unsigned i=0;i<256;i++){
  uint16_t v=latitudeColourLookup(layer[c],i*257);v=latitudeColourLookup(before[c],v);
  v=latitudeColourClamp(((int64_t)v*matrix[c][c]+32768)/65536+matrix[c][3]);v=latitudeColourLookup(after[c],v);
  unsigned q=((unsigned)v+128)/257;gamma.rgb[c][i]=(uint8_t)q;if(q!=i)gamma.active=1;
 }
 const OSSymbol *fn=OSSymbol::withCStringNoCopy("latitudeSetTransactionGamma");
 IOReturn result=fn?fb->callPlatformFunction(fn,false,&gamma,nullptr,nullptr,nullptr):kIOReturnNoMemory;if(fn)fn->release();
 if(result!=kIOReturnSuccess){fb->setProperty("LatitudeColourDispatch","Framebuffer rejected transaction gamma");return false;}
 latitudeColourCacheClear();gLatitudeColourCache.pre=pre;gLatitudeColourCache.post=post;gLatitudeColourCache.plane=plane;
 IOAccelDisplayPipeGammaTable2 *tables[]={pre,post,plane};for(auto t:tables)if(t)((OSObject *)t)->retain();
 gLatitudeColourCache.matrix=m;gLatitudeColourCache.options=options;gLatitudeColourCache.planeOptions=planeOptions;gLatitudeColourCache.valid=true;gLatitudeColourCache.owner=owner;
 fb->setProperty("LatitudeColourDispatch","Transaction LUT / diagonal CSC applied");fb->setProperty("LatitudeColourOptions",options,32);fb->setProperty("LatitudeColourPlaneOptions",planeOptions,32);
 fb->setProperty("LatitudeColourPreTable",pre?kOSBooleanTrue:kOSBooleanFalse);fb->setProperty("LatitudeColourPostTable",post?kOSBooleanTrue:kOSBooleanFalse);fb->setProperty("LatitudeColourPlaneTable",plane?kOSBooleanTrue:kOSBooleanFalse);
 return true;
}
