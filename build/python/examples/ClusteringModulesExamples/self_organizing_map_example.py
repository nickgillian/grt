import GRT
import sys
import numpy as np

def main():
    # Create a new KMeans instance
    som = GRT.SelfOrganizingMap(64, GRT.SelfOrganizingMap.RANDOM_NETWORK, 20)

    r = np.random.uniform(0, 1, (1000,))
    data = np.array([r, np.zeros((1000,)), 1-r]).T

    som.enableScaling( True )
    som.setSigmaWeight( 0.5 )
    if not som.train( data ):
        print("Failed to train model!\n")
        sys.exit(1)

    print("Weights Matrix:\n" + str(som.getWeightsMatrix()))

if __name__ == '__main__':
    main()
    sys.exit(0)
