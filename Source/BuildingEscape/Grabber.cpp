// copyright WrightStudios 2021

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}
// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();

	SetupInputComponent();

}

	void UGrabber::SetupInputComponent()
	{
		InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
		if(InputComponent)
		{
			InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
			InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
		}

	}

	void UGrabber::FindPhysicsHandle()
	{
		//Check for the physics handle component
		PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
		if (PhysicsHandle == nullptr)
		{
		UE_LOG(LogTemp, Error, TEXT("No physics handle component found on %s"), *GetOwner()->GetName());
		}

	}

	void UGrabber::Grab()
	{
		FHitResult HitResult = GetFirstPhysicsBodyInReach();
		UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
		//try and reach any actors with a physics body collision set
		if (HitResult.GetActor())
		{
		PhysicsHandle->GrabComponentAtLocation
			(
				ComponentToGrab,
				NAME_None,
				GetPlayerReach()
			);
		}
	}

	void UGrabber::Release()
	{
		PhysicsHandle->ReleaseComponent();
	}

	// Called every frame
	void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		//if the physics handle is attached
		if (PhysicsHandle->GrabbedComponent)
		{
			//Move the object player is holding
			PhysicsHandle->SetTargetLocation(GetPlayerReach());
		}
	}

	FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
	{
		FHitResult Hit;
		FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
		//Ray-cast out to a certain distance(reach)
		GetWorld()->LineTraceSingleByObjectType(
			Hit,
			GetPlayerWorldPosition(),
			GetPlayerReach(),
			FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
			TraceParams
		);
		return Hit;
	}

	//Gets players position in world
	FVector UGrabber::GetPlayerWorldPosition() const
	{
		FVector PlayerViewPointLocation;
		FRotator PlayerViewPointRotation;

		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
			OUT PlayerViewPointLocation,
			OUT PlayerViewPointRotation
		);

		//Logging out to test
		// UE_LOG(LogTemp, Warning, TEXT("Location: %s Rotation: %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString());

		return PlayerViewPointLocation;
	}

	FVector UGrabber::GetPlayerReach() const
	{
		FVector PlayerViewPointLocation;
		FRotator PlayerViewPointRotation;

		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
		);

		//Logging out to test
		// UE_LOG(LogTemp, Warning, TEXT("Location: %s Rotation: %s"), *PlayerViewPointLocation.ToString(), *PlayerViewPointRotation.ToString());

		//Draw a line from player showing the reach
		return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
	
	}


