// Fill out your copyright notice in the Description page of Project Settings.

#include "NavGrid.h"
#include "TurnComponent.h"

void UTurnComponent::TurnStart()
{
	OnTurnStartEvent.Broadcast();
}

void UTurnComponent::TurnEnd()
{
	OnTurnEndEvent.Broadcast();
}

void UTurnComponent::RoundStart()
{
	OnRoundStartEvent.Broadcast();
}