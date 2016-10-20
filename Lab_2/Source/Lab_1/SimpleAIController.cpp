// Fill out your copyright notice in the Description page of Project Settings.

#include "Lab_1.h"
#include "SimpleAIController.h"

ASimpleAIController::ASimpleAIController()
{
    bDeliveringOrder = false;
    CurrentOrderNumber = -1;
}

int ASimpleAIController::GetCurrentOrderNumber()
{
    return CurrentOrderNumber;
}

void ASimpleAIController::SetCurrentOrderNumber(FPizzaOrder order)
{
    auto HouseLocation = GetHouseLocations()[order.HouseNumber];
    CurrentOrderNumber = order.OrderNumber;
    CurrentDestination = HouseLocation;
    SetNewMoveDestination(HouseLocation);
    bDeliveringOrder = true;
}


void ASimpleAIController::Tick(float DeltaSeconds)
{
    if (bDeliveringOrder) {
        float Distance = GetDistanceToDestination(CurrentDestination);
        if (Distance > 300.f) {
            UE_LOG(LogTemp, Warning, TEXT("Moving to order %d, current distance: %1.3f"), CurrentOrderNumber, Distance);
            SetNewMoveDestination(CurrentDestination);
            return;
        }
        UE_LOG(LogTemp, Warning, TEXT("Trying to deliver order %d, current distance: %1.3f"), CurrentOrderNumber, Distance);
        bool bDeliveredOrder = TryDeliverPizza(CurrentOrderNumber);
        if (bDeliveredOrder) {
            UE_LOG(LogTemp, Warning, TEXT("Delivered order %d"), CurrentOrderNumber);
            bDeliveringOrder = false;
            CurrentOrderNumber = -1;
        } else {
            SetNewMoveDestination(CurrentDestination);
        }
        return;
    }
}
