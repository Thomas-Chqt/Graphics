/*
 * ---------------------------------------------------
 * ApplicationDelegate.mm
 *
 * Author: Thomas Choquet <thomas.publique@icloud.com>
 * Date: 2023/11/26 11:08:17
 * ---------------------------------------------------
 */

#include "ApplicationDelegate.h"
#include "Graphics/Event.hpp"
#include "UtilsCPP/Func.hpp"

using gfx::Event;
using utils::Func;
using gfx::ApplicationRequestTerminationEvent;

@implementation ApplicationDelegate
{
    Func<void(Event&)> m_eventCallBackFunc;
}

- (instancetype)initWithCallBack:(const Func<void(Event&)>&)callBack
{
    [super init];
    m_eventCallBackFunc = callBack;
    return self;
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    assert(m_eventCallBackFunc == true);
    ApplicationRequestTerminationEvent applicationRequestTerminationEvent;
    m_eventCallBackFunc(applicationRequestTerminationEvent);
    return NSTerminateCancel;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    NSEvent* event = [NSEvent otherEventWithType:NSEventTypeApplicationDefined
                                        location:NSMakePoint(0, 0)
                                   modifierFlags:0
                                       timestamp:0
                                    windowNumber:0
                                         context:nil
                                         subtype:0
                                           data1:0
                                           data2:0];
    [NSApp postEvent:event atStart:YES];
    [NSApp stop:nil];
}

@end