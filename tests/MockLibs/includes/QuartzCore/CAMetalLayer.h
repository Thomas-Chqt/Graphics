/*
 * ---------------------------------------------------
 * CAMetalLayer.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/05 18:36:45
 * ---------------------------------------------------
 */

#ifndef CAMETALLAYER_H
# define CAMETALLAYER_H

#include <objc/NSObject.h>
#include <CoreGraphics/CGGeometry.h>

#include "IMockCAMetalLayer.hpp"

using gfx_tests::IMockCAMetalLayer;

@protocol CAMetalDrawable

@property(assign) id texture;

@end

@interface CAMetalLayer : NSObject

+ (CAMetalLayer*) layer;

- (instancetype) initWithMockICAMetalLayer:(IMockCAMetalLayer*)layer;
- (id<CAMetalDrawable>) nextDrawable;

@property(nonatomic) CGSize drawableSize;
@property(nonatomic, assign) void* device;

@end

#endif // CAMETALLAYER_H