// Fill out your copyright notice in the Description page of Project Settings.

#include "Lab_1.h"
#include "SimpleAIController.h"

ASimpleAIController::ASimpleAIController()
{
    bDeliveringOrder = false;
    CurrentOrderNumber = -1;
}

void ASimpleAIController::Tick(float DeltaSeconds)
{
    auto Orders = GetPizzaOrders();
    auto HouseLocations = GetHouseLocations();
    if (bDeliveringOrder) {
        float Distance = GetDistanceToDestination(CurrentDestination);
        if (Distance <= 300.f) {
            UE_LOG(LogTemp, Warning, TEXT("Trying to deliver order %d, current distance: %1.3f"), CurrentOrderNumber, Distance);
            bool bDeliveredOrder = TryDeliverPizza(CurrentOrderNumber);
            if (bDeliveredOrder) {
                UE_LOG(LogTemp, Warning, TEXT("Delivered order %d"), CurrentOrderNumber);
                bDeliveringOrder = false;
                CurrentOrderNumber = -1;
            } else {
                UE_LOG(LogTemp, Warning, TEXT("Continue to %d, current distance: %1.3f"), CurrentOrderNumber, Distance);
                SetNewMoveDestination(CurrentDestination);
            }
            return;
        }
        for (int i = 0; i < Orders.Num(); ++i) {
            float currentDistance = GetDistanceToDestination(HouseLocations[Orders[i].HouseNumber]);
            if (currentDistance < 390.f && CurrentDestination != HouseLocations[Orders[i].HouseNumber]) {
                auto HouseLocation = HouseLocations[Orders[i].HouseNumber];
                bDeliveringOrder = true;
                CurrentOrderNumber = Orders[i].OrderNumber;
                CurrentDestination = HouseLocation;
                SetNewMoveDestination(HouseLocation);
                UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!Took new order %d to house %d"), Orders[i].OrderNumber, Orders[i].HouseNumber);
                return;
            }
        }
        UE_LOG(LogTemp, Warning, TEXT("Continue to %d, current distance: %1.3f"), CurrentOrderNumber, Distance);
        SetNewMoveDestination(CurrentDestination);
        return;
    }
    
    
    if (Orders.Num() == 0) {
        // No orders to serve.
        return;
    }
    
    
    
    int closestOrder = 0;
    float priority = 9999999;
    for (int i = 0; i < Orders.Num(); ++i) {
        float currentDistance = GetDistanceToDestination(HouseLocations[Orders[i].HouseNumber]);
        float currentTime = GetHouseTimeLeft(Orders[i].HouseNumber);
        UE_LOG(LogTemp, Warning, TEXT("priority %f, current distance: %1.3f, time %1.3f, house %d"), (currentDistance - 250) * currentTime * currentTime, currentDistance, currentTime,Orders[i].HouseNumber);
        if (priority > (currentDistance - 310) * currentTime * currentTime) {
            priority = (currentDistance - 310) * currentTime * currentTime;
            closestOrder = Orders[i].OrderNumber;
            UE_LOG(LogTemp, Warning, TEXT("Closest order %d"), closestOrder);
        }
    }
    
    auto Order = Orders[0];
    for (size_t i = 0; i < Orders.Num(); ++i) {
        if (Orders[i].OrderNumber == closestOrder) {
            Order = Orders[i];
        }
    }
    
    int PizzaAmount = GetPizzaAmount();
    if (PizzaAmount == 0) {
        bool bGrabbedPizza = TryGrabPizza();
        // Failed to retriev pizza, need to get closer to the bakery.
        if (!bGrabbedPizza) {
            return;
        }
    }
    
    auto HouseLocation = HouseLocations[Order.HouseNumber];
    bDeliveringOrder = true;
    CurrentOrderNumber = Order.OrderNumber;
    CurrentDestination = HouseLocation;
    SetNewMoveDestination(HouseLocation);
    UE_LOG(LogTemp, Warning, TEXT("Took new order %d to house %d"), Order.OrderNumber, Order.HouseNumber);
    return;
}

