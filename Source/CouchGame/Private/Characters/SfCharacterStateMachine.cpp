// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SfCharacterStateMachine.h"

#include "SfCharacterState.h"
#include "Characters/SfCharacter.h"

void USfCharacterStateMachine::Init(ASfCharacter* InCharacter)
{
	Character = InCharacter;
	FindStates();
	InitStates();

	ChangeState(ESfCharacterStateID::Idle);
}

void USfCharacterStateMachine::Tick(float DeltaTime) const
{
	if (CurrentState == nullptr) return;
	CurrentState->StateTick(DeltaTime);
}

void USfCharacterStateMachine::ChangeState(ESfCharacterStateID NextStateID)
{
	USfCharacterState* NextState = GetState(NextStateID);

	if (NextState == nullptr) return;

	if (CurrentState != nullptr) CurrentState->StateExit(NextStateID);

	const ESfCharacterStateID PreviousStateID = CurrentStateID;
	CurrentState = NextState;
	CurrentStateID = NextStateID;

	if (CurrentState != nullptr) CurrentState->StateEnter(PreviousStateID);
}

USfCharacterState* USfCharacterStateMachine::GetState(ESfCharacterStateID StateID)
{
	for (USfCharacterState* State : AllStates)
	{
		if (StateID == State->GetStateID()) return State;
	}

	return nullptr;
}

ASfCharacter* USfCharacterStateMachine::GetCharacter() const
{
	return Character;
}

void USfCharacterStateMachine::FindStates()
{
	TMap<ESfCharacterStateID, TSubclassOf<USfCharacterState>> StatesMap = Character->GetPossibleStates();
	TArray<ESfCharacterStateID> PossibleStates;
	StatesMap.GetKeys(PossibleStates);
	
	for (const ESfCharacterStateID StateID : PossibleStates)
	{
		if (StateID == ESfCharacterStateID::None) continue;

		const TSubclassOf<USfCharacterState>* StateSubclass;
		// Actually rn we don't need to use a map and stuff, jsut an array would be fine, but for clarity and justin case we keep it
		StateSubclass = StatesMap.Find(StateID);

		USfCharacterState* NewState = NewObject<USfCharacterState>(this, *StateSubclass);

		AllStates.Add(NewState);
	}
}

void USfCharacterStateMachine::InitStates()
{
	for (USfCharacterState* State : AllStates)
	{
		State->StateInit(this);
	}
}
