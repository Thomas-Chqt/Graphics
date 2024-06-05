/*
 * ---------------------------------------------------
 * IMockCAMetalLayer.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/05 18:39:54
 * ---------------------------------------------------
 */

#include "QuartzCore/CAMetalLayer.h"
#include "QuartzCore/IMockCAMetalLayer.hpp"

@implementation CAMetalLayer
{
    gfx_tests::IMockCAMetalLayer* m_mockCAMetalLayer;
}

+ (CAMetalLayer*) layer
{
    return (CAMetalLayer*)gfx_tests::IMockCAMetalLayerStaticFuncs::s_instance->layer()->caMetalLayer;
}

- (instancetype) initWithMockICAMetalLayer:(IMockCAMetalLayer*)layer
{
    [super init];
    m_mockCAMetalLayer = layer;
    return self;
}

- (CGSize) drawableSize
{
    return m_mockCAMetalLayer->getDrawableSize();
}

@end

namespace gfx_tests
{

IMockCAMetalLayerStaticFuncs* IMockCAMetalLayerStaticFuncs::s_instance = nullptr;

IMockCAMetalLayer::IMockCAMetalLayer()
{
    caMetalLayer = [[CAMetalLayer alloc] initWithMockICAMetalLayer:this];
}

IMockCAMetalLayer::~IMockCAMetalLayer()
{
    [(CAMetalLayer*)caMetalLayer release];
}

}