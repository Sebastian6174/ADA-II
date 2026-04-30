#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <cstring>

using namespace std;

struct Tablon{
    int s;
    int r;
    int p;
    int o;
    Tablon(int s, int r, int p, int o) : s(s), r(r), p(p), o(o) {}
};

void solucion_ingenua(vector<int>& camino_actual, vector<bool>& regado, int actual, int cant, int costo, int n, int tiempo_actual);



int valor_optimo_top_down(int S, int tiempo_actual);
void solucion_bottom_up();
vector<int> reconstruir_solucion();

long long C(int i, int t);
void imprimir_solucion(vector<int>& orden);

int n;
vector<Tablon> F;
int memo[1 << 20];
int eleccion[1 << 20];

int main(){
    F = {Tablon(0, 0, 0, 0)};
    
    memset(memo, -1, sizeof(memo));

    long long costo_total = valor_optimo_top_down((1 << n) - 1, 0);
    vector<int> orden = reconstruir_solucion();

    cout << "El costo total es: " << costo_total << endl;
    imprimir_solucion(orden);
    return 0;
}

// ------------------------------ ALGORITMOS INGENUOS ------------------------------

int costo_min = INT_MAX;
vector<int> mejor_camino;

void solucion_ingenua(vector<int>& camino_actual, vector<bool>& regado, int actual, int cant, int costo, int n, int tiempo_actual){

    if (cant == n){
        if(costo < costo_min){
            costo_min = costo;
            mejor_camino = camino_actual;
        }
        return;
    }

    for (int i = 0; i < n; i++){
        if (!regado[i]){
            regado[i] = true;
            camino_actual.push_back(i);
            solucion_ingenua(camino_actual, regado, i, cant + 1, costo + C(i, tiempo_actual), n, tiempo_actual + F[i].r);
            camino_actual.pop_back();
            regado[i] = false;
        }
    }
}

// ------------------------------ ALGORITMOS DINÁMICOS ------------------------------

vector<int> reconstruir_solucion(){
    vector<int> orden;
    int S = (1 << n) - 1;

    while (S > 0){
        int i = eleccion[S];
        orden.push_back(i);
        S ^= (1 << i);
    }

    return orden;
}

int valor_optimo_top_down(int S, int tiempo_actual){

    if(S == 0) {
        return memo[S] = 0;
    }

    if(memo[S] != -1) return memo[S];

    long long costo_min = 2e18;

    for (int i = 0; i < n; i++){
        if (S & (1 << i)){
            
            long long costo = C(i, tiempo_actual)
                            + valor_optimo_top_down(S ^ (1 << i), tiempo_actual + F[i].r);

            if (costo < costo_min){
                costo_min = costo;
                eleccion[S] = i;
            }
        }
    }       

    return memo[S] = costo_min;
}

void solucion_bottom_up(){
    memo[0] = 0;

    for (int S = 1; S < (1 << n) ; S++){
        if (memo[S] == 2e18) continue;
            
        int tiempo_actual = 0;
        for (int k = 1; k < n; k++){
            if (S & (1 << k)){
                tiempo_actual += F[k].r;
            }
        }

        long long costo_min = 2e18;   

        for (int j = 0; j < n; j++){
            if (S & (1 << j)){

                long long costo = C(j, tiempo_actual)
                                + memo[S ^ (1 << j)];

                if (costo < costo_min){
                    costo_min = costo;
                    eleccion[S] = j;
                }
            }
        }
        memo[S] = costo_min;
    }
}


// ------------------------------ ALGORITMO VORÁZ ------------------------------



// ------------------------------ FUNCIONES AUX ------------------------------

long long C(int i, int t){
    Tablon T = F[i];
    if (t == T.o){
        return T.s - (t + T.r);
    } else if(T.s - T.r >= t){
        return 2LL * (T.s - (t + T.r));
    } else{
        return 2LL * T.p * ((t + T.r) - T.s);
    }
}
    
void imprimir_solucion(vector<int>& orden){
    cout << "\n";
    for (int i : orden){
        cout << i << ", ";
    }
}