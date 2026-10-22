#include <libkern/c++/OSSymbol.h>
extern "C" unsigned acpiNameLength(const OSSymbol *s){return s->getLength();}
