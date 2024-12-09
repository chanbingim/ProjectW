// Fill out your copyright notice in the Description page of Project Settings.


#include "LineTraceNotify.h"
#include "Kismet/KismetSystemLibrary.h"
#include "../../Ai_CharacterBase/Ai_CharacterBase.h"
#include "../../Ai_CharacterBase/Ai_CharacterController.h"

bool ULineTraceNotify::Received_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const
{
	Super::Received_Notify(MeshComp, Animation ,EventReference);

	auto OwnningPawn = MeshComp->GetOwner();
	if (OwnningPawn == nullptr)
		return false;

	FVector StartLayPos = OwnningPawn->GetActorLocation();
	FVector EndLayPos = StartLayPos + OwnningPawn->GetActorForwardVector() * 200.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> HitObjects;
	TEnumAsByte<EObjectTypeQuery> PawnObj = UEngineTypes::ConvertToObjectType(ECC_Pawn);
	HitObjects.Add(PawnObj);

	TArray<AActor*> IgnoreActors;

	FHitResult HitResult;
	bool Result = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), StartLayPos, EndLayPos,
		HitObjects, false, IgnoreActors, EDrawDebugTrace::None, HitResult, true, FLinearColor::Black, FLinearColor::Blue, 6.0f);

	if (Result)
	{
		auto HitPawn = Cast<AAi_CharacterBase>(HitResult.GetActor());
		//if (HitPawn != nullptr)
			//HitPawn->AIEvansionToPlayer();
	}
	return true;
}
