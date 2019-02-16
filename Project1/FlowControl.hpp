#ifndef FLOWCONTROL_HPP
#define FLOWCONTROL_HPP

#include <cstdio>

class FlowControl
{
	public:

		// Constructor
			FlowControl();

		// Functional
			void Reset();

			void Update(float deltaTime, float rtt);

			float GetSendRate();

	private:

		enum Mode
		{
			Good,
			Bad
		};

		// Data Members
			Mode mode;
			float penalty_time;
			float good_conditions_time;
			float penalty_reduction_accumulator;
};

#endif /* FLOWCONTROL_HPP */