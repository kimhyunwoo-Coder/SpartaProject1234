#include "BaseballGameModeBase.h"
#include "BaseballGameStateBase.h"
#include "BaseballUserWidget.h"
#include "BaseballPlayerState.h"
#include "BaseballPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Misc/PackageName.h"  // FPackageName ���

ABaseballGameModeBase::ABaseballGameModeBase()
{
    bUseSeamlessTravel = true;

    PlayerStateClass = ABaseballPlayerState::StaticClass();
    NextPlayerId = 0;

    bIsInitialized = false;
    // �����ڿ����� ResetGameVariables()�� ȣ������ �ʽ��ϴ�.
}

void ABaseballGameModeBase::BeginPlay()
{
    Super::BeginPlay();

    // ���� ������ �ְ�, ���� �ʱ�ȭ�� �� �� ��쿡�� �ʱ�ȭ
    if (HasAuthority() && !bIsInitialized)
    {
        bIsInitialized = true;
        ResetGameVariables();
    }

    // ���� ���� �� 5�� �ڿ� ���� ���� (��� Ŭ���̾�Ʈ�� �ʱ� ���� ����)
    FTimerHandle StartGameTimerHandle;
    GetWorldTimerManager().SetTimer(StartGameTimerHandle, this, &ABaseballGameModeBase::StartGame, 5.0f, false);
}

void ABaseballGameModeBase::ResetGameVariables()
{
    // ���� ���ѿ����� ����
    if (!HasAuthority())
    {
        return;
    }

    CurrentTurn = 0;
    TurnsLeft0 = 3;
    TurnsLeft1 = 3;
    bGameOver = false;

    // ���� �õ� �ʱ�ȭ
    FMath::RandInit(FDateTime::Now().GetMillisecond());

    // 3�ڸ� ���� ���� (�ߺ� ���� 1~9)
    TArray<int32> Digits;
    while (Digits.Num() < 3)
    {
        int32 RandomDigit = FMath::RandRange(1, 9);
        if (!Digits.Contains(RandomDigit))
        {
            Digits.Add(RandomDigit);
        }
    }
    TargetNumber = FString::FromInt(Digits[0]) +
        FString::FromInt(Digits[1]) +
        FString::FromInt(Digits[2]);

    UE_LOG(LogTemp, Warning, TEXT("ResetGameVariables: TargetNumber is %s (server only)"), *TargetNumber);
}

void ABaseballGameModeBase::StartGame()
{
    // ��� PlayerController�� ���� �ϰ� ���� �� ������ ����
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(*It);
        if (PC)
        {
            PC->ClientUpdateTurn(CurrentTurn);
            PC->ClientUpdateTurnsRemaining(TurnsLeft0, TurnsLeft1);
        }
    }
}

FString ABaseballGameModeBase::CheckGuess(const FString& Guess)
{
    if (Guess.Len() != 3)
    {
        return FString::Printf(TEXT("OUT (%s)"), *Guess);
    }
    for (TCHAR c : Guess)
    {
        if (!FChar::IsDigit(c))
        {
            return FString::Printf(TEXT("OUT (%s)"), *Guess);
        }
    }
    int32 StrikeCount = 0;
    int32 BallCount = 0;
    for (int32 i = 0; i < 3; i++)
    {
        if (Guess[i] == TargetNumber[i])
        {
            StrikeCount++;
        }
        else if (TargetNumber.Contains(FString(1, &Guess[i])))
        {
            BallCount++;
        }
    }
    if (StrikeCount == 0 && BallCount == 0)
    {
        return FString::Printf(TEXT("OUT (%s)"), *Guess);
    }
    return FString::Printf(TEXT("%dS%dB (%s)"), StrikeCount, BallCount, *Guess);
}

bool ABaseballGameModeBase::ProcessPlayerTurn(int32 PlayerID)
{
    if (bGameOver)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game is over; turn ignored."));
        return false;
    }

    if (PlayerID != CurrentTurn)
    {
        UE_LOG(LogTemp, Warning, TEXT("Player %d tried to play out of turn. CurrentTurn is %d"), PlayerID, CurrentTurn);
        return false;
    }

    // �ش� �÷��̾��� ���� �� ����
    if (PlayerID == 0 && TurnsLeft0 > 0)
    {
        TurnsLeft0--;
    }
    else if (PlayerID == 1 && TurnsLeft1 > 0)
    {
        TurnsLeft1--;
    }

    UE_LOG(LogTemp, Log, TEXT("After Player %d turn: TurnsLeft0=%d, TurnsLeft1=%d"), PlayerID, TurnsLeft0, TurnsLeft1);

    // �� �÷��̾� ��� �� ���� -> ���� ����
    if (TurnsLeft0 == 0 && TurnsLeft1 == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Both players have no turns left. Game Over."));
        bGameOver = true;

        // ��� PlayerController�� GameOver
        for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
        {
            ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(*It);
            if (PC)
            {
                PC->ClientGameOver(false);
            }
        }

        // 10�� �� ���� ����
        FTimerHandle ResetTimerHandle;
        GetWorldTimerManager().SetTimer(ResetTimerHandle, this, &ABaseballGameModeBase::ResetGame, 10.0f, false);
    }
    else
    {
        // �� ��ȯ
        int32 OtherPlayer = (PlayerID == 0) ? 1 : 0;
        if ((OtherPlayer == 0 && TurnsLeft0 > 0) || (OtherPlayer == 1 && TurnsLeft1 > 0))
        {
            CurrentTurn = OtherPlayer;
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("No turn available for other player. CurrentTurn remains %d"), CurrentTurn);
        }
    }

    // �� ��ȯ �� ��� PlayerController�� ������Ʈ
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(*It);
        if (PC)
        {
            PC->ClientUpdateTurn(CurrentTurn);
            PC->ClientUpdateTurnsRemaining(TurnsLeft0, TurnsLeft1);
        }
    }
    return true;
}

void ABaseballGameModeBase::HandleCorrectGuess(int32 WinningPlayerID)
{
    if (bGameOver)
    {
        return;
    }

    bGameOver = true;
    UE_LOG(LogTemp, Log, TEXT("Player %d guessed correctly. Game Over."), WinningPlayerID);

    // ��� PlayerController�� ���� ���� ���� ����
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(*It);
        if (PC && PC->PlayerState)
        {
            ABaseballPlayerState* BPState = Cast<ABaseballPlayerState>(PC->PlayerState);
            bool bLocalWin = (BPState && BPState->GetMyPlayerId() == WinningPlayerID);
            PC->ClientGameOver(bLocalWin);
        }
    }

    // 10�� �Ŀ� ���� ����
    FTimerHandle ResetTimerHandle;
    GetWorldTimerManager().SetTimer(ResetTimerHandle, this, &ABaseballGameModeBase::ResetGame, 10.0f, false);
}

void ABaseballGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    if (NewPlayer && NewPlayer->PlayerState)
    {
        if (ABaseballPlayerState* BPState = Cast<ABaseballPlayerState>(NewPlayer->PlayerState))
        {
            BPState->SetMyPlayerId(NextPlayerId);
            UE_LOG(LogTemp, Log, TEXT("Assigned MyPlayerId = %d to %s"), NextPlayerId, *NewPlayer->GetName());
            NextPlayerId++;
        }
        if (ABaseballPlayerController* PC = Cast<ABaseballPlayerController>(NewPlayer))
        {
            PC->ClientUpdateTurn(CurrentTurn);
            PC->ClientUpdateTurnsRemaining(TurnsLeft0, TurnsLeft1);
        }
    }
}


void ABaseballGameModeBase::ResetGame()
{
    UE_LOG(LogTemp, Log, TEXT("ResetGame: Initiating seamless travel to reset the game."));
    UWorld* World = GetWorld();
    if (World)
    {
        FString MapName = World->GetMapName();
        MapName = FPackageName::GetShortName(MapName);
        FString TravelURL = FString::Printf(TEXT("%s?listen"), *MapName);
        UE_LOG(LogTemp, Log, TEXT("ServerTravel URL: %s"), *TravelURL);
        World->ServerTravel(TravelURL, false);
    }
}
