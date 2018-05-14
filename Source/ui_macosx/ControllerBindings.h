#import <Cocoa/Cocoa.h>
#include "PH_HidMacOSX.h"

@interface ControllerBinding : NSObject
{
	CPH_HidMacOSX* _padHandler;
	PS2::CControllerInfo::BUTTON _buttonId;
}

@property (copy) NSString* buttonName;
@property (copy) NSString* bindingName;
@property (copy) NSString* bindingValue;

-(ControllerBinding*)initWithPadHandler: (CPH_HidMacOSX*)padHandler buttonId: (PS2::CControllerInfo::BUTTON)buttonId buttonName: (NSString*)buttonName;
-(void)requestModification;
-(void)save;
-(void)update;

@end

//---------------------------------------------------------------------------

@interface ControllerBindings : NSObject<NSTableViewDataSource>
{
	CPH_HidMacOSX* _padHandler;
}

@property NSMutableArray* bindings;

-(ControllerBindings*)init;
-(void)save;
-(void)update;
-(ControllerBinding*)getBindingAt: (NSUInteger)index;

@end
