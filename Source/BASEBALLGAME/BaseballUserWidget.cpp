#include "BaseballUserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "BaseballPlayerController.h"

void UBaseballUserWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // --- ��ư/�Է�â �ʱ� ���� (Ȱ��ȭ) ---
    if (EnterButton)
    {
        // ���� ���� �ø��� ��ư�� �ٽ� Ȱ��ȭ�ϰ� OnClicked ���ε�
        EnterButton->SetIsEnabled(true);
        EnterButton->OnClicked.AddDynamic(this, &UBaseballUserWidget::OnEnterClicked);
    }
    if (InputTextBox)
    {
        // �ؽ�Ʈ�ڽ��� �ٽ� Ȱ��ȭ�ϰ� �ؽ�Ʈ�� ���ϴ�
        InputTextBox->SetIsEnabled(true);
        InputTextBox->SetText(FText::FromString(TEXT("")));
    }

    // --- ������ UI �ʱ�ȭ ---
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
        // ���� ���� �޽����� ó������ ����ϴ�.
        GameOverTextBlock->SetText(FText::FromString(TEXT("")));
    }
    if (RestartMessageTextBlock)
    {
        // ����� �ȳ� �޽����� ó���� �� ���ڿ��� �ʱ�ȭ�մϴ�.
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
        // �ð��� �� �Ǿ����Ƿ� ���� ���� ó�� RPC�� ȣ���մϴ�.
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
    // Ÿ�̸� ����
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(TurnTimerHandle);
    }
    // �Է�â �� ��ư ��Ȱ��ȭ
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