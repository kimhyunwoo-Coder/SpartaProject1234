#include "BaseballPlayerController.h"
#include "BaseballUserWidget.h"
#include "BaseballPlayerState.h"
#include "BaseballGameModeBase.h"
#include "BaseballGameStateBase.h"   
#include "GameFramework/PlayerState.h"  
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameOverWidget.h"

void ABaseballPlayerController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("PlayerController BeginPlay: %s"), *GetName());

    if (IsLocalController() && BaseballWidgetClass)
    {
        UE_LOG(LogTemp, Log, TEXT("Creating BaseballWidget now"));
        BaseballWidget = CreateWidget<UBaseballUserWidget>(this, BaseballWidgetClass);
        if (BaseballWidget)
        {
            BaseballWidget->AddToViewport();
            UE_LOG(LogTemp, Log, TEXT("BaseballWidget successfully added to viewport!"));
            // ������ 5�� �Ŀ� StartGame()�� ȣ���ϹǷ�, ���⿡���� �׳� ������ ǥ�ø� ��
        }
    }
}

void ABaseballPlayerController::ServerCheckGuess_Implementation(const FString& Guess)
{
    ABaseballGameModeBase* GM = Cast<ABaseballGameModeBase>(UGameplayStatics::GetGameMode(this));
    if (!GM) return;

    int32 InputPlayerID = -1;
    if (PlayerState)
    {
        if (ABaseballPlayerState* BPState = Cast<ABaseballPlayerState>(PlayerState))
        {
            InputPlayerID = BPState->GetMyPlayerId();
        }
    }
    UE_LOG(LogTemp, Log, TEXT("ServerCheckGuess - InputPlayerID=%d"), InputPlayerID);

    if (InputPlayerID != GM->GetCurrentTurn())
    {
        UE_LOG(LogTemp, Warning, TEXT("Not your turn! CurrentTurn is %d"), GM->GetCurrentTurn());
        return;
    }

    bool bTurnProcessed = GM->ProcessPlayerTurn(InputPlayerID);
    if (!bTurnProcessed)
    {
        UE_LOG(LogTemp, Warning, TEXT("Turn processing failed for PlayerID=%d"), InputPlayerID);
        return;
    }

    FString Result = GM->CheckGuess(Guess);

    // ����� �����Ͽ� UI�� ǥ�� (��� Ŭ���̾�Ʈ�� ����)
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(*It);
        if (PC)
        {
            PC->ClientReceiveResult(InputPlayerID, Result);
        }
    }

    // ���� ���� ó��: "3S0B"�� �����ϸ� ����
    bool bWin = Result.StartsWith(TEXT("3S0B"));
    // �� ���� ����: �ش� �÷��̾��� ���� ���� 0�̸�
    bool bOutOfTurns = false;
    if (InputPlayerID == 0)
    {
        bOutOfTurns = (GM->TurnsLeft0 == 0);
    }
    else
    {
        bOutOfTurns = (GM->TurnsLeft1 == 0);
    }

    // ������ ���� ���
    if (bWin)
    {
        UE_LOG(LogTemp, Log, TEXT("Player %d guessed correctly!"), InputPlayerID);
        // GameMode�� HandleCorrectGuess()�� ȣ���Ͽ� ��� Ŭ���̾�Ʈ�� ���� ���� ���� ���� �� ���� Ÿ�̸� ����
        GM->HandleCorrectGuess(InputPlayerID);
        return;
    }
    // �� ������ ���� ���� ���� ���� (������ �ƴϸ鼭 �ش� �÷��̾��� ���� �� ���� ���)
    else if (bOutOfTurns)
    {
        UE_LOG(LogTemp, Log, TEXT("Player %d has no turns left!"), InputPlayerID);
        // ���� �� ������ �÷��̾�Ը� GameOver �޽��� ����
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(*It);
            if (PC && PC->PlayerState)
            {
                ABaseballPlayerState* BPState = Cast<ABaseballPlayerState>(PC->PlayerState);
                if (BPState && BPState->GetMyPlayerId() == InputPlayerID)
                {
                    PC->ClientGameOver(false);
                }
            }
        }
        return;
    }
}

void ABaseballPlayerController::ClientReceiveResult_Implementation(int32 PlayerID, const FString& Result)
{
    UE_LOG(LogTemp, Log, TEXT("ClientReceiveResult - Received from PlayerID=%d, Result=%s"), PlayerID, *Result);
    if (BaseballWidget)
    {
        if (PlayerID == 0)
        {
            BaseballWidget->UpdateResultText(Result);
        }
        else
        {
            BaseballWidget->UpdateResultText1(Result);
        }
    }
}

void ABaseballPlayerController::RequestCurrentTurn_Implementation()
{
    ABaseballGameModeBase* GM = Cast<ABaseballGameModeBase>(UGameplayStatics::GetGameMode(this));
    if (GM)
    {
        ClientUpdateTurn(GM->GetCurrentTurn());
        ClientUpdateTurnsRemaining(GM->TurnsLeft0, GM->TurnsLeft1);
    }
}

void ABaseballPlayerController::ClientUpdateTurn_Implementation(int32 NewTurn)
{
    if (BaseballWidget)
    {
        FString TurnString = (NewTurn == 0) ? TEXT("Player1 Turn") : TEXT("Player2 Turn");
        BaseballWidget->UpdateTurnText(TurnString);
        BaseballWidget->StartTurnTimer(15);
    }
}

void ABaseballPlayerController::ClientUpdateTurnsRemaining_Implementation(int32 InTurnsLeft0, int32 InTurnsLeft1)
{
    if (BaseballWidget)
    {
        if (ABaseballPlayerState* BPState = Cast<ABaseballPlayerState>(PlayerState))
        {
            int32 MyID = BPState->GetMyPlayerId();
            if (MyID == 0)
            {
                FString TurnsText = FString::Printf(TEXT("Player1: %d turn(s) left"), InTurnsLeft0);
                BaseballWidget->UpdateTurnsRemainingText(TurnsText);
            }
            else
            {
                FString TurnsText = FString::Printf(TEXT("Player2: %d turn(s) left"), InTurnsLeft1);
                BaseballWidget->UpdateTurnsRemainingText(TurnsText);
            }
        }
    }
}

void ABaseballPlayerController::ServerTimeUp_Implementation()
{
    ABaseballGameModeBase* GM = Cast<ABaseballGameModeBase>(UGameplayStatics::GetGameMode(this));
    if (!GM) return;

    int32 MyID = -1;
    if (PlayerState)
    {
        if (ABaseballPlayerState* BPState = Cast<ABaseballPlayerState>(PlayerState))
        {
            MyID = BPState->GetMyPlayerId();
        }
    }
    if (MyID == GM->GetCurrentTurn())
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerTimeUp: Time's up for PlayerID=%d"), MyID);
        bool bTurnProcessed = GM->ProcessPlayerTurn(MyID);
        if (!bTurnProcessed)
        {
            UE_LOG(LogTemp, Warning, TEXT("ServerTimeUp: Turn processing failed for PlayerID=%d"), MyID);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ServerTimeUp called but it's not my turn (MyID=%d)"), MyID);
    }
}
void ABaseballPlayerController::ServerResetGame_Implementation()
{
    // ���� ���ѿ��� GameMode�� ã�� ������ �̹� ������ ���¶�� ���� ��û�� ó���մϴ�.
    if (ABaseballGameModeBase* GM = Cast<ABaseballGameModeBase>(UGameplayStatics::GetGameMode(this)))
    {
        // bGameOver�� true���� Ȯ���մϴ�. (���� �÷��̾� ��� ���� ���� ������ ���� GameOverWidget�� ǥ�õǹǷ�)
        if (GM->IsGameOver())
        {
            // ��� Ŭ���̾�Ʈ�� �������� ���� ����(���� ��ε�)�� �����մϴ�.
            GM->ResetGame();
        }
    }
}

void ABaseballPlayerController::ClientGameOver_Implementation(bool bWin)
{
    // ���� UI ����
    if (BaseballWidget)
    {
        BaseballWidget->RemoveFromParent();
        BaseballWidget = nullptr;
    }

    // GameOver ���� ����
    if (GameOverWidgetClass)
    {
        GameOverWidget = CreateWidget<UGameOverWidget>(this, GameOverWidgetClass);
        if (GameOverWidget)
        {
            GameOverWidget->AddToViewport();

            // "YOU WIN!" �Ǵ� "GAME OVER" �޽����� ǥ�� (�ʿ� ��)
            FString Message = bWin ? TEXT("YOU WIN!") : TEXT("GAME OVER");
            GameOverWidget->UpdateGameOverText(Message);

            // ���⼭ 10�� ī��Ʈ�ٿ� ����
            GameOverWidget->StartCountdown(10);
        }
    }
}