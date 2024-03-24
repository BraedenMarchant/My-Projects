// Include everything necessary here
#include <ctype.h>
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "functions.h"


bool validInput(int argc, char *argv[]){

    // Check the number of inputs
    if ( argc != 6) {
        printf (" Incorrect number of inputs . Expected 5 inputs .\n");
        return false; // Exit the program with an error status
    }

    //iterate thru argument values and validate if number
    //Only validating first 4 input - validating exponential formt is work in progress
    for(int i = 1; i <= 4; i++){
        int length = (int)strlen(argv[i]);
       
        for(int j = 0;j < length; j++){
            if(isdigit(argv[i][j]) == 0 && argv[i][j] != '.') {
                printf("All input must be numeric and separated by a space\n");
                return false;
            }
        }
    }

    return true;
    
}

int main(int argc, char *argv[])
{
    srand(time(NULL));

    // <YOUR CODE: Handle the possible errors in input data given by the user and say how to execute the code>
    // Check the number of inputs
    if ( !validInput(argc, argv)) {
        return 1; // Exit the program with an error status
    }

    // <YOUR CODE: Assign all inputs given by the user argv[i]> like
    // POPULATION_SIZE, MAX_GENERATIONS, crossover_rate, mutate_rate, stop_criteria
    int POPULATION_SIZE  = atoi ( argv [1]);
    int MAX_GENERATIONS  = atoi ( argv [2]);
    float crossover_rate = atof (argv[3]);
    float mutate_rate = atof (argv[4]);
    long double stop_criteria = atof (argv[5]);


    // ###################################################################################
    // you dont need to change anything here
    // the number of variables
    int NUM_VARIABLES = 2;
    // the lower bounds of variables
    double Lbound[] = {-5.0, -5.0};
    // the upper bounds of variable
    double Ubound[] = {5.0, 5.0};
    // ###################################################################################


    // <YOUR CODE: Here make all the initial print outs>
    printf("Genetic Algorithm is initiated.\n");
    printf("The number of variables:  %d\n", NUM_VARIABLES);
    printf("Lower bounds:  [%f, %f]\n", Lbound[0], Lbound[1]);
    printf("Upper bounds:  [%f, %f]\n\n", Ubound[0], Ubound[1]);
    printf("Population Size:  %d\n", POPULATION_SIZE);
    printf("Max Generation:  %d\n", MAX_GENERATIONS);
    printf("Crossover Rate:  %f\n", crossover_rate);
    printf("Mutation Rate:  %f\n", mutate_rate);
    printf("Stopping criteria:  %.16Lf\n", stop_criteria);

    printf("\n\nResults\n---------------------------------------\n");


    clock_t start_time, end_time;
    double cpu_time_used;
    start_time = clock();
    

    // <YOUR CODE: Declare all the arrays you need here>
    double population[POPULATION_SIZE][NUM_VARIABLES];
    double fitness[POPULATION_SIZE];
   

    // <YOUR CODE: Call generate_population function to initialize the "population"> like:
    generate_population(POPULATION_SIZE, NUM_VARIABLES, population, Lbound, Ubound);

    

    // iteration starts here. The loop continues until MAX_GENERATIONS is reached
    // Or stopping criteria is met
    double minimumFitness = 0;
    double minimumFitnessGenes[NUM_VARIABLES];;
    for (int generation = 0; generation < MAX_GENERATIONS; generation++)
    {
        // printf("GENERATION: %d\n", generation);
        double new_population[POPULATION_SIZE][NUM_VARIABLES];

        // <YOUR CODE: Compute the fitness values using objective function for
        // each row in "population" (each set of variables)> like:
        compute_objective_function(POPULATION_SIZE, NUM_VARIABLES, population, fitness);
        

        // <YOUR CODE: Here implement the logic of finding best solution with minimum fitness value
        // and the stopping criteria>
        for(int i = 0; i < POPULATION_SIZE; i++){
            if(fitness[i] < minimumFitness ||  minimumFitness == 0){
                minimumFitness = fitness[i];
                for(int j = 0; j < NUM_VARIABLES; j++){
                    minimumFitnessGenes[j] = population[i][j];
                }
            }
        }

        if(minimumFitness == stop_criteria){
            break;
        }

        // <YOUR CODE: Here call the crossover function>
        crossover(POPULATION_SIZE, NUM_VARIABLES, fitness, new_population, population, crossover_rate);


        // <YOUR CODE: Here call the mutation function>
        mutate(POPULATION_SIZE, NUM_VARIABLES, new_population, population, Lbound, Ubound, mutate_rate);


        // Now you have the a new population, and it goes to the beginning of loop to re-compute all again
    }

    // <YOUR CODE: Jump to this part of code if the stopping criteria is met before MAX_GENERATIONS is met>

    // ###################################################################################
    // You dont need to change anything here
    // Here we print the CPU time taken for your code
    end_time = clock();
    cpu_time_used = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    printf("CPU time: %f seconds\n", cpu_time_used);
    printf("Best solution found: (%f, %f)", minimumFitnessGenes[0], minimumFitnessGenes[1]);
    printf("\nBest fitness: %f\n\n", minimumFitness);
    // ###################################################################################

    // <Here print out the best solution and objective function value for the best solution like the format>

    return 0;
}
