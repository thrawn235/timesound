#include <iostream>
#include <chrono>
#include <portaudio.h>
using namespace std;
using namespace chrono;

struct paTestData
{
	float left_phase;
	float right_phase;
};

static int PaTestCallback(const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
	paTestData *data = (paTestData*)userData;
	float *out = (float*)outputBuffer;
	(void) inputBuffer; //prevent unused variable warning
	
	for(unsigned int i = 0; i < framesPerBuffer; i++)
	{
		*out++ = data->left_phase;
		*out++ = data->right_phase;
		data->left_phase += 0.01;
		if(data->left_phase >= 1)
		{
			data->left_phase -= 2;
		}
		data->right_phase += 0.03;
		if(data->right_phase >= 1)
		{
			data->right_phase -= 2;
		}
	}
	
	return 0;
};

class SoundEngine
{
	private:
	PaStream *stream;
	
	public:
	SoundEngine()
	{
	}
	void Init()
	{
		Pa_Initialize();
	
		static paTestData data;
		Pa_OpenDefaultStream(&stream, 0, 2, paFloat32, 44100, 256, PaTestCallback, &data);
		Pa_StartStream(stream);
	}
	void Terminate()
	{
		Pa_StopStream(stream);
		Pa_CloseStream(stream);
		Pa_Terminate();
	}
};



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
	bool LaterAs(double timepoint)
	{
		duration<double,milli> span = duration_cast<milliseconds>(steady_clock::now() - beginning);
		if(span.count() > timepoint)
		{
			//latestEvent = steady_clock::now();
			return true;
		}
		return false;
	}
	bool LaterAsOnce(double timepoint)
	{
		duration<double,milli> span = duration_cast<milliseconds>(latestEvent - beginning);
		double latestEventTime = span.count();
		if(timepoint >= latestEventTime)
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
	bool LaterAsFromEvent(steady_clock::time_point Event, double time)
	{
	}
	double TimeDifference(steady_clock::time_point time1, steady_clock::time_point time2)
	{
		duration<double,milli> span = duration_cast<milliseconds>(time1 - time2);
		return span.count();
	}
	steady_clock::time_point Now()
	{
		return steady_clock::now();
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
	SoundEngine Sound;
	
	
	Sound.Init();
	
	while(true)
	{
		//get init timestamp
		Time.InitTimeStamp();
		
		if(Time.LaterAsOnce(5000))
		{
			cout<<"5 seconds over!"<<endl;
		}
		
		//subtract current time from timestamp
		Time.Wait(16 - Time.SinceInit()); //framebremse
		//cout<<Time.SinceBeginning()<<endl;
	}
	cout<<"end";
	Sound.Terminate();
	
	return 0;
}
