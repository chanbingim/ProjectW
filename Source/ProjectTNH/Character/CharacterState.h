#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Move UMETA(DisplayName = "Move"),
	Run UMETA(DisplayName = "Run"),
	Guard UMETA(DisplayName = "Guard"),
	GuardHit UMETA(DisplayName = "GuardHit"),
	Attack UMETA(DisplayName = "NormalAttack"),
	Missed UMETA(DisplayName = "Missed"),
	Dodge UMETA(DisplayName = "Dodge"),
	Weak UMETA(DisplayName = "Weak"),
	
	ChangeWeapon UMETA(DisplayName = "ChangeWeapon"),
	Parrying UMETA(DisplayName = "Parrying"),
	Stagger UMETA(DisplayName = "Stagger"),
	Dead UMETA(DisplayName = "Dead"),
	Stun UMETA(DisplayName = "Stun"),
	Execution UMETA(DisplayName = "Excution"),
	ExecutionRecovery UMETA(DisplayName = "ExecutionRecovery"),

	None UMETA(DisplayName = "None")
};

UENUM(BlueprintType)
enum class ECombatDirection : uint8
{
	Left UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right"),
	Crash UMETA(DisplayName = "Crash"),
	None UMETA(DisplayName = "None"),
};
