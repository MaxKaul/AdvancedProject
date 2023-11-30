// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeLibrary.h"
#include "EnumLibrary.h"
#include "TableBaseLibrary.h"
#include "GameFramework/Character.h"
#include "Worker.generated.h"

USTRUCT()
struct FWorkerOptional
{
	GENERATED_BODY()

	TOptional<int> productionSiteID;
	TOptional<class AProductionsite*> productionSiteRef;
};

USTRUCT(BlueprintType)
struct FWorkerSaveData
{
	GENERATED_BODY()

	FWorkerSaveData(){};

	FWorkerSaveData(FVector _position, FRotator _rotation, USkeletalMesh* _mesh, int _workerID, EWorkerStatus _employmentStatus, int _productionSiteID, EPlayerIdent _workerOwner, TArray<FWorkerDesireBase> _desireBase)
	{
		workerLocation = _position;
		workerRotation = _rotation;
		workerMesh = _mesh;
		workerID = _workerID;
		employmentStatus = _employmentStatus;
		productionSiteSaveID = _productionSiteID;
		workerOwner = _workerOwner;
		desireBase = _desireBase;
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
	UPROPERTY()
		EPlayerIdent workerOwner;
	UPROPERTY()
		TArray<FWorkerDesireBase> desireBase;

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
	FORCEINLINE
		EPlayerIdent GetWorkerOwner() { return workerOwner; }
	FORCEINLINE
		TArray<FWorkerDesireBase> GetDesireBase() { return desireBase; }
};

UCLASS()
class ADVANCEDPROJECT_API AWorker : public ACharacter
{
	GENERATED_BODY()

public:
	AWorker();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* _overlapComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIdx, bool _bFromSweep, const FHitResult& _sweepResult) ;
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* _overlapComp, AActor* _otherActor, UPrimitiveComponent* _otherComp, int32 _otherBodyIdx) ;


public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		int GetWorkerID();

	UFUNCTION(BlueprintCallable)
		void SetWorkerOwner(EPlayerIdent _newOwner);

private:
	// Sollte in einer range von 0.005f bis 0.08f eingestellt werden (ERSTMAL), dieser wert wird nähmlich von der ResourceTick rate ABGEZOGEN
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		float productivity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		class UWorkerMoodManager* moodManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		USkeletalMesh* skeletalMesh;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category=Comps,meta=(AllowPrivateAccess))
		UCapsuleComponent* workerHitBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		UCapsuleComponent* capsuleComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess))
		float ZOffsetForSites;

	bool testtoggle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		USkeletalMeshComponent* skeletalMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Comps, meta = (AllowPrivateAccess))
		class AWorkerController* workerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		int workerID;

	UPROPERTY(VisibleAnywhere, Category = Status, meta=(AllowPrivateAccess))
		FWorkerOptional workerOptionals;

	UPROPERTY()
		class UNavigationSystemV1* navigationSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Status, meta=(AllowPrivateAccess))
		EWorkerStatus employmentStatus;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		EPlayerIdent workerOwner;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		AProductionsite* subbedSite;

	UPROPERTY(VisibleAnywhere,Category= WorkerDesires, meta = (AllowPrivateAccess))
		TArray<FWorkerDesireBase> workerDesireBases;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float desireHunger;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float desireLuxury;

private:
	UFUNCTION()
		bool NullcheckDependencies();

	UFUNCTION()
		void State_Idle();
	UFUNCTION()
		void State_Employed_SideJob();
	UFUNCTION()
		void State_Employed_Unassigned();
	UFUNCTION()
		void State_Employed_Assigned();
	UFUNCTION()
		void State_FulfillingNeed();
	UFUNCTION()
		void State_SomethingWentWrong();

	UFUNCTION()
		void MoveOrder(FVector _movePos);

	// Idle -> Open for work i.e roaming the world
	// Employed_Unassigned -> Sollte slebes behaviour sein wie Idle, nur halt nicht open for workd
	// Employed_Assigned -> Ist einer site assigned
	// Fullfilling Needs -> In diesen state wird über den MoodManager geswitched wenn der worker eines seiner bedürfnisse befriedigen muss

	// SideJob -> Kann erstmal weggelassen werden i.e placeholder

public:
	UFUNCTION()
		void UncacheWorkerFromSite();
	UFUNCTION()
		void CacheWorkerToSite();

	FORCEINLINE
		float GetProductivity() { return productivity; }
	FORCEINLINE
		EPlayerIdent GetCurrentOwner() { return workerOwner; }

	UFUNCTION()
		FWorkerSaveData GetWorkerSaveData();

	UFUNCTION()
		void SetWorkerState(EWorkerStatus _employmentStatus, AProductionsite* _site = nullptr);

	UFUNCTION(BlueprintCallable) FORCEINLINE
		EWorkerStatus GetEmployementStatus() { return employmentStatus; }

	FORCEINLINE
		void SetProductionSiteInfo(int _siteID, AProductionsite* _siteRef) { workerOptionals.productionSiteID = _siteID; workerOptionals.productionSiteRef = _siteRef; }

	UFUNCTION(BlueprintCallable) FORCEINLINE
		AProductionsite* GetProductionSiteRef() { return workerOptionals.productionSiteRef.GetValue(); }

	// Der worker Init braucht nicht überladen zu werden da die worker, sollte kein save game vorliegen trozdem von WorkerWorldManager aus gespawned, dieser erstellt dann eine "pseudo" save data
	// Hat den naxchteil das ich im moment die site id immer mit -1 ini weil die auch einen default braucht
	UFUNCTION()
		void InitWorker(FWorkerSaveData _saveData, UNavigationSystemV1* _navSystem, int _workerID, EWorkerStatus _employementStatus, int _siteID, EPlayerIdent _workerOwner, TArray<FWorkerDesireBase> _desireBase);
};

