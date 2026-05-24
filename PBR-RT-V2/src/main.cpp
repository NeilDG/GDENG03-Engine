#include "AnitoApplication.h"
#include "SampleBase.hpp"

int main()
{
	Diligent::SampleBase* sample = PbrRtV2::AnitoApplication::CreateSample();
	delete sample;
	return 0;
}
