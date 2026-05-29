#include "costo/costo.h"

using namespace std;

long long calcular_costo_individual(const Tablon& T, long long t) {
    if (t == T.rp) {
        return T.ts - (t + T.tr);
    }
    if (T.ts - T.tr >= t) {
        return 2LL * (T.ts - (t + T.tr));
    }
    return 2LL * T.p * ((t + T.tr) - T.ts);
}

long long calcular_costo_total(const Finca& f, const std::vector<int>& orden) {
    long long costo_total = 0;
    long long tiempo_actual = 0;

    for (int i : orden) {
        costo_total += calcular_costo_individual(f[i], tiempo_actual);
        tiempo_actual += f[i].tr;
    }

    return costo_total;
}
