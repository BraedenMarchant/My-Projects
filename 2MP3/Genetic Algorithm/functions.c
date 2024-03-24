// Include everything necessary here
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "functions.h"


double generate_random(double min, double max)
{
    // implement a function to return a value between min and max

    double random = ((double)rand()) / RAND_MAX;
    double range =  (max - min) * random;
    double number = min + range;

    return number;
}

unsigned int generate_int(void)
{
    // implement the function to return a random integer value
    int randomInt = rand() % 30;
    return randomInt;
}

// Function to initialize a random population
void generate_population(int POPULATION_SIZE, int NUM_VARIABLES, double population[POPULATION_SIZE][NUM_VARIABLES], double Lbound[NUM_VARIABLES], double Ubound[NUM_VARIABLES])
{
    // randomly initialize for all values in "population[i][j]""
    for(int i = 0; i < POPULATION_SIZE; i++){
        for(int j = 0; j < NUM_VARIABLES; j++) {
            double pop = generate_random(Lbound[j], Ubound[j]);
            population[i][j] =  pop;
        }
    }
}

// Function to compute the objective function for each member of the population
void compute_objective_function(int POPULATION_SIZE, int NUM_VARIABLES, double population[POPULATION_SIZE][NUM_VARIABLES], double fitness[POPULATION_SIZE])
{
    double objFunction;
    /* compute "fitness[i]"" for each set of decision variables (individual) or each row in "population"
    by calling "Objective_function" */
    for(int i = 0; i < POPULATION_SIZE; i++){
        double objFunctionInput[NUM_VARIABLES];
        for(int j = 0; j < NUM_VARIABLES; j++) {
            objFunctionInput[j] = population[i][j];    
        }

        objFunction = Objective_function(NUM_VARIABLES, objFunctionInput);
        
        // take 1/objfunction because trying to minimize fitness
        fitness[i] = 1 / (1e-6 + objFunction);

    }
}

void crossover(int POPULATION_SIZE, int NUM_VARIABLES, double fitness[POPULATION_SIZE], double new_population[POPULATION_SIZE][NUM_VARIABLES], double population[POPULATION_SIZE][NUM_VARIABLES], double crossover_rate)
{
    /*Implement the logic of crossover function here based on "fitness_probs" or each set
    of decision variables (individual) or each row in "population".
    And save the new population in "new_population"*/
    
    long double fitnessTotal = 0;
    double probability[POPULATION_SIZE];
    double cumulativeProbability[POPULATION_SIZE];
    double randomNumbers[POPULATION_SIZE];
    double randomNumbersForCrossover[POPULATION_SIZE];
    int parentsIndex[POPULATION_SIZE];
    int crossoverPoints[POPULATION_SIZE];

    //*** Calculate fitness total, minimumFitness and generate random array for later calculation         
    for(int i = 0; i < POPULATION_SIZE; i++){
        fitnessTotal += fitness[i];
        randomNumbers[i] = generate_random(0.0, 1.0);
        randomNumbersForCrossover[i] = generate_random(0.0, 1.0);
    }
    
    //normalize the fitness probability by dividing it over the total and take cumulative prob
    for(int i = 0; i < POPULATION_SIZE; i++){
        probability[i]=fitness[i]/fitnessTotal;
        cumulativeProbability[i] = 0;
        if(i == 0){
            cumulativeProbability[i] = probability[i];
        } else {
            for(int j = 0; j <= i; j++){                    
                cumulativeProbability[i] += probability[j];                    
            }   
        }
    }

 
    //** calculate new population before crossover by checking what random value is less than the cumulative then adding that to the new pop
    for(int i = 0; i < POPULATION_SIZE; i++){
        int  j = 0;
        bool assigned = false;
        while(assigned == false){
            if(randomNumbers[i] < cumulativeProbability[j]){                    
                for(int k = 0; k < NUM_VARIABLES; k++){
                    new_population[i][k] = population[j][k];
                }
                assigned = true;
            } else { 
                j++;
            } 
        }
    }

   int crossoverCount = 0;
    
    for(int i = 0; i < POPULATION_SIZE; i++) {
        if(randomNumbersForCrossover[i] < crossover_rate){
            parentsIndex[crossoverCount] = i;
            crossoverPoints[crossoverCount] = rand() % NUM_VARIABLES;
            crossoverCount++;
        }
    }

    //save values of first parent
    double firstParent[1][NUM_VARIABLES];
    for(int j = 0; j < NUM_VARIABLES; j++){
        firstParent[0][j] = new_population[0][j];
    }

    for(int i = 0; i < crossoverCount; i++) {
        int crossoverPoint = crossoverPoints[i];        
        double crossOverVariable = new_population[parentsIndex[i]][crossoverPoint];
        int swapIndex = 0;

        //if not the last parent in the array swap with next element. If last parent swap with first
        if(i != crossoverCount - 1){
            swapIndex = parentsIndex[i + 1];
            for(int j = 0; j < NUM_VARIABLES; j++){
                new_population[parentsIndex[i]][j] = new_population[swapIndex][j];
            }
        } else {
            for(int j = 0; j < NUM_VARIABLES; j++){
                new_population[parentsIndex[i]][j] = firstParent[0][j];
            }    
        }

        //swap crossover
        new_population[parentsIndex[i]][crossoverPoint] = crossOverVariable;
    }
}

void mutate(int POPULATION_SIZE, int NUM_VARIABLES, double new_population[POPULATION_SIZE][NUM_VARIABLES], double population[POPULATION_SIZE][NUM_VARIABLES], double Lbound[NUM_VARIABLES], double Ubound[NUM_VARIABLES], double mutate_rate)
{
    /*Implement the logic of mutation on "new_population" and then copy everything into "population"*/
    int totalGenes = NUM_VARIABLES * POPULATION_SIZE;
    int numberOfMutations = round(totalGenes * mutate_rate);
    int geneToMutate, chromosome, gene;
    double mutationValue;

    //generate random numbers to determine which elements to mutate
    for(int i = 0; i < numberOfMutations; i++){
        chromosome = 0;
        gene = 0;
        geneToMutate = rand() % totalGenes;

        //calculate population chromosome and gene to mutate
        if(geneToMutate != 0) {            
            chromosome = geneToMutate / NUM_VARIABLES;
            gene = geneToMutate % NUM_VARIABLES;

            if(gene == 0) {
                chromosome--;
                gene = NUM_VARIABLES - 1;
            } 
        }

        //generate random mutation value    
        mutationValue = generate_random(Lbound[0], Ubound[0]);
        // printf("geneToMutate: %d\n chromosome: %d\n gene: %d\n mutationValue: %f\n", geneToMutate, chromosome, gene,  mutationValue);

        //set mutation
        new_population[chromosome][gene] = mutationValue;

    }

    //copy new_population  to population
    for(int i = 0; i < POPULATION_SIZE; i++){
        for(int j = 0; j < NUM_VARIABLES; j++) {
            population[i][j] = new_population[i][j];
        }
    }

}