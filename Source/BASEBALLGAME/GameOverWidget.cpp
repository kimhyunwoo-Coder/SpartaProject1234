#include "GameOverWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Engine/World.h"

void UGameOverWidget::NativeConstruct()
{


    // 초기 메시지 설정 (원하는 대로 수정 가능)
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

    // 첫 업데이트 즉시 실행
    UpdateCountdown();

    // 1초마다 UpdateCountdown를 반복 호출
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
        // 타이머 종료
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