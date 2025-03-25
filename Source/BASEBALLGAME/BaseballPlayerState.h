
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BaseballPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BASEBALLGAME_API ABaseballPlayerState : public APlayerState
{
	GENERATED_BODY()


public:
    ABaseballPlayerState();

    // 플레이어 ID를 반환하는 함수
    UFUNCTION(BlueprintCallable, Category = "Player")
    int32 GetMyPlayerId() const { return MyPlayerId; }

    // 플레이어 ID를 설정하는 함수
    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetMyPlayerId(int32 NewId) { MyPlayerId = NewId; }


    // 플레이어 ID 변수 (초기값 -1)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    int32 MyPlayerId;
};