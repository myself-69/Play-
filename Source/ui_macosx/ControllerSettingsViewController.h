#import <Cocoa/Cocoa.h>
#import "ControllerBindings.h"

@interface ControllerSettingsViewController : NSViewController
{
	IBOutlet ControllerBindings* bindings;
	IBOutlet NSTableView* bindingsView;
}

@property NSTimer* updateTimer;

@end
