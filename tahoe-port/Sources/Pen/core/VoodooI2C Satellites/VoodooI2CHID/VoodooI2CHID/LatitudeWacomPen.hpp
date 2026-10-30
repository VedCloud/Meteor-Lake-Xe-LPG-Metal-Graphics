// Wacom 056a:4a19, descriptor SHA256 recorded in DESCRIPTOR.json.
// Pure decoder, shared verbatim by the kernel and sanitizer replay tests.
#ifndef LATITUDE_WACOM_PEN_HPP
#define LATITUDE_WACOM_PEN_HPP
struct LatitudeWacomPen {
 bool inRange, tip, eraser, inverted;
 unsigned rawFlags, rawPressure;
 unsigned buttons;
 int x, y, pressure, tiltX, tiltY;
};
static inline unsigned latitudePenLE16(const unsigned char* p) {
 return unsigned(p[0]) | (unsigned(p[1]) << 8);
}
static inline int latitudePenScale(unsigned n, unsigned max) {
 return int((static_cast<unsigned long long>(n > max ? max : n) * 65535) / max);
}
static inline int latitudePenTilt(unsigned n) {
 int v = n < 32768 ? int(n) : int(n) - 65536;
 if(v < -9000) v = -9000;
 if(v > 9000) v = 9000;
 return int((static_cast<long long>(v) * 65536) / 100);
}
static inline bool latitudeDecodePen(const unsigned char* p, unsigned n,
                                    unsigned reportID, LatitudeWacomPen& v) {
 // Both input reports are 168 payload bits + 8-bit report ID.
 if(!p || n < 22 || (reportID != 0x14 && reportID != 0x0b) || p[0] != reportID)
  return false;
 // Contact itself establishes proximity. Do not discard a tip/eraser packet
 // solely because the separate InRange usage is clear in that same packet.
 v.rawFlags = p[1];
 v.rawPressure = latitudePenLE16(p+6);
 v.tip = (p[1] & 1) != 0;
 v.eraser = (p[1] & 4) != 0;
 v.inRange = (p[1] & 0x25) != 0;
 v.inverted = (p[1] & 8) != 0;
 v.buttons = !v.inRange ? 0 : ((v.tip ? 1 : 0) |
             ((p[1] & 0x12) ? 2 : 0) | (v.eraser ? 4 : 0));
 v.x = latitudePenScale(latitudePenLE16(p+2), 29376);
 v.y = latitudePenScale(latitudePenLE16(p+4), 16524);
 v.pressure = (v.tip || v.eraser) ? latitudePenScale(latitudePenLE16(p+6),4095) : 0;
 v.tiltX = latitudePenTilt(latitudePenLE16(p+16));
 v.tiltY = latitudePenTilt(latitudePenLE16(p+18));
 return true;
}
static inline bool latitudePenDescriptor(const unsigned char* p, unsigned n) {
 if(!p || n != 893) return false;
 unsigned long long h = 14695981039346656037ULL;
 for(unsigned i=0;i<n;++i) h = (h ^ p[i]) * 1099511628211ULL;
 return h == 0x3640bb7bf38ad70aULL;
}
#endif
