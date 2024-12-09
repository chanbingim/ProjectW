#pragma once

UENUM(BlueprintType)
enum class EPawnType : uint8
{
	NormalSword UMETA(DisplayName = "NormalSword"),
	NormalSpear UMETA(DisplayName = "NormalSpear"),
	NormalGladiator UMETA(DisplayName = "NormalGladiator"),
	NormalArcher UMETA(DisplayName = "NormalArcher"),
	EliteSword UMETA(DisplayName = "EliteSword"),
	EliteSpear UMETA(DisplayName = "EliteSpear"),
	EliteGladiator UMETA(DisplayName = "EliteGladiator"),
	HighEliteSword UMETA(DisplayName = "HighEliteSword"),
	HighEliteSpear UMETA(DisplayName = "HighEliteSpear"),
	HighEliteGladiator UMETA(DisplayName = "HighEliteGladiator"),
	Player  UMETA(DisplayName = "Player"),
	Boss1 UMETA(DisplayName = "Boss1"),
	Boss2 UMETA(DisplayName = "Boss2"),
};