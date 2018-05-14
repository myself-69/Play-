#import "ControllerBindings.h"

@implementation ControllerBindings

-(ControllerBindings*)init
{
	if(self = [super init])
	{
		_padHandler = new CPH_HidMacOSX();
		self.bindings = [[NSMutableArray alloc] init];
		for(int i = 0; i < PS2::CControllerInfo::MAX_BUTTONS; i++)
		{
			NSString* buttonName = [NSString stringWithUTF8String: PS2::CControllerInfo::m_buttonName[i]];
			[self.bindings addObject: [[ControllerBinding alloc] initWithPadHandler: _padHandler buttonId: static_cast<PS2::CControllerInfo::BUTTON>(i) buttonName: buttonName]];
		}
	}
	return self;
}

-(void)dealloc
{
	delete _padHandler;
}

-(void)save
{
	NSEnumerator* bindingIterator = [self.bindings objectEnumerator];
	ControllerBinding* binding = nil;
	while(binding = [bindingIterator nextObject])
	{
		[binding save];
	}
}

-(void)update
{
	for(ControllerBinding* binding in self.bindings)
	{
		[binding update];
	}
}

-(NSInteger)numberOfRowsInTableView: (NSTableView*)tableView
{
	return [self.bindings count];
}

-(id)tableView: (NSTableView*)tableView objectValueForTableColumn: (NSTableColumn*)tableColumn row: (NSInteger)row
{
	if(row >= [self.bindings count]) return @"";
	ControllerBinding* binding = [self.bindings objectAtIndex: row];
	if([[tableColumn identifier] isEqualToString: @"buttonName"])
	{
		return binding.buttonName;
	}
	else if([[tableColumn identifier] isEqualToString: @"bindingName"])
	{
		return binding.bindingName;
	}
	else if([[tableColumn identifier] isEqualToString: @"bindingValue"])
	{
		return binding.bindingValue;
	}
	return @"";
}

-(ControllerBinding*)getBindingAt: (NSUInteger)index
{
	if(index >= [self.bindings count]) return nil;
	return [self.bindings objectAtIndex: index];
}

@end

//---------------------------------------------------------------------------

@implementation ControllerBinding

-(ControllerBinding*)initWithPadHandler: (CPH_HidMacOSX*)padHandler buttonId: (PS2::CControllerInfo::BUTTON)buttonId buttonName: (NSString*)buttonName
{
	if(self = [super init])
	{
		_padHandler = padHandler;
		_buttonId = buttonId;
		self.buttonName = buttonName;
		self.bindingName = @"";
		[self update];
	}
	return self;
}

-(void)requestModification
{
	[self doesNotRecognizeSelector: _cmd];
}

-(void)save
{
	[self doesNotRecognizeSelector: _cmd];
}

-(void)update
{
	uint32 value = _padHandler->GetBindingValue(_buttonId);
	self.bindingValue = [NSString stringWithFormat: @"%d", value];
}

@end

//---------------------------------------------------------------------------
