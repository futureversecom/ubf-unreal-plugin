#include "Dynamic.h"

#include "GCPin.h"

void UBF::FDynamicHandle::OnComplete(intptr_t Pointer)
{
}

UBF::FDynamicHandle UBF::FDynamicHandle::Foreign(UObject* Object)
{
	UGCPin* GCPin(UGCPin::Pin(Object));
	return Foreign(GCPin, [](const intptr_t VoidPtr)
	{
		UGCPin* GCPin2 = reinterpret_cast<UGCPin*>(VoidPtr);
		GCPin2->Release();
	});
}
