// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Worker.generated.h"

USTRUCT()
struct FWorkerOptional
{
	GENERATED_BODY()

	TOptional<int> productionSiteID;
};

USTRUCT(BlueprintType)
struct FWorkerSaveData
{
	GENERATED_BODY()

	FWorkerSaveData() {  }

	FWorkerSaveData(FVector _position, FRotator _rotation, USkeletalMesh* _mesh, int _workerID, EWorkerStatus _employmentStatus, int _productionSiteID)
	{
		workerLocation = _position;
		workerRotation = _rotation;
		workerMesh = _mesh;
		workerID = _workerID;
		employmentStatus = _employmentStatus;
		productionSiteSaveID = _productionSiteID;
	}

private:
	UPROPERTY()
		FVector workerLocation;
	UPROPERTY()
		FRotator workerRotation;
	UPROPERTY()
		USkeletalMesh* workerMesh;
	UPROPERTY()
		int workerID;
	UPROPERTY()
		EWorkerStatus employmentStatus;
	UPROPERTY()
		int productionSiteSaveID;
	UPROPERTY()
		FWorkerOptional workerOptional;

public:
	FORCEINLINE
		FVector GetWorkerLocation() { return workerLocation; }
	FORCEINLINE
		FRotator GetWorkerRotation() { return workerRotation; }
	FORCEINLINE
		USkeletalMesh* GetWorkerMesh() { return workerMesh; }
	FORCEINLINE
		int GetWorkerID() { return workerID; }
	FORCEINLINE
		EWorkerStatus GetEmploymentStatus() { return employmentStatus; }
	FORCEINLINE
		int GetProductionSiteID() { return productionSiteSaveID; }
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

private:
	// Sollte in einer range von 0.005f bis 0.08f eingestellt werden (ERSTMAL), dieser wert wird nähmlich von der ResourceTick rate ABGEZOGEN
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		float productivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		class UWorkerMoodManager* moodManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		USkeletalMesh* skeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		USkeletalMeshComponent* skeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		class AWorkerController* workerController;

	// Ertsmal placeholder für später wenn der JobManager implementiert ist
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		bool bIsWorking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		int workerID;

	UPROPERTY()
		FWorkerOptional workerOptionals;

	UPROPERTY()
		class UNavigationSystemV1* navigationSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Status, meta=(AllowPrivateAccess))
		EWorkerStatus employmentStatus;

private:
	UFUNCTION()
		bool NullcheckDependencies();

public:
	FORCEINLINE
		float GetProductivity() { return productivity; }

	UFUNCTION()
		FWorkerSaveData GetWorkerSaveData();

	FORCEINLINE
		bool GetIsWorking() { return bIsWorking; }

	FORCEINLINE
		void SetEmployementStatus(EWorkerStatus _employmentStatus) { employmentStatus = _employmentStatus; }

	FORCEINLINE
		void SetProductionSiteID(int _siteID) { workerOptionals.productionSiteID = _siteID; }

	// Der worker Init braucht nicht überladen zu werden da die worker, sollte kein save game vorliegen trozdem von WorkerWorldManager aus gespawned, dieser erstellt dann eine "pseudo" save data 
	UFUNCTION()
		void InitWorker(FWorkerSaveData _saveData, UNavigationSystemV1* _navSystem, int _workerID, EWorkerStatus _employementStatus, int _siteID);
};

