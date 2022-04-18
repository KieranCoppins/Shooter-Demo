// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "Components/SphereComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create our sphere collider and make it the root component
	Collider = CreateDefaultSubobject<USphereComponent> (TEXT ("Collider"));
	RootComponent = Collider;
	Collider->InitSphereRadius (40.f);

	//Link our custom function to the on component hit event - Not sure why this doesnt worked, linked this with blueprint instead
	//Collider->OnComponentHit.AddDynamic (this, &ABullet::OnHit);

	//Create a mesh to render the bullet
	Mesh = CreateDefaultSubobject<UStaticMeshComponent> (TEXT ("Graphic"));
	Mesh->SetupAttachment (RootComponent);

	//Create a projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent> (TEXT ("Projectile Movement"));
	ProjectileMovement->InitialSpeed = 5000.0f;
	ProjectileMovement->MaxSpeed = 5000.0f;
	ProjectileMovement->Velocity = FVector (5000.0f, 0.f, 0.f);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::OnHit (UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//Check if the bullet hit a character
	if (OtherActor->ActorHasTag ("Character")) {
		//Deal damage to the character

	}

	//Destroy self regardless
	Destroy ();
}

