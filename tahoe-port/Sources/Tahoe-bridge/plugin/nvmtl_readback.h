#pragma once
#include <stdint.h>
#include <stddef.h>
/* The caller owns the image until completion. No persistent layout change is made. */
typedef struct { uint32_t cols, rows; size_t packed_row, bytes; VkBufferImageCopy copy; VkImageMemoryBarrier before, after; } nvmtl_read_plan;
static inline int nvmtl_read_plan_make(const nvk_image *i, size_t pitch,
    uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t level, uint32_t layer,
    VkImageAspectFlags aspect, VkImageAspectFlags barrier_aspect, size_t bpp, nvmtl_read_plan *p)
{
    if (!i || !i->img || !w || !h || !bpp || !i->w || !i->h || level >= 32 ||
        level >= (i->mips ? i->mips : 1) || i->samples > 1 ||
        !(i->usage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) ||
        i->layout == VK_IMAGE_LAYOUT_UNDEFINED || i->layout == VK_IMAGE_LAYOUT_PREINITIALIZED ||
        !aspect || (aspect & (aspect-1)) || (aspect & barrier_aspect) != aspect) return -1;
    uint32_t lw=i->w>>level, lh=i->h>>level;
    if (!lw) lw=1; if (!lh) lh=1;
    uint32_t nl=i->layers ? i->layers : 1;
    if (i->mtl_type==7) { nl >>= level; if (!nl) nl=1; }
    if (layer>=nl || x>=lw || y>=lh || w>lw-x || h>lh-y || x>INT32_MAX || y>INT32_MAX || layer>INT32_MAX) return -1;
    uint32_t bw=i->bw ? i->bw : 1, bh=i->bh ? i->bh : 1;
    if (x%bw || y%bh || (w%bw && w!=lw-x) || (h%bh && h!=lh-y)) return -1;
    p->cols=w/bw+(w%bw!=0); p->rows=h/bh+(h%bh!=0);
    if (p->cols>SIZE_MAX/bpp) return -1;
    p->packed_row=p->cols*bpp;
    if (pitch<p->packed_row || p->rows>SIZE_MAX/p->packed_row ||
        (p->rows>1 && pitch>(SIZE_MAX-p->packed_row)/(p->rows-1))) return -1;
    p->bytes=p->packed_row*p->rows;
    p->copy=(VkBufferImageCopy){0,0,0,{aspect,level,i->mtl_type==7 ? 0:layer,1},
        {(int32_t)x,(int32_t)y,i->mtl_type==7 ? (int32_t)layer:0},{w,h,1}};
    VkImageSubresourceRange range={barrier_aspect,level,1,i->mtl_type==7 ? 0:layer,1};
    p->before=(VkImageMemoryBarrier){VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,NULL,
        VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,VK_ACCESS_TRANSFER_READ_BIT,
        (VkImageLayout)i->layout,VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,(VkImage)i->img,range};
    p->after=p->before; p->after.srcAccessMask=VK_ACCESS_TRANSFER_READ_BIT;
    p->after.dstAccessMask=VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT;
    p->after.oldLayout=VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL; p->after.newLayout=(VkImageLayout)i->layout;
    return 0;
}
static inline VkBufferMemoryBarrier nvmtl_read_host_barrier(const nvk_buffer *b, size_t bytes)
{
    return (VkBufferMemoryBarrier){VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,NULL,
        VK_ACCESS_TRANSFER_WRITE_BIT,VK_ACCESS_HOST_READ_BIT,
        VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,(VkBuffer)b->buf,0,bytes};
}

/* IOSurface reuploads can follow render or transfer accesses to this image. */
static inline VkImageMemoryBarrier nvmtl_surface_upload_before(const nvk_image *i, VkImageSubresourceRange range)
{
    return (VkImageMemoryBarrier){VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,NULL,
        i->layout==VK_IMAGE_LAYOUT_UNDEFINED ? 0 : VK_ACCESS_MEMORY_READ_BIT|VK_ACCESS_MEMORY_WRITE_BIT,
        VK_ACCESS_TRANSFER_WRITE_BIT,(VkImageLayout)i->layout,VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_QUEUE_FAMILY_IGNORED,VK_QUEUE_FAMILY_IGNORED,(VkImage)i->img,range};
}
