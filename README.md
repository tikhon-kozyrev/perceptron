Just a [multilayer perceptron](https://en.wikipedia.org/wiki/Multilayer_perceptron) written in C++.

Input/Output module (io) have classes to have a pleasant interface for reading files (IO::FileReader), writing files (IO::FileWriter) and reading CSV files (IO::CSVReader).

[Linear algebra](https://en.wikipedia.org/wiki/Linear_algebra) module (linalg) is presented by template class LinearAlgebra (parametrized by numeric type) with nested classes for vectors/matrices representation and operations with its. It tries to use OpenMP to accelerating of some calculations. The Matrix class has a mechanism for deciding on parallelization of multiplication based on data on the time of previous multiplications.

[Mathematical statistics](https://en.wikipedia.org/wiki/Mathematical_statistics) module (mathstat) contains an interface for distribution generators (MathStat::Distribution). In addition it contains [continuous uniform distribution](https://en.wikipedia.org/wiki/Continuous_uniform_distribution) implementation (MathStat::UniformDistribution).

[Neural network](https://en.wikipedia.org/wiki/Neural_network) module (nn) contains only one template class NN::TPerceptron for multilayer perceptron representation.


The main program (main.cpp):

1. Use [MNIST](https://en.wikipedia.org/wiki/MNIST_database) dataset in CSV format. I downloaded files [here](https://pjreddie.com/media/files/mnist_train.csv) for training and [here](https://pjreddie.com/media/files/mnist_test.csv) for working.
2. Specializes NN::TPerceptron for using float as numeric type.
3. Trains the network (Demo::Train): creates 7-layer perceptron: from 784 neurons on the input layer through 512, 256, 128, 64, 16 on hidden layers and to 10 on output layer. It then trains this network in batches of 100 samples from the dataset, counts the number of recognized samples, and calculates the network error. When the network will be trained by the training dataset, the perceptron is saved to a file (mnist.nn) in an internal format.
4. Tests the trained network (Demo::Test). Application loads perceptron from the file, saved on previous step.Then it feed the test dataset and calculate percent of recognized samples.

