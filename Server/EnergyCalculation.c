#include <stdio.h>

double energy_calcution()
{
    int length = 9;
    int spot = 4;
    double spot_prices[] = {3.1, 4.5 ,1.9, 3.0, 5.4, 6.8 ,7.6 ,2.2, 3.0};
    double lowest = spot_prices[0];
    double highest = spot_prices[0];

    for(int i = 1; i < length; i++) {
        if(spot_prices[i] < lowest) lowest = spot_prices[i];
        if(spot_prices[i] > highest) highest = spot_prices[i];
    }

    double decision = 0;

    if((spot_prices[spot] - lowest) == 0) {
        decision = 0;
    } else {
        decision = (spot_prices[spot] - lowest) / (highest - lowest);
    }
    printf("Decision (0 == use, 1 == sell): %f\n\n", decision);

    return 0.5;
}

int main() {
    energy_calcution();
}
