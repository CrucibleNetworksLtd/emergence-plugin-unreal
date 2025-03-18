#include "EmergenceLocalEVM3Thread.h"

bool FLocalEVM3ThreadRunnable::Init()
{
	return true;
}

uint32 FLocalEVM3ThreadRunnable::Run()
{
	GetURI(*UriBufferRef, *StatusRef);
	return 0;
}

void FLocalEVM3ThreadRunnable::Exit()
{

}

void FLocalEVM3ThreadRunnable::Stop()
{
}
