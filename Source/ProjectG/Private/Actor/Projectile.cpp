

#include "Actor/Projectile.h"
#include "Interface/CombatInterface.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AbilitySystemLibrary.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Overlap);
	SetRootComponent(SphereCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity for the projectile

	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(RootComponent);

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(5.0f); // Set lifespan for the projectile
	SetReplicateMovement(true);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnSphereBeginOverlap);
}

void AProjectile::Destroyed()
{

}

void AProjectile::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
										int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || !OtherComp)
	{
		return; // Ignore self and invalid actors
	}

	if (OtherActor && OtherActor->Implements<UCombatInterface>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile hit: %s"), *OtherActor->GetName());

		if (HasAuthority())
		{
			// Apply damage effect to the target actor
			FDamageEffectParams DamageParams = DamageEffectParams;
			DamageParams.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
			DamageParams.SourceAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
			if (DamageParams.TargetAbilitySystemComponent && DamageParams.SourceAbilitySystemComponent)
			{
				UAbilitySystemLibrary::ApplyDamageEffect(DamageParams);
			}
		}	
	}

	Destroy();
}

