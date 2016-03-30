#ifndef DigitalButton_h__
#define DigitalButton_h__

#include <Arduino.h>

namespace Buttons
{
	//Pin Mode Policies///////////////////////////////////////////////////////

	struct InputMode
	{
		static void init(uint8_t pin)
		{
			pinMode(pin, INPUT);
		}
	};

	struct InputPillupMode
	{
		static void init(uint8_t pin)
		{
			pinMode(pin, INPUT_PULLUP);
		}
	};

	struct OutputMode
	{
		static void init(uint8_t pin)
		{
			pinMode(pin, OUTPUT);
		}
	};

	struct NoMode
	{
		static void init(uint8_t) {}
	};

	//Voltage Interpretation Policies/////////////////////////////////////////

	struct ActiveHigh
	{
		static void turn(uint8_t pin, bool on)
		{
			digitalWrite(pin, on ? HIGH : LOW);
		}

		static bool active(uint8_t pin)
		{
			return (digitalRead(pin) == HIGH);
		}
	};

	struct ActiveLow
	{
		static void turn(uint8_t pin, bool on)
		{
			digitalWrite(pin, on ? LOW : HIGH);
		}

		static bool active(uint8_t pin)
		{
			return (digitalRead(pin) == LOW);
		}
	};

	struct AlwaysActive
	{
		static void turn(uint8_t, bool) {}

		static bool active(uint8_t) { return true; }
	};

	struct AlwaysPassive
	{
		static void turn(uint8_t, bool) {}

		static bool active(uint8_t) { return false; }
	};

	//////////////////////////////////////////////////////////////////////////

	//interface class for digital button
	class DigitalButton
	{
	public:
		DigitalButton() {}
		virtual ~DigitalButton() {}
		
		virtual void on() = 0;
		virtual void off() = 0;
		
		virtual bool active() const = 0;

		virtual void toggle()
		{
			if(active())
			{
				off();
			}
			else
			{
				on();
			}
		}
	
	protected:
		DigitalButton(const DigitalButton&);
		DigitalButton& operator=(const DigitalButton&);
	};

	template <class ModePolicy, class ActivePolicy>
	class DigitalButtonType : public DigitalButton
	{
	public:
		DigitalButtonType(uint8_t pin) : DigitalButton(), _pin(pin)
		{
			ModePolicy::init(_pin);
		}

		virtual ~DigitalButtonType()
		{
			pinMode(_pin, INPUT);
		}

		virtual void on()
		{
			ActivePolicy::turn(_pin, true);
		}

		virtual void off()
		{
			ActivePolicy::turn(_pin, false);
		}

		virtual bool active() const
		{
			return ActivePolicy::active(_pin);
		}

	protected:
		DigitalButtonType(const DigitalButtonType&);
		DigitalButtonType& operator=(const DigitalButtonType&);

		uint8_t _pin;
	};

	typedef DigitalButtonType<InputMode,		ActiveHigh>	ButtonToVcc; //Button pulls pin to VCC
	typedef DigitalButtonType<InputMode,		ActiveLow>	ButtonToGnd; //Button pulls pin to GND
	typedef DigitalButtonType<InputPillupMode,	ActiveLow>	ButtonToGndPullUp; //Button pulls pin to GND, internal pullup active

	typedef DigitalButtonType<NoMode, AlwaysActive>	AlwaysOn; //Always reports active, does not affect pin
	typedef DigitalButtonType<NoMode, AlwaysPassive>AlwaysOff; //Always reports inactive, does not affect pin

	typedef DigitalButtonType<OutputMode, ActiveHigh>	Output; //Output pin
	typedef DigitalButtonType<OutputMode, ActiveLow>	InverseOutput; //Output pin with inverse logic levels
} //namespace Buttons

#endif // DigitalButton_h__
