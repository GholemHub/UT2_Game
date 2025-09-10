// Fill out your copyright notice in the Description page of Project Settings.


#include "UT_HUD.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.h"


void AUT_HUD::BeginPlay()
{
	Super::BeginPlay();

	//if (PlayerWidgetClass)
	//{
	//	PlayerWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerWidgetClass);
	//	if (PlayerWidget)
	//	{
	//		PlayerWidget->AddToViewport();
	//		auto W = Cast<UPlayerWidget>(PlayerWidget);
	//		if (W) {
	//			W->SetHealthText(100.f);
	//		}
	//		
	//		//PlayerWidget->SetHealthText(100.f);  // Safe to call here

	//	}
	//}
}

void AUT_HUD::AddText()
{
	//PlayerWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerWidgetClass);
	//if (PlayerWidget)
	//{
	//	PlayerWidget->AddToViewport();
	//	auto W = Cast<UPlayerWidget>(PlayerWidget);
	//	if (W) {
	//		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("???????????")));
	//		W->SetHealthText(100.f);
	//	}

	//	//PlayerWidget->SetHealthText(100.f);  // Safe to call here

	//}
}
