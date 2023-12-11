// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeLibrary.h"
#include "EnumLibrary.h"
#include "TableBaseLibrary.h"
#include "GameFramework/Character.h"
#include "Worker.generated.h"

USTRUCT(BlueprintType)
struct FWorkerOptional
{
	GENERATED_BODY()

public:
	FWorkerOptional(){}
		
	TOptional<int> workProductionSiteID;
	TOptional<class AProductionsite*> productionSiteRef;

	TOptional<TArray<AMarketStall*>> possibleMarketStalls;
	TOptional<AMarketStall*> currentMarketStall;
};

USTRUCT(BlueprintType)
struct FWorkerSaveData
{
	GENERATED_BODY()

	FWorkerSaveData(){};

	FWorkerSaveData(FVector _position, FRotator _rotation, USkeletalMesh* _mesh, int _workerID, EWorkerStatus _employmentStatus, EWorkerStatus _cachedStatus, EPlayerIdent _workerOwner, TArray<FWorkerDesireBase> _desireBase,
					EResourceIdent _currentLuxuryGood, EResourceIdent _currentNutritionGood, float _desireDefaultMax, float _desireLuxury, float _desireNutrition, TArray<int> _possibleStallIDs)
	{
		s_workerLocation = _position;
		s_workerRotation = _rotation;
		s_workerMesh = _mesh;
		s_workerID = _workerID;
		s_employmentStatus = _employmentStatus;
		s_workerOwner = _workerOwner;
		s_desireBase = _desireBase;
		s_currentLuxuryGood = _currentLuxuryGood;
		s_currentNutritionGood = _currentNutritionGood;
		s_desireLuxury = _desireLuxury;
		s_desireNutrition = _desireNutrition;
		s_desireDefaultMax = _desireDefaultMax;
		s_possibleStallIDs = _possibleStallIDs;
		
	}

private:
	UPROPERTY()
		FVector s_workerLocation;
	UPROPERTY()
		FRotator s_workerRotation;
	UPROPERTY()
		USkeletalMesh* s_workerMesh;
	UPROPERTY()
		int s_workerID;
	UPROPERTY()
		EWorkerStatus s_employmentStatus;
	UPROPERTY()
		EWorkerStatus s_cachedStatus;
	UPROPERTY()
		int s_productionSiteSaveID;
	UPROPERTY()
		FWorkerOptional s_workerOptionals;
	UPROPERTY()
		EPlayerIdent s_workerOwner;
	UPROPERTY()
		TArray<FWorkerDesireBase> s_desireBase;

	UPROPERTY()
		EResourceIdent s_currentLuxuryGood;
	UPROPERTY()
		EResourceIdent s_currentNutritionGood;

	UPROPERTY()
		float s_desireLuxury;
	UPROPERTY()
		float s_desireNutrition;
	UPROPERTY()
		float s_desireDefaultMax;

	UPROPERTY()
		TArray<int> s_possibleStallIDs;

public:
	FORCEINLINE
		FVector GetWorkerLocation_S() { return s_workerLocation; }
	FORCEINLINE
		FRotator GetWorkerRotation_S() { return s_workerRotation; }
	FORCEINLINE
		USkeletalMesh* GetWorkerMesh_S() { return s_workerMesh; }
	FORCEINLINE
		int GetWorkerID_S() { return s_workerID; }
	FORCEINLINE
		EWorkerStatus GetEmploymentStatus_S() { return s_employmentStatus; }
	FORCEINLINE
		EWorkerStatus GetCachedEmploymentStatus_S() { return s_cachedStatus; }
	FORCEINLINE
		EPlayerIdent GetWorkerOwner_S() { return s_workerOwner; }
	FORCEINLINE
		TArray<FWorkerDesireBase> GetDesireBase_S() { return s_desireBase; }
	FORCEINLINE
		EResourceIdent GetCurrentLuxuryGood_S() { return s_currentLuxuryGood; }
	FORCEINLINE
		EResourceIdent GetCurrentNutritionGood_S() { return s_currentNutritionGood; }
	FORCEINLINE
		FWorkerOptional GetWorkerOptionals_S() { return s_workerOptionals; }
	FORCEINLINE
		float GetDesireLuxury() { return s_desireLuxury; }
	FORCEINLINE
		float GetDesireHunger() { return s_desireNutrition; }
	FORCEINLINE
		float GetDesireDefaultMax() { return s_desireDefaultMax; }
	FORCEINLINE
		TArray<int> GetPossibleStallIDs() { return s_possibleStallIDs; }
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

	int DEBUG;

	UFUNCTION(BlueprintCallable) FORCEINLINE
		void SetWorkerOwner(EPlayerIdent _newOwner) { workerOwner = _newOwner; }

private:
	// Sollte in einer range von 0.005f bis 0.08f eingestellt werden (ERSTMAL), dieser wert wird nähmlich von der ResourceTick rate ABGEZOGEN
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		float productivity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerInfo, meta = (AllowPrivateAccess))
		bool bProcessingTicket;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		float ownedCurrency;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		TArray<FName> possibleSockets;

	UPROPERTY(VisibleAnywhere, Category = Status, meta=(AllowPrivateAccess))
		FWorkerOptional workerOptionals;

	UPROPERTY()
		class UNavigationSystemV1* navigationSystem;

	UPROPERTY()
		TArray<int>	possibleStallIDs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Status, meta=(AllowPrivateAccess))
		EWorkerStatus currentStatus;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		EWorkerStatus previousStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		EPlayerIdent workerOwner;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		AMarketManager* marketManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		AProductionsite* subbedSite;

	UPROPERTY(VisibleAnywhere,Category= WorkerDesires, meta = (AllowPrivateAccess))
		TArray<FWorkerDesireBase> workerDesireBases;

	// Desire Values for Luxury and Hunger, to normalize between 0-100, but can also be overloaded for as long as the worker has enough money to get above 100
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float desireNutrition;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float desireLuxury;

	// Dieser wert ergibt sich aus desireNutrition,desireLuxury und dem Gesamt Kapital des workers


	// Wird über den WorkerWorldManager gesetzt (ich hab die mal in die saveData gepackt damit ich nicht noch einen seperaten parameter brauche)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float desireDefaultMax;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float workerSalary;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
	TMap<EResourceIdent, float> luxuryBiasWeightPair;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		TMap<EResourceIdent, float> nutritionBiasWeightPair;

	// Ich benutze die weights sowohl zum herleiten der moral las auch zum ticken der desire values.
	// Dies bietet sich dahingehend an das dass currenweight in relation zur momentan gewählten resource steht und
	// es daher logisch erscheint die entopie aus ihnen herzuleiten
	// Ich muss selbsetverständlich noch die besten werte dafür herrleiten, im moment normalisiere ich diese werte allerdings durch 10
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float currentWeightNutrition;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float currentWeightLuxury;
	// Ich setzte diesen wert erst mal an von -10 bis 10
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float externalMoralModifier;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		float currentWorkerMoral;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		FVector movePos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Status, meta = (AllowPrivateAccess))
		float interactionRange;

	// Ich setzte die beiden gewichtungen jedes mal wenn der worker eine neue buy order ausführt
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		EResourceIdent currentLuxuryGood;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = WorkerDesires, meta = (AllowPrivateAccess))
		EResourceIdent currentNutritionGood;

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
	// Dies ist ein Array weil es auch vorkommen soll das ein worker ein bedürfniss nach luxus und nahrung gleichzeitig stillen will und dies nicht alles an einem MarketStall machen kann
	UFUNCTION()
		TArray<AMarketStall*> ChooseMarketStalls();
	UFUNCTION()
		void ChoseCurrentBiasWeightPair();
	UFUNCTION()
		void State_SomethingWentWrong();
	UFUNCTION() FORCEINLINE
		void SetMoralModifier(float _modifier) { externalMoralModifier = _modifier; }

	// Wenn der worker sich in transit (EWorkerStatus::WS_FullfillNeed) befindet soll dies nicht durch eine entlassung unterbrochen werden
	UFUNCTION()
		void FinishFulfillNeed();
	UFUNCTION()
		void BuyResources();
	UFUNCTION()
		void ProcessBuyReturn(TArray<FResourceTransactionTicket> _tickets);
	UFUNCTION()
		FResourceTransactionTicket CalculateTicket_Luxury();
	UFUNCTION()
		FResourceTransactionTicket CalculateTicket_Nutrition();

	UFUNCTION()
		void MoveWorker(FVector _movePos);
	UFUNCTION()
		void TickWorkerDesire();
	UFUNCTION()
		void TickWorkerMotivation();
	UFUNCTION()
		void FillBiasLists();


	UFUNCTION(BlueprintCallable) FORCEINLINE
		float GetWorkerMotivation() { return currentWorkerMoral; }

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
		float GetWorkerSalary() { return workerSalary; }

	FORCEINLINE
		EPlayerIdent GetCurrentOwner() { return workerOwner; }

	UFUNCTION()
		FWorkerSaveData GetWorkerSaveData();

	UFUNCTION()
		void SetWorkerState(EWorkerStatus _employmentStatus, AProductionsite* _site = nullptr);

	UFUNCTION(BlueprintCallable) FORCEINLINE
		EWorkerStatus GetEmployementStatus() { return currentStatus; }

	FORCEINLINE
		void SetProductionSiteInfo(int _siteID, AProductionsite* _siteRef) { workerOptionals.workProductionSiteID = _siteID; workerOptionals.productionSiteRef = _siteRef; }

	UFUNCTION(BlueprintCallable) FORCEINLINE
		AProductionsite* GetProductionSiteRef() { return workerOptionals.productionSiteRef.GetValue(); }

	UFUNCTION() FORCEINLINE
		void AddCurrencyToWorker(float _amount){ownedCurrency += _amount;};

	UFUNCTION()
		void InitPossibleSitesFromSave();

	// Der worker Init braucht nicht überladen zu werden da die worker, sollte kein save game vorliegen trozdem von WorkerWorldManager aus gespawned, dieser erstellt dann eine "pseudo" save data
	// Hat den naxchteil das ich im moment die site id immer mit -1 ini weil die auch einen default braucht
	UFUNCTION()
		void InitWorker(FWorkerSaveData _saveData, UNavigationSystemV1* _navSystem, int _workerID, EWorkerStatus _employementStatus, EWorkerStatus _cachedStatus, EPlayerIdent _workerOwner, TArray<FWorkerDesireBase> _desireBase, AMarketManager* _marketManager,
						TArray<int> _possibleStallsIDs);


};