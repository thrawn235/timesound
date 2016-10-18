#include <iostream>
#include <chrono>
#include <portaudio.h>
#include <math.h>
#include <vector>
using namespace std;
using namespace chrono;

struct StereoData
{
	float left_phase;
	float right_phase;
};

class SoundEngine
{
	private:
	PaStream *stream;
	PaStreamParameters inputParameters, outputParameters;
	const PaDeviceInfo *inputInfo, *outputInfo;
	
	vector<StereoData> soundtrack;
	StereoData test[50000];
	
	
	public:
	SoundEngine()
	{	
		vector<StereoData> workPiece = GenSinus(500,44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSilence(44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSinus(50,44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSilence(44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSinus(100,44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSinus(200,44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSinus(500,44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSinus(1000,44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSinus(2000,44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSinus(5000,44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSinus(10000,44100);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		workPiece = GenSilence(44100*10);
		soundtrack.insert(soundtrack.end(), workPiece.begin(), workPiece.end());
		workPiece.clear();
		
	}
	vector<StereoData> GenSilence(int Frames)
	{
		vector<StereoData> Data;
		for(int i = 0; i < Frames; i++)
		{
			StereoData Silence;
			Silence.left_phase = 0;
			Silence.right_phase = 0;
			Data.push_back(Silence);
		}
		return Data;
	}
	vector<StereoData> GenSinus(int Frequency, int Frames)
	{
		vector<StereoData> Data;
		for(int i = 0; i < Frames; i++)
		{
			StereoData Silence;
			Silence.left_phase = (float) sin(((double)i/((double)44100/Frequency)) * 3.1415 * 2.);;
			Silence.right_phase = (float) sin(((double)i/((double)44100/Frequency)) * 3.1415 * 2.);;
			Data.push_back(Silence);
		}
		return Data;
	}
	void PlayNow(vector<StereoData> effect)
	{
		soundtrack.insert(soundtrack.begin(),effect.begin(), effect.end()); 
	}
	void Init()
	{
		Pa_Initialize();
		
		inputParameters.device = Pa_GetDefaultInputDevice();
		inputInfo = Pa_GetDeviceInfo(inputParameters.device);
		outputParameters.device = Pa_GetDefaultOutputDevice();
		outputInfo = Pa_GetDeviceInfo(outputParameters.device);
		
		inputParameters.channelCount = 2;
		inputParameters.sampleFormat = paFloat32;
		inputParameters.suggestedLatency = inputInfo->defaultHighInputLatency;
		inputParameters.hostApiSpecificStreamInfo = NULL;
		
		outputParameters.channelCount = 2;
		outputParameters.sampleFormat = paFloat32;
		outputParameters.suggestedLatency = outputInfo->defaultHighOutputLatency;
		outputParameters.hostApiSpecificStreamInfo = NULL;
		
		Pa_OpenStream(&stream, &inputParameters, &outputParameters, 44100, 2048, paClipOff, NULL, NULL);
		Pa_StartStream(stream);
	}
	void FeedStream()
	{	
		Pa_WriteStream(stream, soundtrack.data() , 2048);
		soundtrack.erase(soundtrack.begin(), soundtrack.begin()+2048);
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
		Sound.FeedStream();
		
		if(Time.LaterAsOnce(5000))
		{
			cout<<"5 seconds over!"<<endl;
			Sound.PlayNow(Sound.GenSinus(800, 44100*5));
		}
		
		//subtract current time from timestamp
		Time.Wait(16 - Time.SinceInit()); //framebremse
		//cout<<Time.SinceBeginning()<<endl;
	}
	cout<<"end";
	Sound.Terminate();
	
	return 0;
}
