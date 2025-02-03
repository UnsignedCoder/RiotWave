// Copyright notice...

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponDetectionInterface.generated.h"

class UWeaponHandlingComponent;

/**
 * Interface declaration required by UE's reflection system.
 * Left as minimal boilerplate since the actual interface definition is below.
 */
UINTERFACE()
class UWeaponDetectionInterface : public UInterface {
	GENERATED_BODY()
};

class AWeaponHandlingComponent;

/**
 * Interface defining the contract for actors that can interact with weapons.
 * 
 * This interface serves two main purposes:
 * 1. Provides access to weapon handling capabilities through a component-based design
 * 2. Notifies implementing classes when weapons enter their possession
 * 
 * Using an interface instead of direct component references allows for:
 * - Different weapon handling implementations for different actor types
 * - Easy testing through mock implementations
 * - Decoupling of weapon pickup logic from specific actor implementations
 */
class RIOTWAVE_API IWeaponDetectionInterface {
	GENERATED_BODY()

public:
	/**
	 * Retrieves the component responsible for weapon handling.
	 * Default implementation returns nullptr to make the interface optional.
	 * 
	 * @return The weapon handling component, if one exists
	 */
	virtual UWeaponHandlingComponent* GetWeaponHandlingComponent() { return nullptr; }

	/**
	 * Called when a weapon is picked up by an actor.
	 * Empty default implementation allows classes to implement only the methods they need.
	 * 
	 * @param OwningActor - The actor that picked up the weapon
	 */
	virtual void OnWeaponPicked(AActor* OwningActor) {}
};