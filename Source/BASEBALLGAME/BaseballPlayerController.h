

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseballPlayerController.generated.h"


class UBaseballUserWidget;
class UGameOverWidget;
class ABaseballGameModeBase;

UCLASS()
class BASEBALLGAME_API ABaseballPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    virtual void BeginPlay() override;

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void ServerCheckGuess(const FString& Guess);

    UFUNCTION(Client, Reliable)
    void ClientReceiveResult(int32 InputPlayerID, const FString& Result);

    UFUNCTION(Server, Reliable, BlueprintCallable)
    void RequestCurrentTurn();

    UFUNCTION(Client, Reliable)
    void ClientUpdateTurn(int32 NewTurn);

    UFUNCTION(Client, Reliable)
    void ClientUpdateTurnsRemaining(int32 InTurnsLeft0, int32 InTurnsLeft1);

    UFUNCTION(Server, Reliable)
    void ServerTimeUp();

    UFUNCTION(Client, Reliable)
    void ClientGameOver(bool bWin);
    virtual void ClientGameOver_Implementation(bool bWin);

    // ���� �߰�: ���� ��ư Ŭ�� �� ȣ��Ǵ� ���� RPC
    UFUNCTION(Server, Reliable)
    void ServerResetGame();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UBaseballUserWidget> BaseballWidgetClass;

    UPROPERTY()
    UBaseballUserWidget* BaseballWidget;

    // ���� ���� �� ǥ���� GameOver ����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<class UGameOverWidget> GameOverWidgetClass;

    UPROPERTY()
    UGameOverWidget* GameOverWidget;
};