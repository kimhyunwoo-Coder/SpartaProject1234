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
            // 서버가 5초 후에 StartGame()을 호출하므로, 여기에서는 그냥 위젯을 표시만 함
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

    // 결과를 전파하여 UI에 표시 (모든 클라이언트에 전송)
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(*It);
        if (PC)
        {
            PC->ClientReceiveResult(InputPlayerID, Result);
        }
    }

    // 정답 조건 처리: "3S0B"로 시작하면 정답
    bool bWin = Result.StartsWith(TEXT("3S0B"));
    // 턴 소진 조건: 해당 플레이어의 남은 턴이 0이면
    bool bOutOfTurns = false;
    if (InputPlayerID == 0)
    {
        bOutOfTurns = (GM->TurnsLeft0 == 0);
    }
    else
    {
        bOutOfTurns = (GM->TurnsLeft1 == 0);
    }

    // 정답을 맞춘 경우
    if (bWin)
    {
        UE_LOG(LogTemp, Log, TEXT("Player %d guessed correctly!"), InputPlayerID);
        // GameMode의 HandleCorrectGuess()를 호출하여 모든 클라이언트에 게임 오버 상태 전파 및 리셋 타이머 설정
        GM->HandleCorrectGuess(InputPlayerID);
        return;
    }
    // 턴 소진에 의한 게임 오버 조건 (정답이 아니면서 해당 플레이어의 턴이 다 사용된 경우)
    else if (bOutOfTurns)
    {
        UE_LOG(LogTemp, Log, TEXT("Player %d has no turns left!"), InputPlayerID);
        // 오직 턴 소진한 플레이어에게만 GameOver 메시지 전파
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
    // 서버 권한에서 GameMode를 찾아 게임이 이미 오버된 상태라면 리셋 요청을 처리합니다.
    if (ABaseballGameModeBase* GM = Cast<ABaseballGameModeBase>(UGameplayStatics::GetGameMode(this)))
    {
        // bGameOver가 true인지 확인합니다. (양쪽 플레이어 모두 게임 오버 상태일 때만 GameOverWidget이 표시되므로)
        if (GM->IsGameOver())
        {
            // 모든 클라이언트와 서버에서 게임 리셋(레벨 재로드)을 실행합니다.
            GM->ResetGame();
        }
    }
}

void ABaseballPlayerController::ClientGameOver_Implementation(bool bWin)
{
    // 기존 UI 제거
    if (BaseballWidget)
    {
        BaseballWidget->RemoveFromParent();
        BaseballWidget = nullptr;
    }

    // GameOver 위젯 생성
    if (GameOverWidgetClass)
    {
        GameOverWidget = CreateWidget<UGameOverWidget>(this, GameOverWidgetClass);
        if (GameOverWidget)
        {
            GameOverWidget->AddToViewport();

            // "YOU WIN!" 또는 "GAME OVER" 메시지를 표시 (필요 시)
            FString Message = bWin ? TEXT("YOU WIN!") : TEXT("GAME OVER");
            GameOverWidget->UpdateGameOverText(Message);

            // 여기서 10초 카운트다운 시작
            GameOverWidget->StartCountdown(10);
        }
    }
}