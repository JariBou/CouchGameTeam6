#pragma once

template <typename T>
void UArrayUtils::GetRandomElement(const TArray<T>& Array, T& outElement)
{
	if (Array.IsEmpty()) return;
	uint16 RandRange = FMath::RandRange(0, Array.Num() - 1);
	outElement = Array[RandRange];
}

template <typename T>
T& UArrayUtils::GetRandomElement(const TArray<T>& Array)
{
	if (Array.IsEmpty()) return nullptr;
	uint16 RandRange = FMath::RandRange(0, Array.Num() - 1);
	return Array[RandRange];
}