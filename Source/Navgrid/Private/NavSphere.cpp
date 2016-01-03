﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "NavGrid.h"
#include "NavSphere.h"

#include <cmath>

const float FIcoSphere::Phi = (1 + sqrt(5.0)) / 2.0;

void FIcoSphere::MakeIcosahedron()
{
	/* remove any existing geometry */
	Vertices.Empty();
	Triangles.Empty();

	/* Add vertices */
	AddVertex(-1, Phi, 0);
	AddVertex(1, Phi, 0);
	AddVertex(-1, -Phi, 0);
	AddVertex(1, -Phi, 0);

	AddVertex(0, -1, Phi);
	AddVertex(0, 1, Phi);
	AddVertex(0, -1, -Phi);
	AddVertex(0, 1, -Phi);

	AddVertex(Phi, 0, -1);
	AddVertex(Phi, 0, 1);
	AddVertex(-Phi, 0, -1);
	AddVertex(-Phi, 0, 1);

	/* Add triangles */
	// 5 faces around point 0
	AddTriangle(0, 11, 5);
	AddTriangle(0, 5, 1);
	AddTriangle(0, 1, 7);
	AddTriangle(0, 7, 10);
	AddTriangle(0, 10, 11);

	// 5 adjacent faces
	AddTriangle(1, 5, 9);
	AddTriangle(5, 11, 4);
	AddTriangle(11, 10, 2);
	AddTriangle(10, 7, 6);
	AddTriangle(7, 1, 8);

	// 5 faces around point 3
	AddTriangle(3, 9, 4);
	AddTriangle(3, 4, 2);
	AddTriangle(3, 2, 6);
	AddTriangle(3, 6, 8);
	AddTriangle(3, 8, 9);

	// 5 adjacent faces
	AddTriangle(4, 9, 5);
	AddTriangle(2, 4, 11);
	AddTriangle(6, 2, 10);
	AddTriangle(8, 6, 7);
	AddTriangle(9, 8, 1);

	//expand the sphere so we can see it when debugging
	for (FVector &Vert : Vertices)
	{
		Vert *= 100;
	}
}

void FIcoSphere::DrawDebug(const UWorld* World, const FVector &Center)
{
	FlushPersistentDebugLines(World);
	for (FVector &Vert : Vertices)
	{
		DrawDebugPoint(World, Center + Vert, 10, FColor(255, 0, 0), true);
	}
	for (FTriangle &Tri : Triangles)
	{
		for (int32 i = 0; i < 3; i++)
		{
			DrawDebugLine(World, Center + Vertices[Tri.VertexIds[i % 3]], Center + Vertices[Tri.VertexIds[(i + 1) % 3]], FColor(0, 0, 255), true);
		}
	}
}

// Sets default values
ANavSphere::ANavSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	RootComponent = SceneComponent;
}

// Called when the game starts or when spawned
void ANavSphere::BeginPlay()
{
	Super::BeginPlay();


}

// Called every frame
void ANavSphere::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ANavSphere::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	IcoSphere.MakeIcosahedron();
	IcoSphere.DrawDebug(GetWorld(), GetActorLocation());
}
