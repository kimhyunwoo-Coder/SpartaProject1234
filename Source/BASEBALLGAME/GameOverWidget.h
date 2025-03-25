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
    // �¸�/�й� �޽����� �����ϴ� �Լ�
    UFUNCTION(BlueprintCallable, Category = "GameOver")
    void UpdateGameOverText(const FString& InText);

    // ī��Ʈ�ٿ��� �����ϴ� �Լ� (��: 10��)
    UFUNCTION(BlueprintCallable, Category = "GameOver")
    void StartCountdown(int32 Seconds);

protected:
    virtual void NativeConstruct() override;

    // �¸�/�й� �޽����� ǥ���� �ؽ�Ʈ ��� (UMG���� ��ġ �� IsVariable üũ)
    UPROPERTY(meta = (BindWidget))
    UTextBlock* MessageText;

    // ī��Ʈ�ٿ��� ǥ���� �ؽ�Ʈ ���
    UPROPERTY(meta = (BindWidget))
    UTextBlock* CountdownText;

private:
    int32 RemainingSeconds;
    FTimerHandle CountdownTimerHandle;

    UFUNCTION()
    void UpdateCountdown();
};