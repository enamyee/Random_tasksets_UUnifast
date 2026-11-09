
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <cstdlib>
using namespace std;

struct Task {
    int T;
    int C;
    int D;
};

vector<double> uunifast(int n, double U, mt19937& gen) {
    vector<double> utilizations;
    uniform_real_distribution<double> dist(0.0, 1.0);

    double sumU = U;

    for (int i = n - 1; i > 0; i--) {
        double r = dist(gen);
        double nextSumU = sumU * pow(r, 1.0 / i);

        utilizations.push_back(sumU - nextSumU);
        sumU = nextSumU;
    }

    utilizations.push_back(sumU);
    return utilizations;
}

double calculateUtilization(const vector<Task>& tasks) {
    double total = 0.0;

    for (const Task& task : tasks) {
        total += static_cast<double>(task.C) / task.T;
    }
    return total;
}

vector<Task> generateTaskSet(int n, double U, int v, mt19937& gen) {
    vector<double> utilizations = uunifast(n, U, gen);
    vector<Task> tasks;

    uniform_int_distribution<int> periodDist(100, 1000);

    for (double ui : utilizations) {
        Task task;

        task.T = periodDist(gen);
        task.C = max(1, static_cast<int>(round(ui * task.T)));

        if (v == 0) {
            task.D = task.T;
        } else {
            int minD = static_cast<int>(ceil(0.7 * task.T));

            uniform_int_distribution<int> deadlineDist(minD, task.T);
            task.D = deadlineDist(gen);
        }

        tasks.push_back(task);
    }

    return tasks;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "3 arguments required";
        return 1;
    }

    int n = atoi(argv[1]);
    double U = atof(argv[2]);
    int v = atoi(argv[3]);

    if (n <= 0 || !(U > 0 && U < 1) || (v != 0 && v != 1)) {
        cerr << "Invalid arguments\n";
        return 1;
    }
    if (n / 1000.0 > U) {
        cerr << "Cannot generate a task set\n";
        return 1;
    }

    random_device rd;
    mt19937 gen(rd());

    cout << fixed << setprecision(6);

    for (int setNum = 1; setNum <= 100; setNum++) {
        vector<Task> tasks;
        double actualU;

        do {
            tasks = generateTaskSet(n, U, v, gen);
            actualU = calculateUtilization(tasks);
        } while (actualU > U);

        cout << "\nTask Set #" << setNum << "\n";
        cout << "Task\tT\tC\tD\n";

        for (int i = 0; i < n; i++) {
            cout << i + 1 << "\t"
                 << tasks[i].T << "\t"
                 << tasks[i].C << "\t"
                 << tasks[i].D << "\n";
        }

        cout << "Target U: " << U << "\n";
        cout << "Actual U: " << actualU << "\n";
        cout << "Error: " << abs(U - actualU) << "\n";
    }

    return 0;
}
