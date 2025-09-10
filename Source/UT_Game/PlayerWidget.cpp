// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Components/TextBlock.h"


void UPlayerWidget::NativeConstruct()
{
    Super::NativeConstruct();

   /* if (!HealthText_1)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("HealthText_1 is NOT bound!"));
    }
    else
    {
        HealthText_1->SetText(FText::FromString("Begin"));
        if (HealthText_1)
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("HealthText_2 ::: %s"), *HealthText_1->GetName()));
        }
    }*/
}

void UPlayerWidget::SetHealthText(float Health)
{
   /* GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("!!!!!!!!!!!")));
    if (!HealthText_1) {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("HealthText_1 is EMPTY: %f"), Health));

    }
    if (HealthText_1)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Remaining: %f"), Health));

        FString Text = FString::Printf(TEXT("Health: %.0f"), Health);
        HealthText_1->SetText(FText::FromString(Text));
       
    }*/
}