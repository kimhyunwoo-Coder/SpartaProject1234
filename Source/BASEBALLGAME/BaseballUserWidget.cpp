#include "BaseballUserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "BaseballPlayerController.h"

void UBaseballUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // --- 버튼/입력창 초기 상태 (활성화) ---
    if (EnterButton)
    {
        // 게임 시작 시마다 버튼을 다시 활성화하고 OnClicked 바인딩
        EnterButton->SetIsEnabled(true);
        EnterButton->OnClicked.AddDynamic(this, &UBaseballUserWidget::OnEnterClicked);
    }
    if (InputTextBox)
    {
        // 텍스트박스도 다시 활성화하고 텍스트를 비웁니다
        InputTextBox->SetIsEnabled(true);
        InputTextBox->SetText(FText::FromString(TEXT("")));
    }

    // --- 나머지 UI 초기화 ---
    if (ResultTextBlock)
    {
        ResultTextBlock->SetText(FText::FromString(TEXT("")));
    }

    if (TurnTextBlock)
    {
        TurnTextBlock->SetText(FText::FromString(TEXT("")));
    }
    if (RemainingTurnsTextBlock)
    {
        RemainingTurnsTextBlock->SetText(FText::FromString(TEXT("")));
    }
    if (TimerTextBlock)
    {
        TimerTextBlock->SetText(FText::FromString(TEXT("")));
    }
    if (GameOverTextBlock)
    {
        // 게임 종료 메시지는 처음에는 숨깁니다.
        GameOverTextBlock->SetText(FText::FromString(TEXT("")));
    }
    if (RestartMessageTextBlock)
    {
        // 재시작 안내 메시지도 처음엔 빈 문자열로 초기화합니다.
        RestartMessageTextBlock->SetText(FText::FromString(TEXT("")));
    }
}

void UBaseballUserWidget::OnEnterClicked()
{
    FString UserInput;
    if (InputTextBox)
    {
        UserInput = InputTextBox->GetText().ToString();
    }

    if (ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(GetOwningPlayer()))
    {
        PC->ServerCheckGuess(UserInput);
    }
}

void UBaseballUserWidget::UpdateResultText(const FString& NewResult)
{
    if (ResultTextBlock)
    {
        FString CurrentText = ResultTextBlock->GetText().ToString();
        if (!CurrentText.IsEmpty())
        {
            CurrentText.Append(TEXT("\n"));
        }
        CurrentText.Append(NewResult);
        ResultTextBlock->SetText(FText::FromString(CurrentText));
    }
}

void UBaseballUserWidget::UpdateResultText1(const FString& NewResult)
{
    if (ResultTextBlock1)
    {
        FString CurrentText = ResultTextBlock1->GetText().ToString();
        if (!CurrentText.IsEmpty())
        {
            CurrentText.Append(TEXT("\n"));
        }
        CurrentText.Append(NewResult);
        ResultTextBlock1->SetText(FText::FromString(CurrentText));
    }
}

void UBaseballUserWidget::UpdateTurnText(const FString& NewTurnText)
{
    if (TurnTextBlock)
    {
        TurnTextBlock->SetText(FText::FromString(NewTurnText));
    }
}

void UBaseballUserWidget::UpdateTurnsRemainingText(const FString& NewText)
{
    if (RemainingTurnsTextBlock)
    {
        RemainingTurnsTextBlock->SetText(FText::FromString(NewText));
    }
}

void UBaseballUserWidget::StartTurnTimer(int32 Seconds)
{
    CurrentTurnTime = Seconds;
    UpdateTurnTimer();
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TurnTimerHandle);
        World->GetTimerManager().SetTimer(TurnTimerHandle, this, &UBaseballUserWidget::UpdateTurnTimer, 1.0f, true);
    }
}

void UBaseballUserWidget::UpdateTurnTimer()
{
    if (CurrentTurnTime <= 0)
    {
        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(TurnTimerHandle);
        }
        if (TimerTextBlock)
        {
            TimerTextBlock->SetText(FText::FromString(TEXT("Time's Up")));
        }
        // 시간이 다 되었으므로 게임 종료 처리 RPC를 호출합니다.
        if (ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(GetOwningPlayer()))
        {
            PC->ServerTimeUp();
        }
    }
    else
    {
        FString TimerString = FString::Printf(TEXT("Time Left: %d sec"), CurrentTurnTime);
        if (TimerTextBlock)
        {
            TimerTextBlock->SetText(FText::FromString(TimerString));
        }
        CurrentTurnTime--;
    }
}

void UBaseballUserWidget::UpdateGameOverText(const FString& NewText)
{
    if (GameOverTextBlock)
    {
        GameOverTextBlock->SetText(FText::FromString(NewText));
    }
    // 타이머 정지
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TurnTimerHandle);
    }
    // 입력창 및 버튼 비활성화
    if (EnterButton)
    {
        EnterButton->SetIsEnabled(false);
    }
    if (InputTextBox)
    {
        InputTextBox->SetIsEnabled(false);
    }
}

void UBaseballUserWidget::ShowRestartMessage(const FString& Message)
{
    if (RestartMessageTextBlock)
    {
        RestartMessageTextBlock->SetText(FText::FromString(Message));
    }
}