#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include <array>
#include <cstdint>
#include <map>
#include <vector>
namespace LatitudeLCD {
enum class Layout:unsigned {RGB=0,RGB_V=1,BGR=2,BGR_V=3,Gray=4};
struct Glyph {unsigned x=0,y=0,width=0,height=0;int left=0,top=0,advance26=0;uint64_t generation=0;};
struct Key {unsigned glyph,size,phase;Layout layout;bool operator<(const Key&o)const{return std::array<unsigned,4>{glyph,size,phase,(unsigned)layout}<std::array<unsigned,4>{o.glyph,o.size,o.phase,(unsigned)o.layout};}};
class Rasterizer {
 FT_Library library=nullptr;FT_Face face=nullptr;
 std::map<Key,Glyph> cache;std::vector<uint8_t> atlas;
 unsigned cursorX=1,cursorY=1,shelfHeight=0;uint64_t epoch=1;Layout layout=Layout::RGB;
 void clear();
public:
 static constexpr unsigned extent=1024;
 Rasterizer(const char*path,long faceIndex=0);~Rasterizer();Rasterizer(const Rasterizer&)=delete;Rasterizer&operator=(const Rasterizer&)=delete;
 void setLayout(Layout value);Glyph glyph(unsigned glyphID,unsigned pixelSize,unsigned phase=0);
 unsigned glyphIndex(uint32_t codepoint)const{return FT_Get_Char_Index(face,codepoint);}
 const std::vector<uint8_t>&pixels()const{return atlas;}
 uint64_t generation()const{return epoch;}size_t cached()const{return cache.size();}
 FT_Face rawFace()const{return face;} // variation configuration before the first glyph only
};
std::array<uint8_t,3> blend(std::array<uint8_t,3> foreground,std::array<uint8_t,3> background,std::array<uint8_t,3> coverage);
}
