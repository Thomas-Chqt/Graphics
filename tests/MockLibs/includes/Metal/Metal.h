/*
 * ---------------------------------------------------
 * Metal.h
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2024/06/05 23:58:39
 * ---------------------------------------------------
 */

#ifndef METAL_H
# define METAL_H

#include <objc/NSObject.h>

#include "IMockMetal.hpp"

typedef struct MTLClearColor {
    double red;
    double green;
    double blue;
    double alpha;
} MTLClearColor;

typedef enum MTLLoadAction : NSUInteger {
    MTLLoadActionClear,
    MTLLoadActionLoad
} MTLLoadAction;

typedef enum MTLStoreAction : NSUInteger {
    MTLStoreActionStore
} MTLStoreAction;

typedef enum MTLPixelFormat : NSUInteger {
    MTLPixelFormatRGBA8Unorm,
    MTLPixelFormatBGRA8Unorm
} MTLPixelFormat;

@interface MTLTextureDescriptor : NSObject

@property(readwrite, nonatomic) NSUInteger width;
@property(readwrite, nonatomic) NSUInteger height;
@property(readwrite, nonatomic) MTLPixelFormat pixelFormat;

@end


@interface MTLRenderPassColorAttachmentDescriptor : NSObject

@property(nonatomic) MTLClearColor clearColor;
@property(nonatomic) MTLLoadAction loadAction;
@property(nonatomic) MTLStoreAction storeAction;

@end


@interface MTLRenderPassColorAttachmentDescriptorArray : NSObject

- (MTLRenderPassColorAttachmentDescriptor *)objectAtIndexedSubscript:(NSUInteger)attachmentIndex;
- (void)setObject:(MTLRenderPassColorAttachmentDescriptor *)attachment atIndexedSubscript:(NSUInteger)attachmentIndex;

@end


@interface MTLRenderPassDescriptor : NSObject

@property(readonly) MTLRenderPassColorAttachmentDescriptorArray *colorAttachments;

@end


@protocol MTLCommandBuffer
@end


@protocol MTLCommandQueue

- (id<MTLCommandBuffer>)commandBuffer;

@end


@protocol MTLLibrary
@end

@class NSURL;
@class NSError;

@protocol MTLDevice

- (id<MTLCommandQueue>) newCommandQueue;
- (id<MTLLibrary>) newLibraryWithURL:(NSURL*)url error:(NSError**)err;

@end

id<MTLDevice> MTLCreateSystemDefaultDevice();
MTLClearColor MTLClearColorMake(double red, double green, double blue, double alpha);

#endif // METAL_H