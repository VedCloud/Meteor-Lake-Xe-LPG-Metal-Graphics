#include "LatitudeAWDLTx.h"
#include <cassert>
int main(){
 LatitudeAWDLTxLedger l;LatitudeAWDLTxTag ordinary{};
 l.response(ordinary,1,true);l.finish(ordinary,false);assert(!l.responses&&!l.reclaimed);
 LatitudeAWDLTxTag a{l.issue(),false};++l.submitted;
 l.response(a,1,true);l.response(a,2,false);
 assert(l.responses==1&&l.succeeded==1&&!l.failed);
 l.finish(a,false);l.finish(a,false);assert(l.reclaimed==1&&!l.missingResponse);
 LatitudeAWDLTxTag b{l.issue(),false};++l.submitted;
 l.response(b,9,false);l.finish(b,false);assert(l.failed==1&&l.lastStatus==9);
 LatitudeAWDLTxTag c{l.issue(),false};++l.submitted;l.finish(c,false);
 assert(l.missingResponse==1&&l.reclaimed==3);
 LatitudeAWDLTxTag d{l.issue(),false};++l.submitted;l.finish(d,true);l.finish(d,true);
 assert(l.aborted==1&&l.submitted==l.reclaimed+l.aborted);
 l.next=UINT64_MAX;assert(l.issue()==1);
}
