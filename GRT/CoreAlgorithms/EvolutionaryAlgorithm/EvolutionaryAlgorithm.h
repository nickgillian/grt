/**
 @file
 @author  Nicholas Gillian <ngillian@media.mit.edu>
 */

/*
 GRT MIT License
 Copyright (c) <2013> <Nicholas Gillian, Media Lab, MIT>
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all copies or substantial
 portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef GRT_EVOLUTIONARY_ALGORITHM_HEADER
#define GRT_EVOLUTIONARY_ALGORITHM_HEADER

#include "Individual.h"

GRT_BEGIN_NAMESPACE

/**
 @brief This class implements a template based EvolutionaryAlgorithm.
 
 @remark This implementation is based on Michalewicz, Zbigniew. Genetic algorithms + data structures= evolution programs. springer, 1996.
 */
template <typename INDIVIDUAL>
class EvolutionaryAlgorithm : public MLBase{

public:
    /**
     Default Constructor, if the populationSize and geneSize parameters are greater than zero then the algorithm will
     be initialized.
     
     @param populationSize: the number of individuals in the population. Default value = 0
     @param geneSize: the number of elements in each individuals gene. Default value = 0
     */
    EvolutionaryAlgorithm(const UINT populationSize = 0,const UINT geneSize = 0) : MLBase("EVO")
    {
        maxIteration = 1000;
        minNumIterationsNoChange = 1;
        storeRate = 1;
        bestIndividualIndex = 0;
        bestIndividualFitness = 0;
        mutationRate = 0.01;
        minChange = 1.0e-5;
        baiseCoeff = 2.0;
        initialized = false;
        useElitism = true;
        storeHistory = true;
        baiseWeights = true;
        
        initPopulation( populationSize, geneSize );
    }
    
    /**
     Default Destructor.
     */
    virtual ~EvolutionaryAlgorithm(){
        
    }
    
    /**
     This operator allows you to directly access each individual in the population. The user must ensure that index is a valid number
     in the range of [0 populationSize-1].
     
     @param index: the index of the individual you want to access
     @return returns a reference to the individual in the population at the specific index
     */
    INDIVIDUAL& operator[](const UINT &index){
        return population[ index ];
    }
    
    /**
     This function initalizes the population, setting each individual's genes to a random value in the range [0.0 1.0].
     Both the populationSize and geneSize parameters must be greater than zero.
     
     @param populationSize: the number of individuals in the population
     @param geneSize: the number of elements in each individuals gene
     @return returns true if the population was initalized, false otherwise
     */
    virtual bool initPopulation(const UINT populationSize,const UINT geneSize){
        
        initialized = false;
        this->populationSize = 0;
        this->geneSize = 0;
        bestIndividualIndex = 0;
        bestIndividualFitness = 0;
        population.clear();
        populationWeights.clear();
        accumSumLookup.clear();
        populationHistory.clear();
        fitnessHistory.clear();
        
        if( populationSize == 0 || geneSize == 0 ) return false;
        
        //Init the memory
        this->populationSize = populationSize;
        this->geneSize = geneSize;
        population.resize( populationSize );
        populationWeights.resize( populationSize );
        accumSumLookup.resize( populationSize );
        
        //Init each individual
        UINT i = 0;
        UINT index = 0;
        typename Vector< INDIVIDUAL >::iterator populationIter = population.begin();
        Vector< IndexedDouble >::iterator weightsIter = populationWeights.begin();
        VectorFloat::iterator geneIter;
        
        while( populationIter != population.end() ){
            populationIter->fitness = 0;
            populationIter->gene.resize( geneSize );
            
            //Randomize the gene
            for(i=0; i<geneSize; i++){
                populationIter->gene[ i ] = generateRandomGeneValue();
            }
            
            weightsIter->value = populationIter->fitness;
            weightsIter->index = index++;
            
            populationIter++;
            weightsIter++;
        }
        
        //Save the initial population as the parents
        parents = population;
        
        initialized = true;
        
        return true;
    }
    
    /**
     This function estimates the populations fitness, based on the training data. It will return a reference to the bestFitness value
     and the index of the individual with the best fitness.
     
     @param trainingData: a reference to the trainingData that will be used to estimate the fitness
     @param bestFitness: a reference that will return the best fitness value
     @param bestIndex: a reference that will return the index of the individual with the best fitness
     @return returns true if the population fitness was estimated, false otherwise
     */
    virtual bool estimatePopulationFitness( const MatrixFloat &trainingData, Float &bestFitness, UINT &bestIndex ){
        
        UINT index = 0;
        bestFitness = 0;
        bestIndex = 0;
        
        if( !initialized ) return false;
        
        typename Vector< INDIVIDUAL >::iterator populationIter = population.begin();
        Vector< IndexedDouble >::iterator weightsIter = populationWeights.begin();
        
        while( populationIter != population.end() ){
            //Compute the fitness of the current individual
            weightsIter->value = evaluateFitness( *populationIter, trainingData );
            weightsIter->index = index++;
            
            //Check to see if this is the best fitness so far
            if( weightsIter->value > bestFitness ){
                bestFitness = weightsIter->value;
                bestIndex = weightsIter->index;
            }
            
            //Update the iterators
            populationIter++;
            weightsIter++;
        }
        
        return true;
    }
    
    /**
     This function evolves the current population, based on the fitness of each individual.  You should compute the fitness of each individual 
     first before using this function.
     
     @return returns true if the population was evolved, false otherwise
     */
    virtual bool evolvePopulation(){
        
        if( !initialized ) return false;
        
        UINT i=0;
        UINT index = 0;
        UINT mom = 0;
        UINT dad = 0;
        UINT crossOverPoint = 0;
        typename Vector< INDIVIDUAL >::iterator populationIter = population.begin();
        Vector< IndexedDouble >::iterator weightsIter = populationWeights.begin();
        
        //Get the current weights values
        weightsIter = populationWeights.begin();
        while( populationIter != population.end() ){
            weightsIter->value = baiseWeights ? pow( populationIter->fitness, baiseCoeff ) : populationIter->fitness;
            weightsIter->index = index++;
            
            populationIter++;
            weightsIter++;
        }
        
        //Sort the weighted values by value in ascending order (so the least likely value is first, the second least likely is second, etc...
        sort(populationWeights.begin(),populationWeights.end(),IndexedDouble::sortIndexedDoubleByValueAscending);
        
        //Create the accumulated sum lookup table
        accumSumLookup[0] = populationWeights[0].value;
        for(unsigned int i=1; i<populationSize; i++){
            accumSumLookup[i] = accumSumLookup[i-1] + populationWeights[i].value;
        }
        
        if( accumSumLookup[populationSize-1] == 0 ){
            warningLog << "evolvePopulation() - The accum sum is zero!" << std::endl;
        }
        
        //Reset the population iterator
        populationIter = population.begin();
        
        if( useElitism ){
            //The first child is simply a copy of the best parent
            populationIter->gene = parents[ bestIndividualIndex ].gene;
            populationIter++;
        }
        
        //This is the main evolve loop, at each iteration we do the following until we reach the end of the population
        //- Randomly select a mom and dad from the parents population (parents with a better fitness have a better chance of being selected)
        //- Randomly select a cross over point (this is an index that sets what point along the gene we will merge the data from mom and dad)
        //- Create two individuals (if possible) by combining the gene data from mom and dad
        //- The first child will use the following genes [mom |crossOverPoint| dad], whereas the second child will use [dad |crossOverPoint| mom]
        //- After the cross over, each gene will be randomly mutated
        index = 0;
        while( populationIter != population.end() ){
            
            //Randomly select the parents, individuals with higher weights will have a better chance of being selected
            mom = rand.getRandomNumberWeighted( populationWeights, accumSumLookup );
            dad = rand.getRandomNumberWeighted( populationWeights, accumSumLookup );
            
            //Select the cross over point
            crossOverPoint = rand.getRandomNumberInt(0, geneSize);
            
            //Generate the new individual using cross over (this is the first child)
            for(i=0; i<geneSize; i++){
                if( i < crossOverPoint ) populationIter->gene[i] = parents[ mom ].gene[i];
                else populationIter->gene[i] = parents[ dad ].gene[i];
            }
            
            //Perform random mutation
            for(i=0; i<geneSize; i++){
                if( rand.getRandomNumberUniform(0.0,1.0) <= mutationRate ){
                    populationIter->gene[ i ] = generateRandomGeneValue();
                }
            }
            
            //Update the iterator
            populationIter++;
            
            //Generate the second child (as long as we have not got to the end of the population)
            if( populationIter != population.end() ){
                
                for(i=0; i<geneSize; i++){
                    if( i < crossOverPoint ) populationIter->gene[i] = parents[ dad ].gene[i];
                    else populationIter->gene[i] = parents[ mom ].gene[i];
                }
                
                //Perform random mutation
                for(i=0; i<geneSize; i++){
                    if( rand.getRandomNumberUniform(0.0,1.0) <= mutationRate ){
                        populationIter->gene[ i ] = generateRandomGeneValue();
                    }
                }
                
                //Update the iterator
                populationIter++;
            }
            
        }
        
        //Store the parents for the next iteration
        parents = population;
        
        return true;
    }
    
    /**
     This function evaluates the fitness of an individual, using the training data.  This function assumes that each row in the
     training data is an example, each column must therefore match the geneSize.  
     
     @param individual: a reference to the individual you want to compute the fitness for
     @param trainingData: a reference to the training data that will be used to compute the individual's fitness
     @return returns the fitness of the individual
     */
    virtual Float evaluateFitness( INDIVIDUAL &individual, const MatrixFloat &trainingData ){
        
        individual.fitness = 0;
        
        if( !initialized ) return 0;
        
        if( trainingData.getNumCols() != geneSize ) return 0;
        
        UINT M = trainingData.getNumRows();
        Float error = 0;
        Float minError = grt_numeric_limits< Float >::max();
        
        for(UINT i=0; i<M; i++){
            error = 0;
            //Compute the squared distance
            for(UINT j=0; j<geneSize; j++){
                error += ( trainingData[i][j] - individual.gene[j] ) * ( trainingData[i][j] - individual.gene[j] );
            }
            if( error < minError ) minError = error;
        }
        //Make sure the minError is not zero
        minError += 0.00001;
        minError /= Float(geneSize);
        
        //Good individuals should have a high fitness
        individual.fitness = 1.0/(minError*minError);
        
        return individual.fitness;
    }

    virtual bool train(const MatrixFloat &trainingData){
        
        if( !initialized ) return false;
        
        UINT currentIteration = 0;
        UINT numIterationsNoChange = 0;
        bool keepTraining = true;
        Float lastBestFitness = 0;
        
        if( storeHistory ){
            populationHistory.reserve( maxIteration/storeRate );
            fitnessHistory.reserve( maxIteration/storeRate );
        }
        
        //Init the population
        initPopulation( populationSize, geneSize );
        
        //Compute the fitness of the initial population
        estimatePopulationFitness( trainingData, bestIndividualFitness, bestIndividualIndex );
        lastBestFitness = bestIndividualFitness;
        
        if( storeHistory ){
            populationHistory.push_back( population );
            fitnessHistory.push_back( IndexedDouble(bestIndividualIndex, bestIndividualFitness) );
        }
        
        //Start the main loop
        while( keepTraining ){
            
            //Perform the selection
            if( !evolvePopulation() ){
                errorLog << "Failed to evolve population" << std::endl;
                return false;
            }
            
            //Compute population fitness
            if( !estimatePopulationFitness( trainingData, bestIndividualFitness, bestIndividualIndex ) ){
                return false;
            }
            
            Float delta = fabs( bestIndividualFitness-lastBestFitness );
            lastBestFitness = bestIndividualFitness;
            
            trainingLog << "Iteration: " << currentIteration << "\tBestFitness: " << bestIndividualFitness << "\tBestIndex: " << bestIndividualIndex << "\tDelta: " << delta << "\tNumIterationsNoChange: " << numIterationsNoChange << std::endl;
            
            if( currentIteration >= maxIteration ){
                keepTraining = false;
                trainingLog << "Max Iteration Reached!" << std::endl;
            }
            
            if( delta <= minChange ){
                if( ++numIterationsNoChange >= minNumIterationsNoChange ){
                    keepTraining = false;
                    trainingLog << "Min Changed Reached!" << std::endl;
                }
            }else{
                numIterationsNoChange = 0;
            }
            
            if( customConvergenceCheck() ){
                keepTraining = false;
                trainingLog << "Custom Convergance Triggered!" << std::endl;
            }
            
            //Update the iteration
            currentIteration++;
            
            //Save the current population
            if( currentIteration % storeRate == 0 && storeHistory ){
                populationHistory.push_back( population );
                fitnessHistory.push_back( IndexedDouble(bestIndividualIndex, bestIndividualFitness) );
            }
        }
        
        return true;
    }
    
    UINT getPopulationSize() const{
        return populationSize;
    }
    
    bool getInitialized() const{
        return initialized;
    }
    
    Vector< INDIVIDUAL > getPopulation() const{
        return population;
    }
    
    bool setPopulationSize(const UINT populationSize){
        this->populationSize = populationSize;
        return true;
    }
    
    bool setMinNumIterationsNoChange(const UINT minNumIterationsNoChange){
        this->minNumIterationsNoChange = minNumIterationsNoChange;
        return true;
    }
    
    bool setMaxIterations(const UINT maxIteration){
        this->maxIteration = maxIteration;
        return true;
    }
    
    bool setStoreRate(const UINT storeRate){
        this->storeRate = storeRate;
        return true;
    }
    
    bool setStoreHistory(const bool storeHistory){
        this->storeHistory = storeHistory;
        return true;
    }
    
    bool setBaiseWeights(const bool baiseWeights){
        this->baiseWeights = baiseWeights;
        return true;
    }
    
    bool setBaiseCoeff(const Float baiseCoeff){
        this->baiseCoeff = baiseCoeff;
        return true;
    }
    
    bool setMutationRate(const Float mutationRate){
        this->mutationRate = mutationRate;
        return true;
    }
    
    bool setMinChange(const Float minChange){
        this->minChange = minChange;
        return true;
    }
    
    virtual bool setPopulation( const Vector< INDIVIDUAL > &newPopulation ){
        
        if( newPopulation.size() == 0 ) return false;
        
        population = newPopulation;
        populationSize = (UINT)population.size();
        populationWeights.resize( populationSize );
        accumSumLookup.resize( populationSize );
        
        UINT index = 0;
        typename Vector< INDIVIDUAL >::iterator populationIter = population.begin();
        Vector< IndexedDouble >::iterator weightsIter = populationWeights.begin();
        VectorFloat::iterator geneIter;
    
        while( populationIter != population.end() ){
            weightsIter->value = populationIter->fitness;
            weightsIter->index = index++;
            
            populationIter++;
            weightsIter++;
        }
        
        return true;
    }
    
    virtual inline Float generateRandomGeneValue(){
        return rand.getRandomNumberUniform(0.0,1.0);
    }
    
    virtual bool customConvergenceCheck(){
        return false;
    }
    
    virtual bool printBest() const{
        if( !initialized ) return false;
        
		std::cout << "BestIndividual: ";
		for(UINT i=0; i<geneSize; i++){
			std::cout << population[ bestIndividualIndex ].gene[i] << "\t";
		}
		std::cout << std::endl;
        return true;
    }
    
public:
    
    bool initialized;
    bool useElitism;
    bool storeHistory;
    bool baiseWeights;
    UINT populationSize;
    UINT geneSize;
    UINT minNumIterationsNoChange;
    UINT maxIteration;
    UINT storeRate;
    UINT bestIndividualIndex;
    Float bestIndividualFitness;
    Float mutationRate;
    Float minChange;
    Float baiseCoeff;
    Random rand;
    Vector< INDIVIDUAL > population;
    Vector< INDIVIDUAL > parents;
    Vector< IndexedDouble > populationWeights;
    Vector< Vector< INDIVIDUAL > > populationHistory;
    Vector< IndexedDouble > fitnessHistory;
    VectorFloat accumSumLookup;
};
    
GRT_END_NAMESPACE

#endif //GRT_EVOLUTIONARY_ALGORITHM_HEADER
