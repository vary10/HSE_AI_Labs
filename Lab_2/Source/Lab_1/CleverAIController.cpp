// Fill out your copyright notice in the Description page of Project Settings.

#include "Lab_1.h"
#include "CleverAIController.h"
#include "SimpleAIController.h"
#include <vector>

ACleverAIController::ACleverAIController()
{
    bDeliveringOrder = false;
    CurrentOrderNumber = -1;
    tick = 0;
}

float ACleverAIController::ComputeHouseScore(float Distance, float TimeLeft) const
{
    return fmax(Distance - 150.f, 1.f) * (TimeLeft * TimeLeft);
}

// GetDistanceBetween
void ACleverAIController::Tick(float DeltaSeconds)
{

    if (bDeliveringOrder) {
        float Distance = GetDistanceToDestination(CurrentDestination);
        if (Distance < 300.f) {
            UE_LOG(LogTemp, Warning, TEXT("Trying to deliver order %d, current distance: %1.3f"), CurrentOrderNumber, Distance);
            bool bDeliveredOrder = TryDeliverPizza(CurrentOrderNumber);
            if (bDeliveredOrder) {
                UE_LOG(LogTemp, Warning, TEXT("Delivered order %d"), CurrentOrderNumber);
                bDeliveringOrder = false;
                CurrentOrderNumber = -1;
            }
        }
    }

    if (tick < 20) {
        ++tick;
        return;   
    }
    tick = 0;
    auto Orders = GetPizzaOrders();
    if (Orders.Num() == 0) {
        // No orders to serve.
        // TODO: Go to center of the map.
        return;
    }

    int PizzaAmount = GetPizzaAmount();
    if (PizzaAmount == 0) {
        bool bGrabbedPizza = TryGrabPizza();
        // Failed to retrieve pizza, need to get closer to the bakery.
        if (!bGrabbedPizza) {
            return;
        }
    }

    // Take first order.
    auto HouseLocations = GetHouseLocations();
    std::vector<std::vector<float> > a(GetControllerCount() + 1, std::vector<float>(Orders.Num() + 1));

    for (int id = 0; id < GetControllerCount(); ++id) {
        if (id != ControllerId) {
            ASimpleAIController* Controller = Cast<ASimpleAIController>(GetControllerById(id));
            if (Controller) {
                for (int i = 0; i < Orders.Num(); ++i) {
                    float Distance = Controller->GetDistanceToDestination(HouseLocations[Orders[i].HouseNumber]);
                    float TimeLeft = GetHouseTimeLeft(Orders[i].HouseNumber);
                    a[id+1][i+1] = ComputeHouseScore(Distance, TimeLeft);
                }
            }
        } else {
            for (int i = 0; i < Orders.Num(); ++i) {
                float Distance = GetDistanceToDestination(HouseLocations[Orders[i].HouseNumber]);
                float TimeLeft = GetHouseTimeLeft(Orders[i].HouseNumber);
                a[id+1][i+1] = ComputeHouseScore(Distance, TimeLeft);
            }
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("Created matrix"));
    int n = GetControllerCount();
    int m = Orders.Num();
    std::vector<int> u (n+1), v (m+1), p (m+1), way (m+1);
    for (int i=1; i<=n; ++i) {
        p[0] = i;
        int j0 = 0;
        std::vector<int> minv (m+1, 999999);
        std::vector<char> used (m+1, false);
        do {
            used[j0] = true;
            int i0 = p[j0],  delta = 999999,  j1;
            for (int j=1; j<=m; ++j)
                if (!used[j]) {
                    int cur = a[i0][j]-u[i0]-v[j];
                    if (cur < minv[j])
                        minv[j] = cur,  way[j] = j0;
                    if (minv[j] < delta)
                        delta = minv[j],  j1 = j;
                }
            for (int j=0; j<=m; ++j)
                if (used[j])
                    u[p[j]] += delta,  v[j] -= delta;
                else
                    minv[j] -= delta;
            j0 = j1;
        } while (p[j0] != 0);
        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }

    std::vector<int> list(n);
    // for (int j = 1; j < m + 1; ++j) {
    //     if (p[j] != 0) {
    //         list[p[j]-1] = j - 1;
    //     }
    // }

    // for (int i = 0; i < 4; ++i) {
    //     UE_LOG(LogTemp, Warning, TEXT("%d"), list[i]);
    // }
    auto Order = Orders[0];
    auto HouseLocation = HouseLocations[Order.HouseNumber];
    for (int id = 0; id < GetControllerCount(); ++id) {
        if (id != ControllerId) {
            ASimpleAIController* Controller = Cast<ASimpleAIController>(GetControllerById(id));
            if (Controller) {
                // Order = Orders[list[id]];
                Controller->SetCurrentOrderNumber(Order);
            }
        } else {
            // Order = Orders[list[id]];
            HouseLocation = HouseLocations[Order.HouseNumber];
            CurrentOrderNumber = Order.OrderNumber;
            CurrentDestination = HouseLocation;
            SetNewMoveDestination(HouseLocation);
            bDeliveringOrder = true;
        }
    }





    // if (!bDeliveringOrder || Order.OrderNumber != CurrentOrderNumber) {
    //     auto HouseLocation = HouseLocations[Order.HouseNumber];
    //     bDeliveringOrder = true;
    //     CurrentOrderNumber = Order.OrderNumber;
    //     CurrentDestination = HouseLocation;
    //     SetNewMoveDestination(HouseLocation);
    //     UE_LOG(LogTemp, Warning, TEXT("Took new order %d to house %d with score %1.3f"),
    //         Order.OrderNumber,
    //         Order.HouseNumber,
    //         BestScore);
    // }
}

int ACleverAIController::GetCurrentOrderNumber()
{
    return CurrentOrderNumber;
}
