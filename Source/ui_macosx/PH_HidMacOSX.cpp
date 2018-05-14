#include "PH_HidMacOSX.h"
#include <stdexcept>

CPH_HidMacOSX::CPH_HidMacOSX()
{
	m_bindings[PS2::CControllerInfo::ANALOG_LEFT_X] = std::make_shared<CSimulatedAxisBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardD, kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardG);
	m_bindings[PS2::CControllerInfo::ANALOG_LEFT_Y] = std::make_shared<CSimulatedAxisBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardR, kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardF);

	m_bindings[PS2::CControllerInfo::ANALOG_RIGHT_X] = std::make_shared<CSimulatedAxisBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardH, kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardK);
	m_bindings[PS2::CControllerInfo::ANALOG_RIGHT_Y] = std::make_shared<CSimulatedAxisBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardU, kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardJ);
	
	m_bindings[PS2::CControllerInfo::START] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardReturnOrEnter);
	m_bindings[PS2::CControllerInfo::SELECT] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardRightShift);
	m_bindings[PS2::CControllerInfo::DPAD_LEFT] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardLeftArrow);
	m_bindings[PS2::CControllerInfo::DPAD_RIGHT] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardRightArrow);
	m_bindings[PS2::CControllerInfo::DPAD_UP] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardUpArrow);
	m_bindings[PS2::CControllerInfo::DPAD_DOWN] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardDownArrow);
	m_bindings[PS2::CControllerInfo::SQUARE] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardA);
	m_bindings[PS2::CControllerInfo::CROSS] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardZ);
	m_bindings[PS2::CControllerInfo::TRIANGLE] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardS);
	m_bindings[PS2::CControllerInfo::CIRCLE] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_KeyboardX);

	m_bindings[PS2::CControllerInfo::L1] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_Keyboard1);
	m_bindings[PS2::CControllerInfo::L2] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_Keyboard2);
	m_bindings[PS2::CControllerInfo::L3] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_Keyboard3);
	m_bindings[PS2::CControllerInfo::R1] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_Keyboard8);
	m_bindings[PS2::CControllerInfo::R2] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_Keyboard9);
	m_bindings[PS2::CControllerInfo::R3] = std::make_shared<CSimpleBinding>(kHIDPage_KeyboardOrKeypad, kHIDUsage_Keyboard0);

	m_hidManager = IOHIDManagerCreate(kCFAllocatorDefault, 0);
	{
		CFDictionaryRef matchingDict = CreateDeviceMatchingDictionary(kHIDPage_GenericDesktop, kHIDUsage_GD_Keyboard);
		IOHIDManagerSetDeviceMatching(m_hidManager, matchingDict);
		CFRelease(matchingDict);
	}
	IOHIDManagerScheduleWithRunLoop(m_hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	IOHIDManagerOpen(m_hidManager, kIOHIDOptionsTypeNone);
	IOHIDManagerRegisterInputValueCallback(m_hidManager, InputValueCallbackStub, this);
}

CPH_HidMacOSX::~CPH_HidMacOSX()
{
	IOHIDManagerClose(m_hidManager, 0);
}

void CPH_HidMacOSX::Update(uint8* ram)
{
	for(auto* listener : m_listeners)
	{
		for(unsigned int i = 0; i < PS2::CControllerInfo::MAX_BUTTONS; i++)
		{
			const auto& binding = m_bindings[i];
			if(!binding) continue;
			uint32 value = binding->GetValue();
			auto currentButtonId = static_cast<PS2::CControllerInfo::BUTTON>(i);
			if(PS2::CControllerInfo::IsAxis(currentButtonId))
			{
				listener->SetAxisState(0, currentButtonId, value & 0xFF, ram);
			}
			else
			{
				listener->SetButtonState(0, currentButtonId, value != 0, ram);
			}
		}
	}
}

uint32 CPH_HidMacOSX::GetBindingValue(PS2::CControllerInfo::BUTTON button)
{
	const auto& binding = m_bindings[button];
	if(!binding) return 0;
	return binding->GetValue();
}

CPadHandler::FactoryFunction CPH_HidMacOSX::GetFactoryFunction()
{
	//Needs to be created in the same thread as UI
	return std::bind(&CPH_HidMacOSX::PadHandlerFactory, new CPH_HidMacOSX());
}

CPadHandler* CPH_HidMacOSX::PadHandlerFactory(CPH_HidMacOSX* handler)
{
	return handler;
}

CFMutableDictionaryRef CPH_HidMacOSX::CreateDeviceMatchingDictionary(uint32 usagePage, uint32 usage)
{
	CFMutableDictionaryRef result = CFDictionaryCreateMutable(kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
	if(result == NULL) throw std::runtime_error("CFDictionaryCreateMutable failed.");
	if(usagePage != 0)
	{
		// Add key for device type to refine the matching dictionary.
		CFNumberRef pageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usagePage);
		if(pageCFNumberRef == NULL) throw std::runtime_error("CFNumberCreate failed.");
		CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsagePageKey), pageCFNumberRef);
		CFRelease(pageCFNumberRef);
		// note: the usage is only valid if the usage page is also defined
		if(usage != 0)
		{
			CFNumberRef usageCFNumberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberIntType, &usage);
			if(usageCFNumberRef == NULL) throw std::runtime_error("CFNumberCreate failed.");
			CFDictionarySetValue(result, CFSTR(kIOHIDDeviceUsageKey), usageCFNumberRef);
			CFRelease(usageCFNumberRef);
		}
	}
	return result;
}

void CPH_HidMacOSX::InputValueCallbackStub(void* context, IOReturn result, void* sender, IOHIDValueRef valueRef)
{
	reinterpret_cast<CPH_HidMacOSX*>(context)->InputValueCallback(valueRef);
}

void CPH_HidMacOSX::InputValueCallback(IOHIDValueRef valueRef)
{
	IOHIDElementRef elementRef = IOHIDValueGetElement(valueRef);
	uint32 usagePage = IOHIDElementGetUsagePage(elementRef);
	uint32 usage = IOHIDElementGetUsage(elementRef);
	CFIndex state = IOHIDValueGetIntegerValue(valueRef);
	for(const auto& binding : m_bindings)
	{
		if(!binding) continue;
		binding->ProcessEvent(usagePage, usage, state);
	}
}

//---------------------------------------------------------------------------------

CPH_HidMacOSX::CSimpleBinding::CSimpleBinding(uint32 usagePage, uint32 usage)
: m_usagePage(usagePage)
, m_usage(usage)
{
}

void CPH_HidMacOSX::CSimpleBinding::ProcessEvent(uint32 usagePage, uint32 usage, uint32 state)
{
	if(usagePage != m_usagePage) return;
	if(usage != m_usage) return;
	m_state = state;
}

uint32 CPH_HidMacOSX::CSimpleBinding::GetValue() const
{
	return m_state;
}

//---------------------------------------------------------------------------------

CPH_HidMacOSX::CSimulatedAxisBinding::CSimulatedAxisBinding(uint32 negativeUsagePage, uint32 negativeUsage, uint32 positiveUsagePage, uint32 positiveUsage)
    : m_negativeUsagePage(negativeUsagePage)
    , m_negativeUsage(negativeUsage)
    , m_positiveUsagePage(positiveUsagePage)
    , m_positiveUsage(positiveUsage)
{
}

void CPH_HidMacOSX::CSimulatedAxisBinding::ProcessEvent(uint32 usagePage, uint32 usage, uint32 state)
{
	if(
	   (usagePage == m_negativeUsagePage) &&
	   (usage == m_negativeUsage)
	   )
	{
		m_negativeState = state;
	}

	if(
	   (usagePage == m_positiveUsagePage) &&
	   (usage == m_positiveUsage)
	   )
	{
		m_positiveState = state;
	}
}

uint32 CPH_HidMacOSX::CSimulatedAxisBinding::GetValue() const
{
	uint32 value = 0x7F;

	if(m_negativeState)
	{
		value -= 0x7F;
	}
	if(m_positiveState)
	{
		value += 0x7F;
	}

	return value;
}
