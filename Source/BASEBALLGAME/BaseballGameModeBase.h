
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

    // 플레이어가 입력한 숫자를 비교하여 결과 문자열 반환
    FString CheckGuess(const FString& Guess);

    // 턴 처리를 진행하고 게임 종료 조건을 체크하는 함수
    bool ProcessPlayerTurn(int32 PlayerID);

    // 현재 턴 반환
    int32 GetCurrentTurn() const { return CurrentTurn; }

    // 남은 턴 정보 (외부 접근 가능)
    UPROPERTY(BlueprintReadOnly)
    int32 TurnsLeft0;

    UPROPERTY(BlueprintReadOnly)
    int32 TurnsLeft1;

    // 컴퓨터가 만든 정답
    UPROPERTY(BlueprintReadOnly)
    FString TargetNumber;

    UFUNCTION()
    void StartGame();

    // 게임 리셋 함수
    UFUNCTION(BlueprintCallable)
    void ResetGame();

    UFUNCTION(BlueprintCallable, Category = "Game")
    bool IsGameOver() const { return bGameOver; }

    UFUNCTION(BlueprintCallable, Category = "GameOver")
    void HandleCorrectGuess(int32 WinningPlayerID);

    bool bIsInitialized;
protected:
    // 게임 상태를 초기화하는 함수
    void ResetGameVariables();

    int32 NextPlayerId;
    int32 CurrentTurn; // 0 또는 1

    // 추가: 게임 오버 상태를 추적하는 플래그
    bool bGameOver;
};