// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "GameFramework/Actor.h"

#include "NavTileComponent.h"
#include "GridMovementComponent.h"

#include "NavGrid.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(NavGrid, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileClicked, const UNavTileComponent*, Tile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileCursorOver, const UNavTileComponent*, Tile);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndTileCursorOver, const UNavTileComponent*, Tile);

/**
 * A grid that pawns can move around on.
 *
 */
UCLASS()
class NAVGRID_API ANavGrid : public AActor
{
	GENERATED_BODY()

public:
	ANavGrid();

	/* Collision channel used when tracing for tiles */
	static TEnumAsByte<ECollisionChannel> ECC_NavGridWalkable;
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "NavGrid")
	float TileSize = 200;
	/* Z-Offset for UI elements */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "NavGrid")
	float UIOffset = 30;
	/* Should virtual tiles be placed on empty areas */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "NavGrid")
	bool EnableVirtualTiles = false;

	// Do not place virtual tiles on actors with this tag
	static FName DisableVirtualTilesTag;
	// getter for blueprints
	UFUNCTION(BlueprintPure, Category = "NavGrid")
	FName GetDisableVirtualTilesTag() { return DisableVirtualTilesTag; }

	/* Class used for virtual tiles */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "NavGrid")
	TSubclassOf<UNavTileComponent> TileClass;

	/* Scene Component (root) */
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "Components")
	USceneComponent *SceneComponent = NULL;

	/* Cursor for highlighting tiles */
	UPROPERTY(BlueprintReadOnly, EditAnyWhere, Category = "Components")
	UStaticMeshComponent *Cursor;

protected:
	UPROPERTY()
	TMap<FName, UInstancedStaticMeshComponent *> TileHighlights;
	TMap<FName, const TCHAR *> TileHighLightPaths;
public:
	void SetTileHighlight(UNavTileComponent &Tile, FName Type);
	void ClearTileHighlights();
	void AddHighlightType(const FName &Type, const TCHAR *FileName);
	UInstancedStaticMeshComponent *GetHighlightComponent(FName Type);
public:

	/* Number of tiles that exist in the current level */
	UPROPERTY(VisibleAnywhere, Category = "NavGrid")
	int32 NumPersistentTiles = 0;
	UPROPERTY(EditAnyWhere)
	int32 MaxVirtualTiles = 10000;

	UFUNCTION(BlueprintCallable, Category = "NavGrid")
	static ANavGrid *GetNavGrid(AActor *ActorInWorld);
	static ANavGrid *GetNavGrid(UWorld *World);

	/* Get tile from world location, may return NULL */
	virtual UNavTileComponent *GetTile(const FVector &WorldLocation, bool FindFloor = true, float UpwardTraceLength = 100, float DownwardTraceLength = 100);
protected:
	UNavTileComponent *LineTraceTile(const FVector &WorldLocation, bool FindFloor, float UpwardTraceLength, float DownwardTraceLength);
	UNavTileComponent *LineTraceTile(const FVector &Start, const FVector &End);

public:
	void TileClicked(const UNavTileComponent *Tile);
	void TileCursorOver(const UNavTileComponent *Tile);
	void EndTileCursorOver(const UNavTileComponent *Tile);

protected:
	/* Do pathfinding and and store all tiles that Pawn can reach in TilesInRange */
	virtual void CalculateTilesInRange(UGridMovementComponent *Comp);
public:
	/* Find all tiles in range. Call CalculateTilesInRange if neccecary */
	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	void GetTilesInRange(UGridMovementComponent *Comp, TArray<UNavTileComponent *> &OutTiles);
	/* Reset all temporary data in all tiles in the world */
	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	void ClearTiles();
protected:
	/* Contains tiles found in the last call to CalculateTilesInRange() */
	UPROPERTY()
	TArray<UNavTileComponent *> TilesInRange;
	/* Latest Pawn passed to CalculcateTilesInRange() */
	UPROPERTY()
	UGridMovementComponent *CurrentPawn;
	/* Starting Tile for the latest call to CalculcateTilesInRange() */
	UPROPERTY()
	UNavTileComponent *CurrentTile;

public:
	/* Triggered by mouse clicks on tiles*/
	UPROPERTY(BlueprintAssignable, Category = "NavGrid")
	FOnTileClicked OnTileClicked;
	/* Triggered when the cursor enters a tile */
	UPROPERTY(BlueprintAssignable, Category = "NavGrid")
	FOnTileCursorOver OnTileCursorOver;
	/* Triggered when the cursor leaves a tile */
	UPROPERTY(BlueprintAssignable, Category = "NavGrid")
	FOnEndTileCursorOver OnEndTileCursorOver;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	bool TraceTileLocation(const FVector & TraceStart, const FVector & TraceEnd, FVector & OutTilePos);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	UNavTileComponent *PlaceTile(const FVector &Location, AActor *TileOwner = NULL);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pathfinding")
	UNavTileComponent *ConsiderPlaceTile(const FVector &TraceStart, const FVector &TraceEnd, AActor *TileOwner = NULL);
	/* Find a place to put a tile that is close to Location and that matches the grid layout */
	FVector AdjustToTileLocation(const FVector &Location);

protected:
	UPROPERTY(VisibleAnywhere, Category = "NavGrid")
	TArray<UNavTileComponent *> VirtualTiles;
	/* place virtual tiles within the movement range of a pawn */
	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	void GenerateVirtualTiles(const UGridMovementComponent *Comp);
	/* place a single virtual tile under a pawn */
	UFUNCTION(BlueprintCallable, Category = "Pathfinding")
	void GenerateVirtualTile(const UGridMovementComponent *Comp);
	void DestroyVirtualTiles();
	virtual void Destroyed() override;
public:
	/** return every tile in the supplied world */
	static void GetEveryTile(TArray<UNavTileComponent* > &OutTiles, UWorld *World);
};
