#pragma once
namespace LatitudeTablet {
class ModeQualification {
 unsigned long long previous=0,lastCC=0,foldSince=0;bool clockSeen=false,ccSeen=false,laptopSeen=false,foldPending=false,qualified=false,tablet=false;
public:
 void reset(){*this=ModeQualification();}
 void event(unsigned code,unsigned long long now){
  if(clockSeen && now<previous)reset();
  if(code==0xcc && laptopSeen){lastCC=now;ccSeen=true;}
  if(code==0xcd){qualified=false;tablet=false;foldPending=false;ccSeen=false;}
 }
 bool update(unsigned long long now,bool valid,unsigned raw){
  if(clockSeen && now<previous){reset();return false;}
  clockSeen=true;previous=now;
  if(!valid || raw>=255){qualified=false;tablet=false;foldPending=false;ccSeen=false;return false;}
  if(!(raw&3)){laptopSeen=true;qualified=true;tablet=false;foldPending=false;if(ccSeen && (lastCC>now || now-lastCC>5000))ccSeen=false;return true;}
  // Require a measured laptop state, an actual folded CC event and a second
  // stable firmware read. A clear gated VGBS bit or booting folded is insufficient.
  if(qualified && tablet)return true;
  if(!laptopSeen || !ccSeen || lastCC>now || now-lastCC>5000){qualified=false;foldPending=false;return false;}
  if(!foldPending){foldPending=true;foldSince=now;qualified=false;return false;}
  if(now-foldSince<500)return false;
  tablet=true;qualified=true;return true;
 }
 bool isTablet()const{return qualified&&tablet;}
};
}
