// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SfCharacterStateID.h"
#include "UObject/Object.h"
#include "SfCharacterStateMachine.generated.h"

class ASfCharacter;
class USfCharacterState;
/**
 * 
 */
UCLASS()
class COUCHGAME_API USfCharacterStateMachine : public UObject
{
	GENERATED_BODY()

public:
	void Init(ASfCharacter* InCharacter);

	void Tick(float DeltaTime) const;

	UFUNCTION(BlueprintCallable)
	void ChangeState(ESfCharacterStateID NextStateID);

	USfCharacterState* GetState(ESfCharacterStateID StateID);

	ASfCharacter* GetCharacter() const;

	void SetWantsToRun(bool NewWantsToRun);

	bool GetWantsToRun() const;

protected:
	UPROPERTY()
	TObjectPtr<ASfCharacter> Character;

	UPROPERTY()
	TArray<USfCharacterState*> AllStates;

	UPROPERTY(BlueprintReadOnly)
	ESfCharacterStateID CurrentStateID;

	UPROPERTY()
	TObjectPtr<USfCharacterState> CurrentState;

	UPROPERTY()
	bool WantsToRun;

	void FindStates();

	void InitStates();
};
