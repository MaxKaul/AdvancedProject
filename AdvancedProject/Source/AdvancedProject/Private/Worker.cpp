// Fill out your copyright notice in the Description page of Project Settings.


#include "Worker.h"

#include "NavigationSystem.h"
#include "Productionsite.h"
#include "WorkerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Sets default values
AWorker::AWorker()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	productivity = 0.08f;

	ZOffsetForSites = 700.f;

	workerHitBox = CreateDefaultSubobject<UCapsuleComponent>("Hit Box");
	workerHitBox->SetupAttachment(GetRootComponent());

	capsuleComp = GetCapsuleComponent();
}

// Called when the game starts or when spawned
void AWorker::BeginPlay()
{
	Super::BeginPlay();

	workerHitBox->OnComponentBeginOverlap.AddDynamic(this, &AWorker::OnOverlapBegin);
	workerHitBox->OnComponentEndOverlap.AddDynamic(this, &AWorker::OnOverlapEnd);
}

void AWorker::OnOverlapBegin(UPrimitiveComponent* _overlapComp, AActor* _otherActor, UPrimitiveComponent* _otherComp,
	int32 _otherBodyIdx, bool _bFromSweep, const FHitResult& _sweepResult)
{
	if (subbedSite && Cast<AProductionsite>(_otherActor) == subbedSite)
		subbedSite->SubscribeWorkerToOnSite(this);
}

void AWorker::OnOverlapEnd(UPrimitiveComponent* _overlapComp, AActor* _otherActor, UPrimitiveComponent* _otherComp,
	int32 _otherBodyIdx)
{

}

// Called every frame
void AWorker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (employmentStatus)
	{
	case EWorkerStatus::WS_Unemployed:
		State_Idle();
		break;
	case EWorkerStatus::WS_Assigned_SideJob:
		State_Employed_Assigned();
		break;
	case EWorkerStatus::WS_Unassigned:
		State_Employed_Unassigned();
		break;
	case EWorkerStatus::WS_Assigned_MainJob:
		State_Employed_Assigned();
		break;


	case EWorkerStatus::WS_DEFAULT:
		break;
	case EWorkerStatus::WS_ENTRY_AMOUNT:
		State_SomethingWentWrong();
		break;
	default:
		State_SomethingWentWrong();
		break;;
	}
}

int AWorker::GetWorkerID()
{
	return workerID;
}

void AWorker::SetWorkerOwner(EPlayerIdent _newOwner)
{
	workerOwner = _newOwner;
}

void AWorker::InitWorker(FWorkerSaveData _saveData, UNavigationSystemV1* _navSystem, int _workerID, EWorkerStatus _employementStatus, int _siteID, EPlayerIdent _workerOwner)
{
	workerID = _workerID;
	workerOptionals.productionSiteID  = _siteID;
	employmentStatus = _employementStatus;

	skeletalMeshComponent = GetMesh();
	
	skeletalMesh = _saveData.GetWorkerMesh();

	skeletalMeshComponent->SetSkeletalMesh(skeletalMesh, true);
	workerController = Cast<AWorkerController>(GetController());
	navigationSystem = _navSystem;
	workerOwner = _workerOwner;
}

FWorkerSaveData AWorker::GetWorkerSaveData()
{
	FWorkerSaveData savedata =
	{
		GetActorLocation(),
		GetActorRotation(),
		skeletalMesh,
		workerID,
		employmentStatus,
		workerOptionals.productionSiteID.GetValue(),
		workerOwner
	};

	return savedata;
}

void AWorker::SetWorkerState(EWorkerStatus _employmentStatus, AProductionsite* _site)
{
	employmentStatus = _employmentStatus;

	workerController->StopMovement();

	if (_site && employmentStatus == EWorkerStatus::WS_Assigned_MainJob)
		subbedSite = _site;
	else
		subbedSite = nullptr;
}

void AWorker::State_Idle()
{
	if (GetMovementComponent()->Velocity.Length() > 0)
		return;

	FNavLocation rndloc;
	navigationSystem->GetRandomPoint(rndloc);

	workerController->MoveToLocation(rndloc);
}

void AWorker::State_Employed_Unassigned()
{
	if (GetMovementComponent()->Velocity.Length() > 0)
		return;

	FNavLocation rndloc;
	navigationSystem->GetRandomPoint(rndloc);

	workerController->MoveToLocation(rndloc);
}

void AWorker::State_Employed_SideJob()
{

}

void AWorker::State_Employed_Assigned()
{
	if (!subbedSite)
		return;

	FVector loc = subbedSite->GetActorLocation();
	loc.Z += ZOffsetForSites;

	workerController->MoveToLocation(loc);
}

void AWorker::State_FulfillingNeed()
{

}

void AWorker::State_SomethingWentWrong()
{
	UE_LOG(LogTemp, Warning, TEXT("UFF"));
}

void AWorker::MoveOrder(FVector _movePos)
{
	if (GetMovementComponent()->Velocity.Length() > 0)
		return;

	workerController->MoveToLocation(_movePos);
}

void AWorker::UncacheWorkerFromSite()
{
	capsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	workerHitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetVisibility(true);
}

void AWorker::CacheWorkerToSite()
{
	capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	workerHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetVisibility(false);
	workerController->StopMovement();
}

bool AWorker::NullcheckDependencies()
{
	bool status = true;

	return status;
}
