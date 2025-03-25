
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

    // �÷��̾� ID�� ��ȯ�ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Player")
    int32 GetMyPlayerId() const { return MyPlayerId; }

    // �÷��̾� ID�� �����ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "Player")
    void SetMyPlayerId(int32 NewId) { MyPlayerId = NewId; }


    // �÷��̾� ID ���� (�ʱⰪ -1)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
    int32 MyPlayerId;
};