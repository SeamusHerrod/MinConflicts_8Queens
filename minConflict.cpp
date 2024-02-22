#include <iostream>
#include <random>
#include <vector>
#include <ctime>
#include <cmath>
#include <algorithm>

using namespace std;

static size_t EVALS = 0;     // running total number of evals
//static int GENS = 1000;    // number of generations 
//static int k = 1000;
static int total_runs = 100;
static double pmut = 0.1;
int successfulRuns = 0;

int myRand(int low, int high) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(low, high);
    double r = dis(gen);
    return r;
}

void mutate(int config[], double p) {
    random_device rd;
    mt19937 eng(rd());
    uniform_int_distribution<> distr(1,8);
    uniform_real_distribution<> dis(0, 1);
    for (int i = 1; i <= 8; i++) {
        double r = dis(eng);
        if (r < p) {
            config[i - 1] = distr(eng);
        }
    }
}

// returns 1 if queens q1 and q2 are attacking each other, 0 o.w.
int attacking(int q1col, int q1row, int q2col, int q2row) {
  if (q1row==q2row) return(1); // same row
  int coldiff=q1col-q2col;
  int rowdiff=q1row-q2row;
  if (abs(coldiff)==abs(rowdiff)) return(1); // same diagonal
  return(0);
}

int fitness(const int config[]) {
    EVALS++;
    int fit = 28;
    for (int i = 0; i < 8; i++) {
        for (int j = i+1; j < 8; j++) {
            fit -= attacking(i, config[i], j, config[j]);
        }
    }
    return fit;
}

void random_config(int state[]) {
    random_device rd;
    mt19937 eng(rd());
    uniform_int_distribution<> distr(1,8);
    for (int i = 0; i < 8; i++) {
        state[i] = distr(eng);
    }
}

void print_config(int config[]) {
        cout << "[";
        for (int i = 0; i < 8; i++){
            if (i == 7) {
                cout << config[i] << "]" << endl;
            }
            else{
                cout << config[i] << ", ";
            }
    }
}

size_t average_runs(vector<size_t> evals, int succ){
    int cum_evals = 0;
    for (int i = 0; i < total_runs; i++){
        cum_evals += evals[i];
    }
    return ceil(cum_evals / succ);
}

size_t conflicts(int i, int config[]) {
    int num_conflicts = 0; 
    for (int j = 0; j < 8; j++){
        if (j != i) {
            if (attacking(i, config[i], j, config[j])){
                num_conflicts++;
            }
        }
    }
    return num_conflicts;
}

bool random_min_conflicts(int config[], int max_steps) {
    
    for (int i = 0; i < max_steps; i++){

        if (fitness(config) == 28){
            successfulRuns++;
            return true;
        }
        int var; // pick random conflicting var
        do {
            var = myRand(0,7);
        } while (conflicts(var, config) == 0);
        // iterate through row positions in var's column
        // move to the column which has smallest number of conflicts

        int min_position = config[var]; // init min seen conflicts to current row of var column 
                                        // if this is already in the min conflicts spot, then it can be 
                                        // used to restore the original value 
        int min_conflicts = conflicts(var, config);
        bool changed_min_conflicts = false;
        for (int i = 1; i <= 8; i++){
            config[var] = i; 
            int new_num_conflicts = conflicts(var, config);
            if (new_num_conflicts < min_conflicts){
                min_position = i;
                min_conflicts = new_num_conflicts;
                changed_min_conflicts = true;
            }
        }
        if (changed_min_conflicts == false) {
            mutate(config, pmut);
        }
        config[var] = min_position;

    }
    return false;
}

bool cyclic_min_conflicts(int config[], int max_steps) {
        
    for (int i = 0; i < max_steps; i++){

        if (fitness(config) == 28){
            successfulRuns++;
            return true;
        }
        for (int j = 0; j < 8; j++) {
            int var = j;
            // iterate through row positions in var's column
            // move to the column which has smallest number of conflicts

            int min_position = config[var]; // init min seen conflicts to current row of var column 
                                            // if this is already in the min conflicts spot, then it can be 
                                            // used to restore the original value 
            int min_conflicts = conflicts(var, config);
            bool changed_min_conflicts = false;
            for (int i = 1; i <= 8; i++){
                config[var] = i; 
                int new_num_conflicts = conflicts(var, config);
                if (new_num_conflicts < min_conflicts){
                    min_position = i;
                    min_conflicts = new_num_conflicts;
                    changed_min_conflicts = true;
                }
            }
            if (changed_min_conflicts == false) {
                mutate(config, pmut);
            }
            config[var] = min_position;
        }

    }
    return false;
}

int main(int argc, char* argv[]) {
    srand(time(0));
    int best_fitness = 0;
    vector<size_t> evals_per_run(0);
    if (argc < 2) {
        cout << "enter command line argument:\n1 for random\n2 for cyclic" << endl;
        return 0;
    }
    if (atoi(argv[1]) == 1) {
        /* generate random initial configuarion */
        for (int i = 0; i < total_runs; i++){
            int init_config[8];
            random_config(init_config);

            if (random_min_conflicts(init_config, 250)) {
                print_config(init_config);
                cout << "NUM EVALS: " << EVALS << endl;
                evals_per_run.push_back(EVALS);
            }

            EVALS = 0;
        }
        cout << "successful runs: " << successfulRuns << endl;
        cout << "average num evals: " << average_runs(evals_per_run, successfulRuns) << endl;
    }
    else if (atoi(argv[1]) == 2) {
        /* generate random initial configuarion */
        for (int i = 0; i < total_runs; i++){
            int init_config[8];
            random_config(init_config);

            if (cyclic_min_conflicts(init_config, 250)) {
                print_config(init_config);
                cout << "NUM EVALS: " << EVALS << endl;
                evals_per_run.push_back(EVALS);
            }

            EVALS = 0;
        }
        cout << "successful runs: " << successfulRuns << endl;
        cout << "average num evals: " << average_runs(evals_per_run, successfulRuns) << endl;
    }
    return 0;
}