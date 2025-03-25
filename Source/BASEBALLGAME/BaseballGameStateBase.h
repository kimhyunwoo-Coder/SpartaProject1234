// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BaseballGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class BASEBALLGAME_API ABaseballGameStateBase : public AGameStateBase
{
    GENERATED_BODY()

public:
    ABaseballGameStateBase();

    // 게임 전체의 마지막 결과 (예: "1S0B")
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
    FString LastResult;

    // LastResult를 설정하는 함수
    UFUNCTION(BlueprintCallable, Category = "GameState")
    void SetLastResult(const FString& NewResult) { LastResult = NewResult; }

    // LastResult를 반환하는 함수
    UFUNCTION(BlueprintCallable, Category = "GameState")
    FString GetLastResult() const { return LastResult; }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};