// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class UTextBlock;
class UButton;


UCLASS()
class BASEBALLGAME_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
    // 승리/패배 메시지를 갱신하는 함수
    UFUNCTION(BlueprintCallable, Category = "GameOver")
    void UpdateGameOverText(const FString& InText);

    // 카운트다운을 시작하는 함수 (예: 10초)
    UFUNCTION(BlueprintCallable, Category = "GameOver")
    void StartCountdown(int32 Seconds);

protected:
    virtual void NativeConstruct() override;

    // 승리/패배 메시지를 표시할 텍스트 블록 (UMG에서 배치 후 IsVariable 체크)
    UPROPERTY(meta = (BindWidget))
    UTextBlock* MessageText;

    // 카운트다운을 표시할 텍스트 블록
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CountdownText;

private:
    int32 RemainingSeconds;
    FTimerHandle CountdownTimerHandle;

    UFUNCTION()
    void UpdateCountdown();
};