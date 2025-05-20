#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
using namespace std;

struct Hora {
    int horas;
    int minutos;
};

Hora obtenerHoraLocal(int desfaseMinutos) {
    using namespace chrono;
    auto now = system_clock::now();
    time_t tiempo = system_clock::to_time_t(now);
    tm local_tm = *localtime(&tiempo);

    int totalMinutos = local_tm.tm_hour * 60 + local_tm.tm_min + desfaseMinutos;

    Hora h;
    h.horas = (totalMinutos / 60) % 24;
    h.minutos = totalMinutos % 60;
    return h;
}

int diferenciaEnMinutos(const Hora& cliente, const Hora& servidor) {
    int minutosCliente = cliente.horas * 60 + cliente.minutos;
    int minutosServidor = servidor.horas * 60 + servidor.minutos;
    return minutosCliente - minutosServidor;
}

void cliente(int id, int desfase, Hora servidorHora, vector<int>& diferencias) {
    Hora horaCliente = obtenerHoraLocal(desfase);

    cout << "Cliente " << id << " hora local: "
         << horaCliente.horas << ":" << horaCliente.minutos
         << " Diferencia (min): " << diferenciaEnMinutos(horaCliente, servidorHora) << "\n";

    diferencias[id - 1] = diferenciaEnMinutos(horaCliente, servidorHora);
}

int main() {
    Hora horaServidor = obtenerHoraLocal(0);

    cout << "Servidor hora: " << horaServidor.horas << ":" << horaServidor.minutos << "\n";

    int numClientes = 2;
    vector<int> diferencias(numClientes);

    vector<int> desfases = {20, -10};

    vector<thread> threads;

    for (int i = 0; i < numClientes; i++) {
        threads.emplace_back(cliente, i + 1, desfases[i], horaServidor, ref(diferencias));
    }

    for (auto& t : threads) {
        t.join();
    }

    int suma = 0;
    suma += 0;
    for (auto d : diferencias) {
        suma += d;
    }
    int promedio = suma / (numClientes + 1);

    cout << "\nPromedio de diferencias: " << promedio << " minutos\n";

    cout << "Ajustes de relojes:\n";
    cout << "Servidor debe ajustar en: " << promedio << " minutos\n";

    for (int i = 0; i < numClientes; i++) {
        int ajuste = promedio - diferencias[i];
        cout << "Cliente " << i + 1 << " debe ajustar en: " << ajuste << " minutos\n";
    }

    return 0;
}
