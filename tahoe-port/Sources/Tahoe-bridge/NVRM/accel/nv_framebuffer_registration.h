#pragma once

enum { kNvDisplayPipeSlots = 16, kNvPipeAbsent = -1, kNvPipeInvalid = -2 };

template <typename Pipe, typename Framebuffer, typename Owner>
static int nvFindFramebufferPipe(Pipe *const *pipes, unsigned count,
                                 Framebuffer *fb, Owner owner)
{
    if (!pipes || !fb || count > kNvDisplayPipeSlots) return kNvPipeInvalid;
    int found = kNvPipeAbsent;
    for (unsigned slot = 0; slot < count; slot++) {
        if (!pipes[slot]) return kNvPipeInvalid;
        if (owner(pipes[slot]) != fb) continue;
        if (found != kNvPipeAbsent) return kNvPipeInvalid;
        found = (int)slot;
    }
    return found;
}
