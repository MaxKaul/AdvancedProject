// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WorkerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCEDPROJECT_API UWorkerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	void NativeBeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = AnimationProperties)
		void UpdateAnimationProperties();

	

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimInfo, meta = (AllowPrivateAccess))
		float velocity;
	// Placeholder für wenn ich das im Worker implementiert hab
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AnimInfo, meta = (AllowPrivateAccess))
		bool bIsWorking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		class AWorker* instanceOwner;
};
