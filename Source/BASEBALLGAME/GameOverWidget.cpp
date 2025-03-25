#include "GameOverWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UGameOverWidget::NativeConstruct()
{


    // �ʱ� �޽��� ���� (���ϴ� ��� ���� ����)
    if (MessageText)
    {
        MessageText->SetText(FText::FromString(TEXT("GAME OVER")));
    }
    if (CountdownText)
    {
        CountdownText->SetText(FText::FromString(TEXT("")));
    }
}

void UGameOverWidget::UpdateGameOverText(const FString& InText)
{
    if (MessageText)
    {
        MessageText->SetText(FText::FromString(InText));
    }
}

void UGameOverWidget::StartCountdown(int32 Seconds)
{
    RemainingSeconds = Seconds;

    // ù ������Ʈ ��� ����
    UpdateCountdown();

    // 1�ʸ��� UpdateCountdown�� �ݺ� ȣ��
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            CountdownTimerHandle,
            this,
            &UGameOverWidget::UpdateCountdown,
            1.0f,
            true
        );
    }
}

void UGameOverWidget::UpdateCountdown()
{
    if (RemainingSeconds > 0)
    {
        if (CountdownText)
        {
            FString DisplayText = FString::Printf(TEXT("Restarting in %d seconds."), RemainingSeconds);
            CountdownText->SetText(FText::FromString(DisplayText));
        }
        RemainingSeconds--;
    }
    else
    {
        // Ÿ�̸� ����
        if (GetWorld())
        {
            GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
        }
        if (CountdownText)
        {
            CountdownText->SetText(FText::FromString(TEXT("ReStart Game")));
        }
    }
}