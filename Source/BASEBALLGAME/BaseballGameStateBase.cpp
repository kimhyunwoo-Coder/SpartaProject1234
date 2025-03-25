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

    // LastResult�� ��� Ŭ���̾�Ʈ�� ����
    DOREPLIFETIME(ABaseballGameStateBase, LastResult);
}