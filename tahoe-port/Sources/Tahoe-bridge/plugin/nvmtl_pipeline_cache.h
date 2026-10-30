#pragma once
/* Device-lifetime compilation cache; independent of fresh command pools. */
static VkResult nvmtl_pipeline_cache_start(VkDevice device, int enabled,
                                          PFN_vkCreatePipelineCache create,
                                          VkPipelineCache *cache)
{
    *cache=VK_NULL_HANDLE;
    if(!enabled || !create)return VK_SUCCESS;
    VkPipelineCacheCreateInfo info={VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,NULL,0,0,NULL};
    VkResult result=create(device,&info,NULL,cache);
    if(result!=VK_SUCCESS)*cache=VK_NULL_HANDLE;
    return result;
}
static VkPipelineCache nvmtl_pipeline_cache_pick(VkDevice device,VkDevice owner,
                                                VkPipelineCache requested,VkPipelineCache shared)
{
    return requested ? requested : device==owner ? shared : VK_NULL_HANDLE;
}
