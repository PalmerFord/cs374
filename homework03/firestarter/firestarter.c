/* firestarter.c 
 * David Joiner
 * Usage: Fire [forestSize(20)] [numTrials(5000)] * [numProbabilities(101)] [showGraph(1)]
 */
#include <stdio.h>
#include <stdlib.h>
#include "X-graph.h"
#include <mpi.h>
#include "parallelLoopChunks.h"
#include <limits.h>

#define UNBURNT 0
#define SMOLDERING 1
#define BURNING 2
#define BURNT 3

#define true 1
#define false 0

typedef int boolean;

extern void seed_by_time(int);
extern int ** allocate_forest(int);
extern void initialize_forest(int, int **);
extern double get_percent_burned(int, int **);
extern void delete_forest(int, int **);
extern void light_tree(int, int **,int,int);
extern boolean forest_is_burning(int, int **);
extern void forest_burns(int, int **,double);
extern int burn_until_out(int,int **,double,int,int);
extern void print_forest(int, int **);

int main(int argc, char ** argv) {
    // initial conditions and variable definitions
    int forest_size = 20;
    double * prob_spread;
    double prob_min = 0.0;
    double prob_max = 1.0;
    double prob_step;
    int **forest;
    double * percent_burned;
    int i_trial;
    int n_trials = 5000;
    int i_prob;
    int n_probs = 101;
    int do_display = 0;

    int * iter_count;
    int * total_iter;
    double * total_percent;
    double start_time = 0.0;
    int num_processes = 0;
    int id = 0;
    const int MASTER = 0;

    xgraph thegraph;

    unsigned start = -1, stop = -1;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);
    start_time = MPI_Wtime();

    // check command line arguments
    if (argc > 1) {
        sscanf(argv[1],"%d",&forest_size);
    }
    if (argc > 2) {
        sscanf(argv[2],"%d",&n_trials);
    }
    if (argc > 3) {
        sscanf(argv[3],"%d",&n_probs);
    }
    if (argc > 4) {
        sscanf(argv[4],"%d",&do_display);
    }
    if (do_display!=0) do_display=1;

    // setup problem
    seed_by_time(0);
    forest=allocate_forest(forest_size);
    prob_spread = (double *) malloc (n_probs*sizeof(double));
    percent_burned = (double *) malloc (n_probs*sizeof(double));
    iter_count = (int *) malloc (n_probs*sizeof(int));
    total_percent = (double *) malloc (n_probs*sizeof(double));
    total_iter = (int *) malloc (n_probs*sizeof(int));
    getChunkStartStopValues(id, num_processes, n_trials, &start, &stop);

    // for a number of probabilities, calculate
    // average burn and output
    prob_step = (prob_max-prob_min)/(double)(n_probs-1);
    if (id == MASTER) {
        printf("Probability of fire spreading | Average percent burned | Number of Iterations\n");
    }

    //initialize values of the array
    for (i_prob = 0; i_prob < n_probs; i_prob++) {
        prob_spread[i_prob] = prob_min + (double)i_prob * prob_step;
        percent_burned[i_prob] = 0.0;
        iter_count[i_prob] = 0;
    }

    // perform simulation trials
    for (i_trial = start; i_trial < stop; ++i_trial) {
        for (i_prob = 0; i_prob < n_probs; i_prob++) {
            iter_count[i_prob] = burn_until_out(forest_size,forest,prob_spread[i_prob],
                forest_size/2,forest_size/2);
            percent_burned[i_prob] += get_percent_burned(forest_size,forest);
            
        }
    }

    MPI_Reduce(percent_burned, total_percent, n_probs, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(iter_count, total_iter, n_probs, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // display average burn statistics and total simulation time
    if (id == MASTER) {
        for (i_prob = 0; i_prob < n_probs; i_prob++) {
            total_percent[i_prob] /= n_trials;
            printf("%lf \t %lf \t%d\n", prob_spread[i_prob], total_percent[i_prob], iter_count[i_prob]);
        }

        printf("Total Time: %f\n", MPI_Wtime() - start_time);

        // plot graph
        if (do_display == 1) {
            xgraphSetup(&thegraph, 300, 300);
            xgraphDraw(&thegraph, n_probs, 0, 0, 1, 1, prob_spread, total_percent);
            pause();
        }
    }

    MPI_Finalize();

    // clean up
    delete_forest(forest_size,forest);
    free(prob_spread);
    free(percent_burned);
    free(iter_count);
    free(total_percent);
    free(total_iter);
    return 0;
}

#include <time.h>

void seed_by_time(int offset) {
    time_t the_time;
    time(&the_time);
    srand((int)the_time+offset);
}

int burn_until_out(int forest_size,int ** forest, double prob_spread,
    int start_i, int start_j) {
    int count;

    initialize_forest(forest_size,forest);
    light_tree(forest_size,forest,start_i,start_j);

    // burn until fire is gone
    count = 0;
    while(forest_is_burning(forest_size,forest)) {
        forest_burns(forest_size,forest,prob_spread);
        count++;
    }

    return count;
}

double get_percent_burned(int forest_size,int ** forest) {
    int i,j;
    int total = forest_size*forest_size-1;
    int sum=0;

    // calculate pecrent burned
    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNT) {
                sum++;
            }
        }
    }

    // return percent burned;
    return ((double)(sum-1)/(double)total);
}


int ** allocate_forest(int forest_size) {
    int i;
    int ** forest;

    forest = (int **) malloc (sizeof(int*)*forest_size);
    for (i=0;i<forest_size;i++) {
        forest[i] = (int *) malloc (sizeof(int)*forest_size);
    }

    return forest;
}

void initialize_forest(int forest_size, int ** forest) {
    int i,j;

    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            forest[i][j]=UNBURNT;
        }
    }
}

void delete_forest(int forest_size, int ** forest) {
    int i;

    for (i=0;i<forest_size;i++) {
        free(forest[i]);
    }
    free(forest);
}

void light_tree(int forest_size, int ** forest, int i, int j) {
    forest[i][j]=SMOLDERING;
}

boolean fire_spreads(double prob_spread) {
    if ((double)rand()/(double)RAND_MAX < prob_spread) 
        return true;
    else
        return false;
}

void forest_burns(int forest_size, int **forest,double prob_spread) {
    int i,j;
    extern boolean fire_spreads(double);

    //burning trees burn down, smoldering trees ignite
    for (i=0; i<forest_size; i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNING) forest[i][j]=BURNT;
            if (forest[i][j]==SMOLDERING) forest[i][j]=BURNING;
        }
    }

    //unburnt trees catch fire
    for (i=0; i<forest_size; i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNING) {
                if (i!=0) { // North
                    if (fire_spreads(prob_spread)&&forest[i-1][j]==UNBURNT) {
                        forest[i-1][j]=SMOLDERING;
                    }
                }
                if (i!=forest_size-1) { //South
                    if (fire_spreads(prob_spread)&&forest[i+1][j]==UNBURNT) {
                        forest[i+1][j]=SMOLDERING;
                    }
                }
                if (j!=0) { // West
                    if (fire_spreads(prob_spread)&&forest[i][j-1]==UNBURNT) {
                        forest[i][j-1]=SMOLDERING;
                    }
                }
                if (j!=forest_size-1) { // East
                    if (fire_spreads(prob_spread)&&forest[i][j+1]==UNBURNT) {
                        forest[i][j+1]=SMOLDERING;
                    }
                }
            }
        }
    }
}

boolean forest_is_burning(int forest_size, int ** forest) {
    int i,j;

    for (i=0; i<forest_size; i++) {
        for (j=0; j<forest_size; j++) {
            if (forest[i][j]==SMOLDERING||forest[i][j]==BURNING) {
                return true;
            }
        }
    }
    return false;
}

void print_forest(int forest_size,int ** forest) {
    int i,j;

    for (i=0;i<forest_size;i++) {
        for (j=0;j<forest_size;j++) {
            if (forest[i][j]==BURNT) {
                printf(".");
            } else {
                printf("X");
            }
        }
        printf("\n");
    }
}
