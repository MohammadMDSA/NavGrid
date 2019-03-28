// Fill out your copyright notice in the Description page of Project Settings.

#include "NavGridPrivatePCH.h"

UNavLadderComponent::UNavLadderComponent(const FObjectInitializer &ObjectInitializer)
	:Super(ObjectInitializer)
{
	BottomPathPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BottomPathPoint"));
	BottomPathPoint->SetupAttachment(this);

	TopPathPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TopPathPoint"));
	TopPathPoint->SetupAttachment(this);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(this);
}

void UNavLadderComponent::UpdateBodySetup()
{
	Super::UpdateBodySetup();

	// Update NeighbourhoodExtent
	FVector NeighbourhoodExtent = BoxExtent;
	/* make the tile wider to increase the chance of it overlapping with regular tiles */
	if (IsValid(Grid))
	{
		NeighbourhoodExtent.X = FMath::Max<float>(NeighbourhoodExtent.X, Grid->TileSize);
		NeighbourhoodExtent.Y = FMath::Max<float>(NeighbourhoodExtent.Y, Grid->TileSize / 2);
	}
	/* Make the shape slightly larger than the actual tile so it will intersect its neighbours */
	NeighbourhoodExtent += FVector(15);
	NeighbourhoodShape = FCollisionShape::MakeBox(NeighbourhoodExtent);

	// Update path points and PawnLocationOffset
	PawnLocationOffset = GetComponentRotation().RotateVector(FVector(90, 0, 0));
	if (IsValid(Grid))
	{
		BottomPathPoint->SetRelativeLocation(FVector(Grid->TileSize / 2, 0, 50 - BoxExtent.Z));
		TopPathPoint->SetRelativeLocation(FVector(Grid->TileSize / 2, 0, BoxExtent.Z - 25));
	}
}

FVector UNavLadderComponent::GetPawnLocation() const
{
	return (BottomPathPoint->GetComponentLocation() + TopPathPoint->GetComponentLocation()) / 2;
}

void UNavLadderComponent::GetUnobstructedNeighbours(const UCapsuleComponent & CollisionCapsule, TArray<UNavTileComponent*>& OutNeighbours)
{
	OutNeighbours.Empty();
	for (auto N : *GetNeighbours())
	{
		//Determine if we should trace from the top or bottom point
		float TopDistance = (TopPathPoint->GetComponentLocation() - N->GetPawnLocation()).Size();
		float BottomDistance = (BottomPathPoint->GetComponentLocation() - N->GetPawnLocation()).Size();
		FVector TracePoint = TopDistance < BottomDistance ? TopPathPoint->GetComponentLocation() : BottomPathPoint->GetComponentLocation();

		if (!N->Obstructed(TracePoint, CollisionCapsule))
		{
			OutNeighbours.Add(N);
		}
	}
}

bool UNavLadderComponent::Obstructed(const FVector & FromPos, const UCapsuleComponent & CollisionCapsule) const
{
	//Determine if we should trace to the top or bottom point
	float TopDistance = (TopPathPoint->GetComponentLocation() - FromPos).Size();
	float BottomDistance = (BottomPathPoint->GetComponentLocation() - FromPos).Size();
	FVector TracePoint = TopDistance < BottomDistance ? TopPathPoint->GetComponentLocation() : BottomPathPoint->GetComponentLocation();

	return UNavTileComponent::Obstructed(FromPos + CollisionCapsule.RelativeLocation, TracePoint + CollisionCapsule.RelativeLocation, CollisionCapsule);
}

bool UNavLadderComponent::Traversable(float MaxWalkAngle, const TArray<EGridMovementMode>& AvailableMovementModes) const
{
	return AvailableMovementModes.Contains(EGridMovementMode::ClimbingDown) || AvailableMovementModes.Contains(EGridMovementMode::ClimbingUp);
}

bool UNavLadderComponent::LegalPositionAtEndOfTurn(float MaxWalkAngle, const TArray<EGridMovementMode>& AvailableMovementModes) const
{
	return false;
}

void UNavLadderComponent::AddSplinePoints(const FVector &FromPos, USplineComponent &OutSpline, bool LastTile) const
{
	float TopDistance = (TopPathPoint->GetComponentLocation() - FromPos).Size();
	float BottomDistance = (BottomPathPoint->GetComponentLocation() - FromPos).Size();
	if (TopDistance > BottomDistance)
	{
		OutSpline.AddSplinePoint(BottomPathPoint->GetComponentLocation(), ESplineCoordinateSpace::Local, false);
		OutSpline.AddSplinePoint(TopPathPoint->GetComponentLocation(), ESplineCoordinateSpace::Local, false);
	}
	else
	{
		OutSpline.AddSplinePoint(TopPathPoint->GetComponentLocation(), ESplineCoordinateSpace::Local, false);
		OutSpline.AddSplinePoint(BottomPathPoint->GetComponentLocation(), ESplineCoordinateSpace::Local, false);
	}

	if (LastTile)
	{
		OutSpline.RemoveSplinePoint(OutSpline.GetNumberOfSplinePoints() - 1);
		OutSpline.AddSplinePoint(PawnLocationOffset + GetComponentLocation(), ESplineCoordinateSpace::Local, false);
	}
}

FVector UNavLadderComponent::GetSplineMeshUpVector()
{
	FRotator Rot = GetComponentRotation();
	FVector UpVector = Rot.RotateVector(FVector(0, -1, 0));
	return UpVector;
}
