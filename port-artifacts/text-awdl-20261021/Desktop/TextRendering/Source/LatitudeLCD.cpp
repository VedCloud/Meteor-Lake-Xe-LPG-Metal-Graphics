#include "LatitudeLCD.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <cstring>
namespace LatitudeLCD {
Rasterizer::Rasterizer(const char*path,long index):atlas(extent*extent*4,0){
 if(FT_Init_FreeType(&library))throw std::runtime_error("FreeType initialization failed");
 if(FT_New_Face(library,path,index,&face)){FT_Done_FreeType(library);library=nullptr;throw std::runtime_error("Font face unavailable");}
 if(FT_Library_SetLcdFilter(library,FT_LCD_FILTER_DEFAULT)){FT_Done_Face(face);FT_Done_FreeType(library);face=nullptr;library=nullptr;throw std::runtime_error("Filtered LCD rendering unavailable");}
}
Rasterizer::~Rasterizer(){if(face)FT_Done_Face(face);if(library)FT_Done_FreeType(library);}
void Rasterizer::clear(){cache.clear();std::fill(atlas.begin(),atlas.end(),0);cursorX=cursorY=1;shelfHeight=0;epoch++;}
void Rasterizer::setLayout(Layout value){if((unsigned)value>4)throw std::invalid_argument("Unknown panel layout");if(value!=layout){layout=value;clear();}}
Glyph Rasterizer::glyph(unsigned id,unsigned size,unsigned phase){
 if(size<6||size>256||phase>2||id>=(unsigned)face->num_glyphs)throw std::invalid_argument("Invalid glyph request");
 Key key{id,size,phase,layout};auto found=cache.find(key);if(found!=cache.end())return found->second;
 if(FT_Set_Pixel_Sizes(face,0,size))throw std::runtime_error("Font size rejected");
 bool vertical=layout==Layout::RGB_V||layout==Layout::BGR_V,gray=layout==Layout::Gray;
 FT_Vector delta={vertical?0:(FT_Pos)(phase*64/3),vertical?(FT_Pos)(phase*64/3):0};FT_Set_Transform(face,nullptr,&delta);
 FT_Int32 flags=FT_LOAD_DEFAULT|(gray?FT_LOAD_TARGET_NORMAL:vertical?FT_LOAD_TARGET_LCD_V:FT_LOAD_TARGET_LCD);
 if(FT_Load_Glyph(face,id,flags)||face->glyph->format!=FT_GLYPH_FORMAT_OUTLINE)throw std::runtime_error("Unsupported non-outline glyph");
 if(FT_Render_Glyph(face->glyph,gray?FT_RENDER_MODE_NORMAL:vertical?FT_RENDER_MODE_LCD_V:FT_RENDER_MODE_LCD))throw std::runtime_error("Glyph rendering failed");
 FT_Bitmap&b=face->glyph->bitmap;unsigned width=gray||vertical?b.width:b.width/3,height=gray||!vertical?b.rows:b.rows/3;
 if((!gray&&((vertical&&b.rows%3)||(!vertical&&b.width%3)))||width+2>extent||height+2>extent)throw std::runtime_error("Invalid LCD bitmap dimensions");
 if(cursorX+width+1>=extent){cursorX=1;cursorY+=shelfHeight+1;shelfHeight=0;}
 if(cursorY+height+1>=extent||cache.size()>=4096)clear();
 Glyph result{cursorX,cursorY,width,height,face->glyph->bitmap_left,face->glyph->bitmap_top,(int)face->glyph->advance.x,epoch};
 auto row=[&](unsigned y){return b.buffer+(b.pitch>=0?y*(size_t)b.pitch:(b.rows-1-y)*(size_t)(-b.pitch));};
 bool reverse=layout==Layout::BGR||layout==Layout::BGR_V;
 for(unsigned y=0;y<height;y++)for(unsigned x=0;x<width;x++){
  size_t at=((result.y+y)*extent+result.x+x)*4;unsigned sum=0;
  for(unsigned c=0;c<3;c++){unsigned channel=reverse?2-c:c;uint8_t coverage=gray?row(y)[x]:vertical?row(y*3+channel)[x]:row(y)[x*3+channel];atlas[at+c]=coverage;sum+=coverage;}
  atlas[at+3]=(sum+1)/3;
 }
 cursorX+=width+1;shelfHeight=std::max(shelfHeight,height);cache[key]=result;return result;
}
static double linear(double v){return v<=.04045?v/12.92:std::pow((v+.055)/1.055,2.4);}
static uint8_t srgb(double v){v=std::clamp(v,0.,1.);return (uint8_t)std::lround(255*(v<=.0031308?12.92*v:1.055*std::pow(v,1/2.4)-.055));}
std::array<uint8_t,3> blend(std::array<uint8_t,3> fg,std::array<uint8_t,3> bg,std::array<uint8_t,3> coverage){
 std::array<uint8_t,3> out;for(unsigned i=0;i<3;i++){double a=coverage[i]/255.;out[i]=srgb(linear(fg[i]/255.)*a+linear(bg[i]/255.)*(1-a));}return out;
}
}
