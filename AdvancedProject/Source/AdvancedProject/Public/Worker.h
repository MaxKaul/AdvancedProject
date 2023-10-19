// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Worker.generated.h"

USTRUCT(BlueprintType)
struct FWorkerSaveData
{
	GENERATED_BODY()

	FWorkerSaveData() {  }

	FWorkerSaveData(FVector _position, FRotator _rotation, USkeletalMesh* _mesh)
	{
		workerLocation = _position;
		workerRotation = _rotation;
		workerMesh = _mesh;
	}

private:
	UPROPERTY()
		FVector workerLocation;
	UPROPERTY()
		FRotator workerRotation;
	UPROPERTY()
		USkeletalMesh* workerMesh;

public:
	FORCEINLINE
		FVector GetWorkerLocation() { return workerLocation; }
	FORCEINLINE
		FRotator GetWorkerRotation() { return workerRotation; }
	FORCEINLINE
		USkeletalMesh* GetWorkerMesh() { return workerMesh; }
};


UCLASS()
class ADVANCEDPROJECT_API AWorker : public ACharacter
{
	GENERATED_BODY()

public:
	AWorker();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Sollte in einer range von 0.005f bis 0.08f eingestellt werden (ERSTMAL), dieser wert wird nähmlich von der ResourceTick rate ABGEZOGEN
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		float productivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		class UWorkerMoodManager* moodManager;

	// Muss vom WorkerWorldManager aus kommen
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
	//	TArray<USkeletalMesh*> possibleSkeletalMeshes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		USkeletalMesh* skeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		USkeletalMeshComponent* skeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		class AWorkerController* workerController;

	// Ertsmal placeholder für später wenn der JobManager implementiert ist
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		bool bIsWorking;

	UFUNCTION()
		bool NullcheckDependencies();

	// Der worker Init braucht nicht überladen zu werden da die worker, sollte kein save game vorliegen trozdem von WorkerWorldManager aus gespawned, dieser erstellt dann eine "pseudo" save data 
	UFUNCTION()
		void InitWorker(FWorkerSaveData _saveData);

public:
	FORCEINLINE
		float GetProductivity() { return productivity; }

	UFUNCTION()
		FWorkerSaveData GetWorkerSaveData();

	FORCEINLINE
		bool GetIsWorking() { return bIsWorking; }
};

