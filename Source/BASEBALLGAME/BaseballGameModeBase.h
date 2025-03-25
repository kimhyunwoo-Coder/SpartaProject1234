
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseballLogic.h"
#include "BaseballGameModeBase.generated.h"

class UUserWidget;  
class ABaseballGameStateBase; 

UCLASS()
class BASEBALLGAME_API ABaseballGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

    
public:
    ABaseballGameModeBase();

    virtual void BeginPlay() override;
    virtual void PostLogin(APlayerController* NewPlayer) override;

    // �÷��̾ �Է��� ���ڸ� ���Ͽ� ��� ���ڿ� ��ȯ
    FString CheckGuess(const FString& Guess);

    // �� ó���� �����ϰ� ���� ���� ������ üũ�ϴ� �Լ�
    bool ProcessPlayerTurn(int32 PlayerID);

    // ���� �� ��ȯ
    int32 GetCurrentTurn() const { return CurrentTurn; }

    // ���� �� ���� (�ܺ� ���� ����)
    UPROPERTY(BlueprintReadOnly)
    int32 TurnsLeft0;

    UPROPERTY(BlueprintReadOnly)
    int32 TurnsLeft1;

    // ��ǻ�Ͱ� ���� ����
    UPROPERTY(BlueprintReadOnly)
    FString TargetNumber;

    UFUNCTION()
    void StartGame();

    // ���� ���� �Լ�
    UFUNCTION(BlueprintCallable)
    void ResetGame();

    UFUNCTION(BlueprintCallable, Category = "Game")
    bool IsGameOver() const { return bGameOver; }

    UFUNCTION(BlueprintCallable, Category = "GameOver")
    void HandleCorrectGuess(int32 WinningPlayerID);

    bool bIsInitialized;
protected:
    // ���� ���¸� �ʱ�ȭ�ϴ� �Լ�
    void ResetGameVariables();

    int32 NextPlayerId;
    int32 CurrentTurn; // 0 �Ǵ� 1

    // �߰�: ���� ���� ���¸� �����ϴ� �÷���
    bool bGameOver;
};