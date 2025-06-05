/*
 * ---------------------------------------------------
 * MetalRenderPass.mm
 *
 * Author: Thomas Choquet <semoir.dense-0h@icloud.com>
 * Date: 2025/06/04 14:20:06
 * ---------------------------------------------------
 */

#include "Graphics/RenderPass.hpp"

#include "Metal/MetalRenderPass.hpp"
#include "Metal/MetalEnums.hpp"

#import <Metal/Metal.h>

namespace gfx
{

MetalRenderPass::MetalRenderPass(const RenderPass::Descriptor& desc)
{
    m_mtlRenderPassDescriptor = [MTLRenderPassDescriptor new];
    for (int i = 0; auto& colorAttachment : desc.colorAttachments)
    {
        m_mtlRenderPassDescriptor.colorAttachments[i].loadAction = toMTLLoadAction(colorAttachment.loadAction);
        m_mtlRenderPassDescriptor.colorAttachments[i].storeAction = MTLStoreActionStore;
    }
    if (desc.depthAttachment.has_value())
    {
        m_mtlRenderPassDescriptor.depthAttachment.loadAction = toMTLLoadAction(desc.depthAttachment.value().loadAction);
        m_mtlRenderPassDescriptor.depthAttachment.storeAction = MTLStoreActionStore;
    }
}

MetalRenderPass::~MetalRenderPass()
{
    [m_mtlRenderPassDescriptor release];
}

}
