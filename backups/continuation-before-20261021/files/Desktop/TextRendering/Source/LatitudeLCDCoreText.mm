#include "LatitudeLCDCoreText.h"
#include "LatitudeLCD.h"
#include FT_MULTIPLE_MASTERS_H
#include FT_TRUETYPE_TABLES_H
#import <Foundation/Foundation.h>
#include <mutex>
#include <memory>
#include <cmath>
#include <cstring>
#include <algorithm>
using namespace LatitudeLCD;
struct FaceEntry {std::string key;std::unique_ptr<Rasterizer> raster;};
static std::mutex lock;static std::vector<FaceEntry> faces;
static bool sameNativeTables(CTFontRef font,FT_Face face){
 // Native variable-font PostScript names may describe an instance rather than
 // the base face name. Establish actual face identity from source font tables;
 // stripping a suffix or matching a family name would select the wrong face.
 const FT_ULong tags[]={FT_MAKE_TAG('h','e','a','d'),FT_MAKE_TAG('m','a','x','p'),FT_MAKE_TAG('g','l','y','f'),FT_MAKE_TAG('C','F','F',' '),FT_MAKE_TAG('C','F','F','2')};
 bool outline=false;
 for(unsigned i=0;i<5;i++){
  CFDataRef native=CTFontCopyTable(font,(CTFontTableTag)tags[i],kCTFontTableOptionNoOptions);FT_ULong length=0;bool available=FT_Load_Sfnt_Table(face,tags[i],0,nullptr,&length)==0;
  if(!native&&!available){if(i<2)return false;continue;}
  if(!native||!available||length>16*1024*1024||(size_t)CFDataGetLength(native)!=length){if(native)CFRelease(native);return false;}
  std::vector<uint8_t>table(length);bool same=FT_Load_Sfnt_Table(face,tags[i],0,table.data(),&length)==0&&memcmp(table.data(),CFDataGetBytePtr(native),length)==0;CFRelease(native);if(!same)return false;
  if(i>=2)outline=true;
 }return outline;
}
static Rasterizer *rasterFor(CTFontRef font){
 auto url=(NSURL*)CFBridgingRelease(CTFontCopyAttribute(font,kCTFontURLAttribute));
 if(![url isKindOfClass:NSURL.class]||!url.isFileURL)return nullptr;
 auto ps=(NSString*)CFBridgingRelease(CTFontCopyPostScriptName(font));
 auto variation=(NSDictionary*)CFBridgingRelease(CTFontCopyVariation(font));
 if(!ps)return nullptr;std::string key=std::string(url.path.UTF8String)+"|"+ps.UTF8String+"|"+(variation?[[variation description] UTF8String]:"");
 for(auto &entry:faces)if(entry.key==key)return entry.raster.get();
 std::unique_ptr<Rasterizer> raster;
 for(long i=0;i<64;i++){
  auto trial=std::make_unique<Rasterizer>(url.path.fileSystemRepresentation,i);FT_Face f=trial->rawFace();
  bool same=false;if(const char*name=FT_Get_Postscript_Name(f))same=[ps isEqualToString:[NSString stringWithUTF8String:name]];
  if(same||sameNativeTables(font,f)){raster=std::move(trial);break;}
  if(i+1>=f->num_faces)break;
 }
 if(!raster)return nullptr;
 if(variation.count){
  FT_MM_Var *axes=nullptr;auto f=raster->rawFace();if(FT_Get_MM_Var(f,&axes))return nullptr;
  std::vector<FT_Fixed> coordinates;for(unsigned i=0;i<axes->num_axis;i++){NSNumber*n=variation[@(axes->axis[i].tag)];double v=n?n.doubleValue:axes->axis[i].def/65536.;coordinates.push_back((FT_Fixed)std::llround(v*65536));}
  FT_Error error=FT_Set_Var_Design_Coordinates(f,(FT_UInt)coordinates.size(),coordinates.data());FT_Done_MM_Var(f->glyph->library,axes);if(error)return nullptr;
 }
 if(faces.size()>=8)faces.erase(faces.begin());auto result=raster.get();faces.push_back({key,std::move(raster)});return result;
}
struct Prepared {int x,y;unsigned width,height;std::array<uint8_t,3>color;std::vector<uint8_t>coverage;};
// Independent coverage for each channel survives until the opaque background
// is resolved. No attempt to squeeze component alpha into a single alpha byte.
struct LatitudeLCDTextLayer {std::vector<Prepared> prepared;unsigned rotation;};
static LatitudeLCDTextLayer* makeLayer(CTLineRef line,double baselineX,double baselineY,unsigned rotation){@autoreleasepool{
 if(!line||rotation>3||!std::isfinite(baselineX)||!std::isfinite(baselineY)||fabs(baselineX)>32768||fabs(baselineY)>32768)return nullptr;
 std::lock_guard<std::mutex> guard(lock);auto result=std::make_unique<LatitudeLCDTextLayer>();result->rotation=rotation;auto &prepared=result->prepared;size_t bytes=0;
 try{
  CFArrayRef runs=CTLineGetGlyphRuns(line);if(!runs||CFArrayGetCount(runs)>512)return nullptr;
  for(CFIndex r=0;r<CFArrayGetCount(runs);r++){
   CTRunRef run=(CTRunRef)CFArrayGetValueAtIndex(runs,r);CFIndex count=CTRunGetGlyphCount(run);if(count<0||count>4096)return nullptr;
   auto attrs=(NSDictionary*)CTRunGetAttributes(run);CTFontRef font=(__bridge CTFontRef)attrs[(__bridge NSString*)kCTFontAttributeName];if(!font||CFGetTypeID(font)!=CTFontGetTypeID())return nullptr;
   auto matrix=CTFontGetMatrix(font);if(matrix.a!=1||matrix.b||matrix.c||matrix.d!=1||matrix.tx||matrix.ty)return nullptr;
   double size=CTFontGetSize(font);if(size<6||size>256||std::fabs(size-std::round(size))>.001)return nullptr;
   CGAffineTransform text=CTRunGetTextMatrix(run);if(text.a!=1||text.b||text.c||text.d!=1||text.tx||text.ty)return nullptr;
   if([attrs[(__bridge NSString*)kCTStrokeWidthAttributeName] doubleValue]!=0 || [attrs[(__bridge NSString*)kCTUnderlineStyleAttributeName] intValue]!=0 || [attrs[(__bridge NSString*)kCTVerticalFormsAttributeName] boolValue])return nullptr;
   Rasterizer*raster=rasterFor(font);if(!raster)return nullptr;raster->setLayout((Layout)rotation);
   std::array<uint8_t,3>color={0,0,0};id rawColor=attrs[(__bridge NSString*)kCTForegroundColorAttributeName];
   if(rawColor){CFTypeRef object=(__bridge CFTypeRef)rawColor;if(CFGetTypeID(object)!=CGColorGetTypeID()||CGColorGetAlpha((CGColorRef)object)!=1)return nullptr;CGColorSpaceRef space=CGColorSpaceCreateWithName(kCGColorSpaceSRGB);CGColorRef cg=space?CGColorCreateCopyByMatchingToColorSpace(space,kCGRenderingIntentDefault,(CGColorRef)object,nullptr):nullptr;if(space)CGColorSpaceRelease(space);if(!cg)return nullptr;const CGFloat*c=CGColorGetComponents(cg);for(unsigned k=0;k<3;k++)color[k]=(uint8_t)std::lround(std::clamp(c[k],0.,1.)*255);CGColorRelease(cg);}
   if([attrs[(__bridge NSString*)kCTForegroundColorFromContextAttributeName] boolValue])return nullptr;
   std::vector<CGGlyph>glyphs(count);std::vector<CGPoint>positions(count);CTRunGetGlyphs(run,CFRangeMake(0,0),glyphs.data());CTRunGetPositions(run,CFRangeMake(0,0),positions.data());
   for(CFIndex i=0;i<count;i++){
    CGPoint point=positions[i];if(!std::isfinite(point.x)||!std::isfinite(point.y)||fabs(point.x)>32768||fabs(point.y)>32768)return nullptr;
    double x=baselineX+point.x;double frac=x-std::floor(x);auto g=raster->glyph(glyphs[i],(unsigned)std::lround(size),(unsigned)std::min(2.,std::floor(frac*3)));
    if(prepared.size()>=16384)return nullptr;
    Prepared p{(int)std::floor(x)+g.left,(int)std::floor(baselineY-point.y)-g.top,g.width,g.height,color,{}};
    if(g.width&&g.height){bytes+=sizeof(Prepared)+(size_t)g.width*g.height*3;if(bytes>16*1024*1024)return nullptr;p.coverage.resize((size_t)g.width*g.height*3);for(unsigned y=0;y<g.height;y++)for(unsigned a=0;a<g.width;a++)for(unsigned c=0;c<3;c++)p.coverage[(y*g.width+a)*3+c]=raster->pixels()[((g.y+y)*raster->extent+g.x+a)*4+c];}
    if(!g.width||!g.height){bytes+=sizeof(Prepared);if(bytes>16*1024*1024)return nullptr;}
    prepared.push_back(std::move(p));
   }
  }

 }catch(...){return nullptr;}
 return result.release();
}}
LatitudeLCDTextLayer* LatitudeLCDCreateTextLayer(CTLineRef line,double x,double y,unsigned rotation){try{return makeLayer(line,x,y,rotation);}catch(...){return nullptr;}}
void LatitudeLCDDestroyTextLayer(LatitudeLCDTextLayer*layer){delete layer;}
int LatitudeLCDTextLayerBounds(const LatitudeLCDTextLayer*layer,int*x,int*y,unsigned*width,unsigned*height){
 if(!layer||!x||!y||!width||!height)return 0;int left=1000000,top=1000000,right=-1000000,bottom=-1000000;
 for(const auto&p:layer->prepared){if(!p.width||!p.height)continue;left=std::min(left,p.x);top=std::min(top,p.y);right=std::max(right,p.x+(int)p.width);bottom=std::max(bottom,p.y+(int)p.height);}
 if(right<=left||bottom<=top)return 0;*x=left;*y=top;*width=(unsigned)(right-left);*height=(unsigned)(bottom-top);return 1;
}
int LatitudeLCDCompositeTextLayerAt(const LatitudeLCDTextLayer*layer,uint8_t*rgba,size_t byteLength,unsigned width,unsigned height,size_t pitch,unsigned rotation,int originX,int originY){
 if(!layer||!rgba||!width||!height||width>16384||height>16384||pitch<(size_t)width*4||pitch>262144||rotation>3||rotation!=layer->rotation||originX< -100000||originX>100000||originY< -100000||originY>100000||byteLength<(size_t)(height-1)*pitch+(size_t)width*4)return 0;
 const auto &prepared=layer->prepared;
 // Reject the whole transaction before writing if even one destination texel
 // has unresolved alpha. A rejected composition leaves the target unchanged.
 for(auto&p:prepared)for(unsigned y=0;y<p.height;y++)for(unsigned x=0;x<p.width;x++){int dx=p.x+(int)x-originX,dy=p.y+(int)y-originY;if(dx>=0&&dy>=0&&dx<(int)width&&dy<(int)height&&rgba[(size_t)dy*pitch+dx*4+3]!=255)return 0;}
 for(auto&p:prepared)for(unsigned y=0;y<p.height;y++)for(unsigned x=0;x<p.width;x++){
  int dx=p.x+(int)x-originX,dy=p.y+(int)y-originY;if(dx<0||dy<0||dx>=(int)width||dy>=(int)height)continue;
  auto at=rgba+(size_t)dy*pitch+dx*4;size_t c=((size_t)y*p.width+x)*3;auto out=blend(p.color,{at[0],at[1],at[2]},{p.coverage[c],p.coverage[c+1],p.coverage[c+2]});for(unsigned k=0;k<3;k++)at[k]=out[k];
 }
 return 1;
}
int LatitudeLCDCompositeTextLayer(const LatitudeLCDTextLayer*layer,uint8_t*rgba,size_t byteLength,unsigned width,unsigned height,size_t pitch,unsigned rotation){return LatitudeLCDCompositeTextLayerAt(layer,rgba,byteLength,width,height,pitch,rotation,0,0);}
int LatitudeLCDDrawCTLine(CTLineRef line,uint8_t*rgba,unsigned width,unsigned height,size_t pitch,double x,double y,unsigned rotation){
 if(!rgba||!width||!height||width>16384||height>16384||pitch<(size_t)width*4||pitch>262144)return 0;
 std::unique_ptr<LatitudeLCDTextLayer> layer(LatitudeLCDCreateTextLayer(line,x,y,rotation));
 return LatitudeLCDCompositeTextLayer(layer.get(),rgba,pitch*height,width,height,pitch,rotation);
}

size_t LatitudeLCDTextLayerByteSize(const LatitudeLCDTextLayer*layer){if(!layer)return 0;size_t n=sizeof(*layer);for(const auto&p:layer->prepared)n+=sizeof(p)+p.coverage.size();return n;}
LatitudeLCDTextLayer* LatitudeLCDCreateGlyphLayer(CTFontRef font,const CGGlyph*glyphs,const CGPoint*positions,size_t count,unsigned canvasHeight,const uint8_t color[3],unsigned rotation){@autoreleasepool{
 if(!font||!glyphs||!positions||!color||!count||count>16384||canvasHeight>16384||rotation>3)return nullptr;
 std::lock_guard<std::mutex>guard(lock);
 try{
  auto matrix=CTFontGetMatrix(font);double size=CTFontGetSize(font);
  if(!CGAffineTransformIsIdentity(matrix)||size<6||size>256||fabs(size-round(size))>.001)return nullptr;
  auto raster=rasterFor(font);if(!raster||FT_HAS_COLOR(raster->rawFace()))return nullptr;raster->setLayout((Layout)rotation);
  auto result=std::make_unique<LatitudeLCDTextLayer>();result->rotation=rotation;size_t bytes=0;
  for(size_t i=0;i<count;i++){
   auto point=positions[i];if(!std::isfinite(point.x)||!std::isfinite(point.y)||fabs(point.x)>32768||fabs(point.y)>32768||point.y!=floor(point.y))return nullptr;
   double frac=point.x-floor(point.x);auto g=raster->glyph(glyphs[i],(unsigned)lround(size),(unsigned)std::min(2.,floor(frac*3)));
   Prepared p{(int)floor(point.x)+g.left,(int)canvasHeight-(int)point.y-g.top,g.width,g.height,{color[0],color[1],color[2]}, {}};
   bytes+=sizeof(p)+(size_t)g.width*g.height*3;if(bytes>16*1024*1024)return nullptr;
   p.coverage.resize((size_t)g.width*g.height*3);for(unsigned y=0;y<g.height;y++)for(unsigned x=0;x<g.width;x++)for(unsigned c=0;c<3;c++)p.coverage[((size_t)y*g.width+x)*3+c]=raster->pixels()[((g.y+y)*raster->extent+g.x+x)*4+c];
   result->prepared.push_back(std::move(p));
  }return result.release();
 }catch(...){return nullptr;}
}}
