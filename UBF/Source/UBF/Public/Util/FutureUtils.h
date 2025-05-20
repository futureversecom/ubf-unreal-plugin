#pragma once

namespace UBF
{
	template <typename T>
	TFuture<TArray<T>> WaitAll(TArray<TFuture<T>>& Futures)
	{
		TSharedRef<TPromise<TArray<T>>> CombinedPromise = MakeShared<TPromise<TArray<T>>>();
		TSharedRef<TArray<T>> Results = MakeShared<TArray<T>>();
		Results->SetNum(Futures.Num());

		if (Futures.Num() == 0)
		{
			CombinedPromise->SetValue(*Results);
			return CombinedPromise->GetFuture();
		}

		TSharedRef<FThreadSafeCounter> Counter = MakeShared<FThreadSafeCounter>(Futures.Num());

		for (int32 Index = 0; Index < Futures.Num(); ++Index)
		{
			Futures[Index].Next([CombinedPromise, Results, Counter, Index, FuturesNum = Futures.Num()](const T& Result)
			{
				(*Results)[Index] = Result;

				if (Counter->Decrement() == 0)
				{
					CombinedPromise->SetValue(*Results);
				}
			});
		}

		return CombinedPromise->GetFuture();
	}

}
