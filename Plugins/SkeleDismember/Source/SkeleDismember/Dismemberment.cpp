// Fill out your copyright notice in the Description page of Project Settings.


#include "Dismemberment.h"

#include "CableComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "SkeletonDataAsset.h"
#include "Engine/StaticMeshActor.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"


// Sets default values for this component's properties
UDismemberment::UDismemberment()
{

}

//Find Skeletal Mesh to Allow Dismemberment
void UDismemberment::BeginPlay()
{
	SkeleMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if(!SkeleMesh)
		return;
	
	InitialiseBones();
}

//Create Bone Structure & Set Collision Responses
void UDismemberment::InitialiseBones()
{
	if(!SkeletonData)
		return;
	SkeleMesh->SetSkeletalMeshAsset(SkeletonData->SkeletalMesh);
	SkeleMesh->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
	Limbs = SkeletonData->Limbs;
}

void UDismemberment::Handle_LimbHit(FName HitBoneName, float Damage)
{
	int LimbIndex = GetLimbIndexFromBoneName(HitBoneName);
	
	if(LimbIndex == -1 || Limbs[LimbIndex].HasDetached)
		return;

	//Take Damage to the Limb & Check if its hit 0
	FMath::Clamp(Limbs[LimbIndex].CurrentHealth -= Damage,0,Limbs[LimbIndex].CurrentHealth);
	if(!Limbs[LimbIndex].CurrentHealth == 0)
		return;

	SkeleMesh->HideBoneByName(Limbs[LimbIndex].LimbRootName,PBO_Term);
	
	//Detach Limb Forever if MAX repairs Reached
	if(Limbs[LimbIndex].CurrentRepairs >= Limbs[LimbIndex].MaxRepairs)
	{
		Handle_OnLimbSevered(Limbs[LimbIndex]);
		OnLimbSevered.Broadcast(Limbs[LimbIndex]);
	}
	else
	{
		Handle_OnLimbRemoved(Limbs[LimbIndex]);
		OnLimbRemoved.Broadcast(Limbs[LimbIndex]);
	}
	Limbs[LimbIndex].HasDetached = true;
	
}

//When the Limb has been severed and can no longer be repaired
void UDismemberment::Handle_OnLimbSevered(FLimbGroupData Limb)
{
	FTransform BoneTrans = GetLimbTransform(Limb.LimbRootName);
	FVector BoneDir = BoneTrans.GetLocation() - GetBoneParentTransform(Limb.LimbRootName).GetLocation();
	FRotator BoneRot = FRotationMatrix::MakeFromX(BoneDir).Rotator();
	
	SpawnParticles(BoneTrans.GetLocation(),BoneRot);
	SpawnMesh(BoneTrans.GetLocation(),SkeleMesh->GetRelativeRotation(),Limb);
}

//When the limb has been removed and can be destroyed [Tether Bool :)]
void UDismemberment::Handle_OnLimbRemoved(FLimbGroupData Limb)
{
	FTransform BoneTrans = GetLimbTransform(Limb.LimbRootName);
	FVector BoneDir = BoneTrans.GetLocation() - GetBoneParentTransform(Limb.LimbRootName).GetLocation();
	FRotator BoneRot = FRotationMatrix::MakeFromX(BoneDir).Rotator();

	SpawnParticles(BoneTrans.GetLocation(),BoneRot);
	AStaticMeshActor* SpawnedMesh = SpawnMesh(BoneTrans.GetLocation(),SkeleMesh->GetRelativeRotation(),Limb);
	if(Limb.bUseTetherPhysics == true)
		SpawnPhysicsTether(SpawnedMesh,Limb);
}

//Get the limb to be repaired, Rebuilds the physics body (Bit expensive but it works well)
void UDismemberment::Handle_OnLimbRepaired(FLimbGroupData Limb)
{
	int LimbIndex = GetLimbIndexFromBoneName(Limb.LimbRootName);
	if(!Limbs.IsValidIndex(LimbIndex) || Limbs[LimbIndex].HasDetached)
		return;

	OnLimbRepaired.Broadcast(Limb);
	RecreateSkeletalPhysics();
	Limbs[LimbIndex].CurrentHealth += 10;
	Limbs[LimbIndex].CurrentRepairs += 1;
	SkeleMesh->UnHideBoneByName(Limbs[LimbIndex].LimbRootName);
	for (FLimbGroupData SearchLimb : Limbs)
	{
		if(SkeleMesh->IsBoneHidden(SkeleMesh->GetBoneIndex(SearchLimb.LimbRootName)))
			SkeleMesh->TermBodiesBelow(SearchLimb.LimbRootName);
	}
}

//Repair all limbs, this is cheaper than OnLimbRepaired
void UDismemberment::RepairAllLimbs()
{
	for (int i = 0; i < Limbs.Num();i++)
	{
		Limbs[i].CurrentRepairs = 0;
		Limbs[i].HasDetached = false;
		Limbs[i].CurrentHealth = Limbs[i].MaxHealth;
		SkeleMesh->UnHideBoneByName(Limbs[i].LimbRootName);
	}
	RecreateSkeletalPhysics();
}

//Spawn Mesh and set defaults
AStaticMeshActor* UDismemberment::SpawnMesh_Implementation(FVector Location, FRotator Rotation,FLimbGroupData Limb)
{
	if(Limb.Mesh == nullptr)
		return nullptr;

	//Create static mesh and spawn at Bone socket (Pre baked mesh)
	AStaticMeshActor* LimbMesh = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
	LimbMesh->SetMobility(EComponentMobility::Movable);
	LimbMesh->SetActorLocation(Location);
	LimbMesh->SetActorRotation(SkeleMesh->GetRelativeRotation());
	UStaticMeshComponent* MeshComponent = LimbMesh->GetStaticMeshComponent();
	if(MeshComponent)
	{
		MeshComponent->SetStaticMesh(Limb.Mesh);
		MeshComponent->SetSimulatePhysics(true);
		MeshComponent->RegisterComponent();
		MeshComponent->SetLinearDamping(4);
		MeshComponent->SetAngularDamping(2);
	}
	OnLimbSpawned.Broadcast(Location,Rotation,Limb);
	return LimbMesh;
}

//Spawn a Particle System
UNiagaraComponent* UDismemberment::SpawnParticles_Implementation(FVector InLocation, FRotator InRotation)
{
	UNiagaraComponent* Particles=UNiagaraFunctionLibrary::SpawnSystemAttached(SkeletonData->ParticleSystem, GetOwner()->GetRootComponent(), NAME_None, InLocation, InRotation, EAttachLocation::Type::KeepWorldPosition, true);
	Particles->AttachToComponent(GetOwner()->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);
	OnSpawnParticles.Broadcast(InLocation,InRotation);
	return Particles;
}

//Spawning of tether physics if the bool is enabled (Not advised due to dodgy physiscs)
UPhysicsConstraintComponent* UDismemberment::SpawnPhysicsTether_Implementation(AStaticMeshActor* MeshToAttach, FLimbGroupData Limb)
{
	//Physics Collision Setup
	UStaticMeshComponent* MeshComp = MeshToAttach->GetStaticMeshComponent();
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetCollisionProfileName("Spectator");
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionObjectType(ECC_WorldStatic);
	MeshComp->SetSimulatePhysics(true);

	//Setup Cable Component Attachments
	FTransform SocketTransform = SkeleMesh->GetSocketTransform(Limb.LimbRootName,RTS_World);


	//Setup Physics Constraint
	UPhysicsConstraintComponent* PhysicsComp = NewObject<UPhysicsConstraintComponent>(this,UPhysicsConstraintComponent::StaticClass(),TEXT("PhysicsComp"));
	if(PhysicsComp)
	{
		SocketTransform.Rotator() = SkeleMesh->GetRelativeRotation();
		PhysicsComp->SetupAttachment(SkeleMesh,Limb.LimbRootName);
		PhysicsComp->ConstraintActor1 = GetOwner();
		PhysicsComp->ConstraintActor2 = MeshToAttach;
		//Physics Setup- Convert Data to editable Struct??
		PhysicsComp->SetAngularSwing1Limit(ACM_Limited,15);
		PhysicsComp->SetAngularSwing2Limit(ACM_Limited,25);
		PhysicsComp->SetAngularTwistLimit(ACM_Limited,50);

		PhysicsComp->SetLinearXLimit(LCM_Limited,5);
		PhysicsComp->SetLinearZLimit(LCM_Limited,SkeletonData->TetherLength-10.0f);
		PhysicsComp->RegisterComponent();
	}
	return PhysicsComp;
}

//Spawn Cable Tether
UCableComponent* UDismemberment::SpawnCableTether_Implementation(AStaticMeshActor* MeshToAttach, FTransform SocketTransform, FName SocketName)
{
	UCableComponent* CableTether = NewObject<UCableComponent>(this,UCableComponent::StaticClass(),TEXT("CableComp"));
	if(CableTether)
	{
		CableTether->SetRelativeTransform(SocketTransform);
		CableTether->AttachToComponent(SkeleMesh,FAttachmentTransformRules::SnapToTargetIncludingScale,SocketName);
		CableTether->EndLocation = {0,0,0};
		CableTether->CableWidth = SkeletonData->TetherWidth;
		CableTether->CableLength = SkeletonData->TetherLength;
		CableTether->SetAttachEndToComponent(MeshToAttach->GetStaticMeshComponent(),"None");
		CableTether->RegisterComponent();
	}
	return CableTether;
}

int UDismemberment::GetLimbIndexFromBoneName(FName Bone)
{
	//Get All Root Bones
	FName CurrentBone = Bone;
	TArray<FName> RootBones;
	for (FLimbGroupData LimbData : Limbs)
		RootBones.Add(LimbData.LimbRootName);
	
	do //Check if Hit Bone is Connected to a Limb
	{
		if(RootBones.Contains(CurrentBone))
			return RootBones.IndexOfByKey(CurrentBone);		
		CurrentBone = SkeleMesh->GetParentBone(CurrentBone);
	} while(CurrentBone != "none");

	return -1;
}

void UDismemberment::RecreateSkeletalPhysics()
{
	//Remove ALL skeletal bones and recreate them - EXPENSIVE, use at own performance risk.
	SkeleMesh->TermArticulated();
	SkeleMesh->InitArticulated(GetWorld()->GetPhysicsScene());
	UE_LOG(LogTemp,Warning,TEXT("Rebuilt Skeletal Physics for %s"),*GetOwner()->GetName());
}

void UDismemberment::EvaluateLimbs(int& Arms, int& Legs, int& Heads, int& Other)
{
	for (FLimbGroupData Limb : Limbs)
	{
		switch (Limb.LimbType)
		{
		case E_LimbTypes::Arms:
			Arms++;
			break;
		case E_LimbTypes::Legs:
			Legs++;
			break;
		case E_LimbTypes::Head:
			Heads++;
			break;
		case E_LimbTypes::Other:
			Other++;
			break;
		default:
			break;
		}
	}
}