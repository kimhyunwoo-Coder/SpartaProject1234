#include "BaseballGameStateBase.h"
#include "Net/UnrealNetwork.h"

ABaseballGameStateBase::ABaseballGameStateBase()
{
    LastResult = TEXT("");
    bReplicates = true;
}

void ABaseballGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // LastResult를 모든 클라이언트에 복제
    DOREPLIFETIME(ABaseballGameStateBase, LastResult);
}