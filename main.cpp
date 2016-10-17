#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

class TimeEngine
{
	private:
	steady_clock::time_point beginning;
	steady_clock::time_point initStamp;
	steady_clock::time_point latestEvent;
	
	public:
	TimeEngine()
	{
		beginning = steady_clock::now();
		initStamp = beginning;
		latestEvent = initStamp;
	}
	void InitTimeStamp()
	{
		initStamp = steady_clock::now();
	}
	int SinceInit()
	{
		duration<int,milli> span = duration_cast<milliseconds>(steady_clock::now() - initStamp);
		return span.count();
	}
	double SinceBeginning()
	{
		duration<double,milli> span = duration_cast<milliseconds>(steady_clock::now() - beginning);
		return span.count();
	}
	bool LaterAs(double timepoint) //will only trigger once!
	{
		duration<double,milli> valueSpan = duration_cast<milliseconds>(latestEvent - steady_clock::now());
		if(valueSpan.count() > timepoint)
		{
			duration<double,milli> span = duration_cast<milliseconds>(steady_clock::now() - beginning);
			if(span.count() > timepoint)
			{
				latestEvent = steady_clock::now();
				return true;
			}
		}
		return false;
	}
	void Wait(int timespan)
	{
		steady_clock::time_point waitPoint = steady_clock::now();
		bool wait = true;
		while(wait)
		{
			duration<int,milli> span = duration_cast<milliseconds>(steady_clock::now() - waitPoint);
			if(span.count() >= timespan)
			{
				wait = false;
			}
		}
		return;
	}
};

int main()
{
	TimeEngine Time;
	
	while(true)
	{
		//get init timestamp
		Time.InitTimeStamp();
		
		if(Time.LaterAs(5000))
		{
			cout<<"5 seconds over!"<<endl;
		}
		
		//subtract current time from timestamp
		Time.Wait(16 - Time.SinceInit()); //framebremse
		cout<<Time.SinceInit()<<endl;
	}
	cout<<"hello";
	cout<<"\b\b\b\bhaha";
	return 0;
}
