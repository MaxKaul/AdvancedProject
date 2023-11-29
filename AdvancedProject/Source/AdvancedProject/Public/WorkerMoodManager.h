// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorkerMoodManager.generated.h"


USTRUCT()
struct FWorkerMoodManagerSaveData
{
	GENERATED_BODY()

	FWorkerMoodManagerSaveData() {};
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVANCEDPROJECT_API UWorkerMoodManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWorkerMoodManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		FWorkerMoodManagerSaveData GetWorkerMoodManagerSaveData();

private:
	UFUNCTION()
		void InitWorkerMoodManager(class AWorker* _owner, FWorkerMoodManagerSaveData _saveData);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		AWorker* managerOwner;
};