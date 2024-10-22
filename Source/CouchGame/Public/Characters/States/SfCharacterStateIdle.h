// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/SfCharacterState.h"
#include "SfCharacterStateIdle.generated.h"

/**
 * 
 */
UCLASS()
class COUCHGAME_API USfCharacterStateIdle : public USfCharacterState
{
public:
	virtual ESfCharacterStateID GetStateID() const override;

	virtual void StateEnter(ESfCharacterStateID PreviousStateID) override;
	
	virtual void StateExit(ESfCharacterStateID NextStateID) override;
	
	virtual void StateTick(float DeltaTime) override;

private:
	GENERATED_BODY()
};
