#include "LatitudeLCDCoreText.h"
#include "LatitudeLCD.h"
#include FT_MULTIPLE_MASTERS_H
#import <Foundation/Foundation.h>
#include <mutex>
#include <memory>
#include <cmath>
#include <cstring>
#include <algorithm>
using namespace LatitudeLCD;
struct FaceEntry {std::string key;std::unique_ptr<Rasterizer> raster;};
static std::mutex lock;static std::vector<FaceEntry> faces;
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
  if(const char*name=FT_Get_Postscript_Name(f)){if(ps&&[ps isEqualToString:[NSString stringWithUTF8String:name]]){raster=std::move(trial);break;}}
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
int LatitudeLCDDrawCTLine(CTLineRef line,uint8_t*rgba,unsigned width,unsigned height,size_t pitch,double baselineX,double baselineY,unsigned rotation){@autoreleasepool{
 if(!line||!rgba||!width||!height||width>16384||height>16384||pitch<(size_t)width*4||pitch>262144||rotation>3||!std::isfinite(baselineX)||!std::isfinite(baselineY)||fabs(baselineX)>32768||fabs(baselineY)>32768)return 0;
 std::lock_guard<std::mutex> guard(lock);std::vector<Prepared> prepared;size_t bytes=0;
 try{
  CFArrayRef runs=CTLineGetGlyphRuns(line);if(!runs||CFArrayGetCount(runs)>512)return 0;
  for(CFIndex r=0;r<CFArrayGetCount(runs);r++){
   CTRunRef run=(CTRunRef)CFArrayGetValueAtIndex(runs,r);CFIndex count=CTRunGetGlyphCount(run);if(count<0||count>4096)return 0;
   auto attrs=(NSDictionary*)CTRunGetAttributes(run);CTFontRef font=(__bridge CTFontRef)attrs[(__bridge NSString*)kCTFontAttributeName];if(!font||CFGetTypeID(font)!=CTFontGetTypeID())return 0;
   auto matrix=CTFontGetMatrix(font);if(matrix.a!=1||matrix.b||matrix.c||matrix.d!=1||matrix.tx||matrix.ty)return 0;
   double size=CTFontGetSize(font);if(size<6||size>256||std::fabs(size-std::round(size))>.001)return 0;
   CGAffineTransform text=CTRunGetTextMatrix(run);if(text.a!=1||text.b||text.c||text.d!=1||text.tx||text.ty)return 0;
   if([attrs[(__bridge NSString*)kCTStrokeWidthAttributeName] doubleValue]!=0 || [attrs[(__bridge NSString*)kCTUnderlineStyleAttributeName] intValue]!=0 || [attrs[(__bridge NSString*)kCTVerticalFormsAttributeName] boolValue])return 0;
   Rasterizer*raster=rasterFor(font);if(!raster)return 0;raster->setLayout((Layout)rotation);
   std::array<uint8_t,3>color={0,0,0};id rawColor=attrs[(__bridge NSString*)kCTForegroundColorAttributeName];
   if(rawColor){CFTypeRef object=(__bridge CFTypeRef)rawColor;if(CFGetTypeID(object)!=CGColorGetTypeID()||CGColorGetAlpha((CGColorRef)object)!=1)return 0;CGColorSpaceRef space=CGColorSpaceCreateWithName(kCGColorSpaceSRGB);CGColorRef cg=space?CGColorCreateCopyByMatchingToColorSpace(space,kCGRenderingIntentDefault,(CGColorRef)object,nullptr):nullptr;if(space)CGColorSpaceRelease(space);if(!cg)return 0;const CGFloat*c=CGColorGetComponents(cg);for(unsigned k=0;k<3;k++)color[k]=(uint8_t)std::lround(std::clamp(c[k],0.,1.)*255);CGColorRelease(cg);}
   if([attrs[(__bridge NSString*)kCTForegroundColorFromContextAttributeName] boolValue])return 0;
   std::vector<CGGlyph>glyphs(count);std::vector<CGPoint>positions(count);CTRunGetGlyphs(run,CFRangeMake(0,0),glyphs.data());CTRunGetPositions(run,CFRangeMake(0,0),positions.data());
   for(CFIndex i=0;i<count;i++){
    CGPoint point=positions[i];if(!std::isfinite(point.x)||!std::isfinite(point.y)||fabs(point.x)>32768||fabs(point.y)>32768)return 0;
    double x=baselineX+point.x;double frac=x-std::floor(x);auto g=raster->glyph(glyphs[i],(unsigned)std::lround(size),(unsigned)std::min(2.,std::floor(frac*3)));
    Prepared p{(int)std::floor(x)+g.left,(int)std::floor(baselineY-point.y)-g.top,g.width,g.height,color,{}};
    if(g.width&&g.height){bytes+=(size_t)g.width*g.height*3;if(bytes>16*1024*1024)return 0;p.coverage.resize((size_t)g.width*g.height*3);for(unsigned y=0;y<g.height;y++)for(unsigned a=0;a<g.width;a++)for(unsigned c=0;c<3;c++)p.coverage[(y*g.width+a)*3+c]=raster->pixels()[((g.y+y)*raster->extent+g.x+a)*4+c];}
    prepared.push_back(std::move(p));
   }
  }
 }catch(...){return 0;}
 // Preflight opaque-destination invariant before changing any pixels. Transparent
 // text layers require a component-alpha compositor, not RGBA alpha guessing.
 for(auto&p:prepared)for(unsigned y=0;y<p.height;y++)for(unsigned x=0;x<p.width;x++){int dx=p.x+x,dy=p.y+y;if(dx>=0&&dy>=0&&dx<(int)width&&dy<(int)height&&rgba[(size_t)dy*pitch+dx*4+3]!=255)return 0;}
 for(auto&p:prepared)for(unsigned y=0;y<p.height;y++)for(unsigned x=0;x<p.width;x++){
  int dx=p.x+x,dy=p.y+y;if(dx<0||dy<0||dx>=(int)width||dy>=(int)height)continue;
  auto at=rgba+(size_t)dy*pitch+dx*4;size_t c=((size_t)y*p.width+x)*3;auto out=blend(p.color,{at[0],at[1],at[2]},{p.coverage[c],p.coverage[c+1],p.coverage[c+2]});for(unsigned k=0;k<3;k++)at[k]=out[k];
 }
 return 1;
}}
