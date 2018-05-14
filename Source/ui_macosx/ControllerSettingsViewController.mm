#import "ControllerSettingsViewController.h"

@implementation ControllerSettingsViewController

-(id)init
{
	if(self = [super initWithNibName: @"ControllerSettingsView" bundle: nil])
	{
		
	}
	return self;
}

-(void)viewWillAppear
{
	self.updateTimer = [NSTimer timerWithTimeInterval: 0.2 repeats: YES block: ^(NSTimer* timer){ [self updateBindingValues]; }];
	[[NSRunLoop currentRunLoop] addTimer: self.updateTimer forMode: NSModalPanelRunLoopMode];
}

-(void)viewWillDisappear
{
	[self.updateTimer invalidate];
}

-(void)updateBindingValues
{
	[bindings update];
	[bindingsView reloadData];
}

@end
