// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h" 
#include "Components/Button.h"
#include "GameFramework/HUD.h" 
#include "BaseballUserWidget.generated.h"

class UEditableTextBox;
class UButton;
class UTextBlock;
class ABaseballPlayerController;

UCLASS()
class BASEBALLGAME_API UBaseballUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

    UFUNCTION(BlueprintCallable)
    void OnEnterClicked();

    UFUNCTION(BlueprintCallable)
    void UpdateResultText(const FString& NewResult);

    UFUNCTION(BlueprintCallable)
    void UpdateResultText1(const FString& NewResult);

    UFUNCTION(BlueprintCallable)
    void UpdateTurnText(const FString& NewTurnText);

    UFUNCTION(BlueprintCallable)
    void UpdateTurnsRemainingText(const FString& NewText);

    UFUNCTION(BlueprintCallable)
    void StartTurnTimer(int32 Seconds);

    UFUNCTION(BlueprintCallable)
    void UpdateTurnTimer();

    UFUNCTION(BlueprintCallable)
    void UpdateGameOverText(const FString& NewText);

    // ���Ӱ� �߰�: ����� �޽����� ǥ���ϴ� �Լ�
    UFUNCTION(BlueprintCallable)
    void ShowRestartMessage(const FString& Message);

protected:
    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* InputTextBox;

    UPROPERTY(meta = (BindWidget))
    UButton* EnterButton;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ResultTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* ResultTextBlock1;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TurnTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* RemainingTurnsTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* TimerTextBlock;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* GameOverTextBlock;

    // ���Ӱ� �߰�: ����� �ȳ� �޽����� �ؽ�Ʈ ���
    UPROPERTY(meta = (BindWidget))
    UTextBlock* RestartMessageTextBlock;

private:
    int32 CurrentTurnTime;
    FTimerHandle TurnTimerHandle;
};