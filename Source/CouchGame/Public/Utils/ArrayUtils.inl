#pragma once

template <typename T>
void UArrayUtils::GetRandomElement(const TArray<T>& Array, T& outElement)
{
	if (Array.IsEmpty()) return;
	int32 RandRange = FMath::RandRange(0, Array.Num() - 1);
	outElement = Array[RandRange];
}