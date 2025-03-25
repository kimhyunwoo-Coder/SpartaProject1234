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

    // ���� ��ü�� ������ ��� (��: "1S0B")
    UPROPERTY(Replicated, BlueprintReadOnly, Category = "GameState")
    FString LastResult;

    // LastResult�� �����ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "GameState")
    void SetLastResult(const FString& NewResult) { LastResult = NewResult; }

    // LastResult�� ��ȯ�ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "GameState")
    FString GetLastResult() const { return LastResult; }

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};