// Fill out your copyright notice in the Description page of Project Settings.


#include "Dismemberment.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInstance.h"

#include "Engine/World.h"


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

	int Size = Limbs.Num();
	PhysComponents.SetNum(Size);
	CableComponents.SetNum(Size);
	Meshes.SetNum(Size);
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
	
	UNiagaraComponent* Particles = SpawnParticles(BoneTrans.GetLocation(),BoneRot,Limb.LimbRootName);
	AStaticMeshActor* Mesh = SpawnMesh(BoneTrans.GetLocation(),SkeleMesh->GetRelativeRotation(),Limb);

	int Index = GetLimbIndexFromBoneName(Limb.LimbRootName);
	if(Index ==-1)
		return;
	UpdateLimbRefs(Index,Mesh,nullptr,nullptr);

}

//When the limb has been removed and can be destroyed [Tether Bool :)]
void UDismemberment::Handle_OnLimbRemoved(FLimbGroupData Limb)
{
	FTransform BoneTrans = GetLimbTransform(Limb.LimbRootName);
	FVector BoneDir = BoneTrans.GetLocation() - GetBoneParentTransform(Limb.LimbRootName).GetLocation();
	FRotator BoneRot = FRotationMatrix::MakeFromX(BoneDir).Rotator();

	AStaticMeshActor* SpawnedMesh = SpawnMesh(BoneTrans.GetLocation(),SkeleMesh->GetRelativeRotation(),Limb);
	UNiagaraComponent* Particles = SpawnParticles(BoneTrans.GetLocation(),BoneRot,Limb.LimbRootName);
	UPhysicsConstraintComponent* PhysComp = nullptr;
	UCableComponent* Cable = nullptr;
	
	if(Limb.bUseTetherPhysics == true)
	{
		PhysComp = SpawnPhysicsTether(SpawnedMesh,Limb);
		Cable = SpawnCableTether(SpawnedMesh,Limb.LimbRootName);
	}

	int Index = GetLimbIndexFromBoneName(Limb.LimbRootName);
	if(Index ==-1)
		return;
	UpdateLimbRefs(Index,SpawnedMesh,PhysComp,Cable);
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
	UpdateLimbRefs(LimbIndex,nullptr,nullptr,nullptr);
	for (FLimbGroupData SearchLimb : Limbs)
	{
		if(SkeleMesh->IsBoneHidden(SkeleMesh->GetBoneIndex(SearchLimb.LimbRootName))){}
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
		UpdateLimbRefs(i,nullptr,nullptr,nullptr);
	}
	OnLimbRepaired.Broadcast(Limbs[0]);
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
	UStaticMeshComponent* MeshComp = LimbMesh->GetStaticMeshComponent();
	if(MeshComp)
	{
		MeshComp->SetStaticMesh(Limb.Mesh);
		MeshComp->SetCollisionProfileName("Spectator");
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetCollisionObjectType(ECC_WorldStatic);
		MeshComp->SetSimulatePhysics(true);
	
	}
	OnLimbSpawned.Broadcast(Location,Rotation,Limb);
	return LimbMesh;
}

//Spawn a Particle System
UNiagaraComponent* UDismemberment::SpawnParticles_Implementation(FVector InLocation, FRotator InRotation, FName Socket)
{
	if(SkeletonData->ParticleSystem == nullptr)
		return nullptr;
	UNiagaraComponent* Particles=UNiagaraFunctionLibrary::SpawnSystemAttached(SkeletonData->ParticleSystem, GetOwner()->GetRootComponent(), NAME_None, InLocation, InRotation, EAttachLocation::Type::KeepWorldPosition, true);
	Particles->AttachToComponent(SkeleMesh,FAttachmentTransformRules::KeepWorldTransform,Socket);

	OnSpawnParticles.Broadcast(InLocation,InRotation);
	GetOwner()->AddInstanceComponent(Particles);
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
	MeshComp->SetLinearDamping(4);
	MeshComp->SetAngularDamping(2);

	//Setup Cable Component Attachments
	FTransform SocketTransform = SkeleMesh->GetSocketTransform(Limb.LimbRootName,RTS_World);


	//Setup Physics Constraint
	FString CompName = "CableComp" + Limb.LimbRootName.ToString();

	FAttachmentTransformRules Rules = {EAttachmentRule::SnapToTarget,true};
	Rules.RotationRule = EAttachmentRule::KeepRelative;
	Rules.ScaleRule = EAttachmentRule::KeepWorld;
	
	UPhysicsConstraintComponent* PhysicsComp = NewObject<UPhysicsConstraintComponent>(this,UPhysicsConstraintComponent::StaticClass());
	if(PhysicsComp)
	{
		SocketTransform.Rotator() = SkeleMesh->GetRelativeRotation();
		
		PhysicsComp->AttachToComponent(SkeleMesh,Rules,Limb.LimbRootName);

		//PhysicsComp->ConstraintActor1 = GetOwner();
		PhysicsComp->OverrideComponent1 = SkeleMesh;
		PhysicsComp->ComponentName1.ComponentName = SkeleMesh.GetFName();
		PhysicsComp->ConstraintActor2 = MeshToAttach;
		PhysicsComp->InitComponentConstraint();
		//PhysicsComp->SetConstrainedComponents(SkeleMesh,Limb.LimbRootName,MeshToAttach->GetStaticMeshComponent(),"");

		//Physics Setup- Convert Data to editable Struct??
		PhysicsComp->SetAngularSwing1Limit(ACM_Limited,25);
		PhysicsComp->SetAngularSwing2Limit(ACM_Limited,45);
		PhysicsComp->SetAngularTwistLimit(ACM_Limited,60);

		PhysicsComp->SetLinearXLimit(LCM_Limited,5);
		PhysicsComp->SetLinearZLimit(LCM_Limited,SkeletonData->TetherLength-10.0f);
		GetOwner()->AddInstanceComponent(PhysicsComp);
		GetOwner()->FinishAndRegisterComponent(PhysicsComp);
	}
	return PhysicsComp;
}

//Spawn Cable Tether
UCableComponent* UDismemberment::SpawnCableTether_Implementation(AStaticMeshActor* MeshToAttach, FName SocketName)
{
	FTransform SocketTransform = SkeleMesh->GetSocketTransform(SocketName,RTS_World);
	FString CompName = "CableComp" + SocketName.ToString();
	
	UCableComponent* CableTether = NewObject<UCableComponent>(this,UCableComponent::StaticClass());
	if(CableTether)
	{
		CableTether->SetRelativeTransform(SocketTransform);
		CableTether->AttachToComponent(SkeleMesh,FAttachmentTransformRules::SnapToTargetIncludingScale,SocketName);
		CableTether->EndLocation = {0,0,0};
		CableTether->CableWidth = SkeletonData->TetherWidth;
		CableTether->CableLength = SkeletonData->TetherLength;
		CableTether->SetAttachEndToComponent(MeshToAttach->GetStaticMeshComponent(),"None");
		GetOwner()->AddInstanceComponent(CableTether);
		GetOwner()->FinishAndRegisterComponent(CableTether);

		if(SkeletonData->CableMaterial != nullptr)
			CableTether->SetMaterial(0,SkeletonData->CableMaterial);
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

void UDismemberment::UpdateLimbRefs(int LimbIndex, AStaticMeshActor* Mesh, UPhysicsConstraintComponent* Phys, UCableComponent* Cable)
{


	//Remove Things if exist and input is null
	if(Mesh==nullptr && Meshes[LimbIndex] != nullptr)
		Meshes[LimbIndex]->Destroy();
	if(Cable==nullptr && CableComponents[LimbIndex] != nullptr)
		CableComponents[LimbIndex]->DestroyComponent();
	if(Phys==nullptr && PhysComponents[LimbIndex] != nullptr)
		PhysComponents[LimbIndex]->DestroyComponent();

	//Assign Things
	Meshes[LimbIndex] = Mesh;
	CableComponents[LimbIndex] = Cable;
	PhysComponents[LimbIndex]=Phys;

	
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
		if(Limb.CurrentHealth<=0)
			continue;
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

	if(SkeletonData->OverrideArms != 0)
		Arms = SkeletonData->OverrideArms;

	if(SkeletonData->OverrideLegs != 0)
		Legs = SkeletonData->OverrideLegs;
}