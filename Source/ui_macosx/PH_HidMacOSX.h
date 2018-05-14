#pragma once

#include "../PadHandler.h"
#import <IOKit/hid/IOHIDLib.h>

class CPH_HidMacOSX : public CPadHandler
{
public:
	CPH_HidMacOSX();
	virtual ~CPH_HidMacOSX();

	void Update(uint8*) override;
	uint32 GetBindingValue(PS2::CControllerInfo::BUTTON);

	static FactoryFunction GetFactoryFunction();

private:
	class CBinding
	{
	public:
		virtual ~CBinding() = default;

		virtual void ProcessEvent(uint32, uint32, uint32) = 0;
		virtual uint32 GetValue() const = 0;
	};

	typedef std::shared_ptr<CBinding> BindingPtr;

	class CSimpleBinding : public CBinding
	{
	public:
		CSimpleBinding(uint32, uint32);

		void ProcessEvent(uint32, uint32, uint32) override;
		uint32 GetValue() const override;

	private:
		uint32 m_usagePage;
		uint32 m_usage;
		uint32 m_state = 0;
	};

	class CSimulatedAxisBinding : public CBinding
	{
	public:
		CSimulatedAxisBinding(uint32, uint32, uint32, uint32);

		void ProcessEvent(uint32, uint32, uint32) override;
		uint32 GetValue() const override;

	private:
		uint32 m_negativeUsagePage;
		uint32 m_negativeUsage;
		
		uint32 m_positiveUsagePage;
		uint32 m_positiveUsage;

		uint32 m_negativeState = 0;
		uint32 m_positiveState = 0;
	};

	static CPadHandler* PadHandlerFactory(CPH_HidMacOSX*);

	CFMutableDictionaryRef CreateDeviceMatchingDictionary(uint32, uint32);
	static void InputValueCallbackStub(void*, IOReturn, void*, IOHIDValueRef);
	void InputValueCallback(IOHIDValueRef);

	IOHIDManagerRef m_hidManager;
	BindingPtr m_bindings[PS2::CControllerInfo::MAX_BUTTONS];
};

