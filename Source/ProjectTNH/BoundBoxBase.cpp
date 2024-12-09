// Fill out your copyright notice in the Description page of Project Settings.


#include "BoundBoxBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"

ABoundBoxBase::ABoundBoxBase()
{
	SphereComponents = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapArea"));
}

void ABoundBoxBase::BeginPlay()
{
	Super::BeginPlay();
}

FVector ABoundBoxBase::GetRandomPointInBox()
{
	FVector Origin;
	FVector BoxExtent, randompoint;

	GetActorBounds(false, Origin, BoxExtent);

	FVector halfSize = BoxExtent * 0.5f;

	auto pos = UKismetMathLibrary::RandomPointInBoundingBox(GetActorLocation(), halfSize);
	randompoint = FVector(pos.X, pos.Y, 0.0f);

	return pos;
}
