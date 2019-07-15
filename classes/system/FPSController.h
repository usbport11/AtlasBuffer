#ifndef fpscontrollerh
#define fpscontrollerh

struct stFPSController
{
	const float SmallFrameTime;
	const float FixedFrameTime;
	const int FixedFrameCount;
	const float MinFrameTime;
	const int MinFrameCount;
	float SummaryFrameTime;
	float LastFrameTime;
	float CurrentFrameTime;
	float DeltaFrameTime;
	int FrameCount;
	
	stFPSController():FixedFrameTime((float)1/60),FixedFrameCount(60),MinFrameTime((float)1/30),MinFrameCount(30),SmallFrameTime(0.001f)
	{
		LastFrameTime = 0;
		CurrentFrameTime = 0;
		DeltaFrameTime = 0;
		SummaryFrameTime = 0;
		FrameCount = 0;
	}
	
	void Initialize(float Time)
	{
		CurrentFrameTime = Time;
		LastFrameTime = CurrentFrameTime;
		FrameCount = 0;
	}
	
	void FrameStep(float Time)
	{
		CurrentFrameTime = Time;
    	DeltaFrameTime = Time - LastFrameTime;
    	LastFrameTime = CurrentFrameTime;
		SummaryFrameTime += DeltaFrameTime;
		FrameCount ++;
		if(SummaryFrameTime >= 1) 
		{
			FrameCount = 0;
			SummaryFrameTime = 0;
		}
	}
	
	void FrameCheck()
	{
		//too slow
		if(DeltaFrameTime >= MinFrameTime) DeltaFrameTime = MinFrameTime;
		//too fast //DANGEROUS!
		//while(DeltaFrameTime < FixedFrameTime)
		//	Sleep(1);
	}
};

#endif
