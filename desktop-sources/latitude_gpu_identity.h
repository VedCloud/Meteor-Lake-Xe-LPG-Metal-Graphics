#pragma once
/* Reporting only. Never used to size a physical mapping or an allocation. */
static bool latitudeGPUIdentityValid(unsigned version, unsigned vendor, unsigned device,
                                     const char *name, unsigned length,
                                     unsigned long long heap, unsigned long long ram)
{
    if (version != 1 || vendor != 0x8086 || (device != 0x7d45 && device != 0x7d55) ||
        !name || !length || length > 255 || !heap || heap > ram ||
        !(heap >> 20) || (heap >> 20) > 0x7fffffffULL) return false;
    for (unsigned i = 0; i < length; ++i)
        if ((unsigned char)name[i] < 0x20 || (unsigned char)name[i] > 0x7e) return false;
    return name[length] == '\0';
}
