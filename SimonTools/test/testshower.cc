#include "SRothman/SimonTools/src/ToyShowerer.h"

int main(){
    ToyShowerer showerer("UNIFORM", 
                         "GLUON",
                         "LNX",
                         0.005,
                         0.01,
                         0.5);
    jet result;

    showerer.test_cos2phi(10000, 10);

    showerer.test_lnx(10000, 10, 0.001, 0.5);

    showerer.test_gluon_z(10000, 10);

    showerer.shower(100, 0, 0, 0, 15, result);

    for (const auto& particle : result.particles){
        printf("pt: %f, eta: %f, phi: %f, mass: %f\n", particle.pt, particle.eta, particle.phi, 0.0);
    }

    return 0;
}
