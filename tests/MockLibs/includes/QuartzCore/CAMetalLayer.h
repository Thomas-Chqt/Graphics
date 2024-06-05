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

@interface CAMetalLayer : NSObject

+ (CAMetalLayer*) layer;

- (instancetype) initWithMockICAMetalLayer:(IMockCAMetalLayer*)layer;

@property(nonatomic) CGSize drawableSize;

@end

#endif // CAMETALLAYER_H