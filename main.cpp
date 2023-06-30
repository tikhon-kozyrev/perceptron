/*
	Copyright (c) 2023 Tikhon Kozyrev (tikhon.kozyrev@gmail.com)
*/
#include <iostream>
#include <iomanip>
#include "io/csvreader.hpp"
#include "nn/perceptron.hpp"

using Perceptron = NN::TPerceptron<float>;

namespace Demo {
	void Train() {
		auto sigmoid = [](Perceptron::Number &x) -> void {
			x = 1./(1. + exp(-x));
		};
		auto dsigmoid = [](const Perceptron::Number &x) -> Perceptron::Number {
			return x*(1.-x);
		};
		Perceptron net(0.001, sigmoid, dsigmoid);

		do {
			IO::CSVReader csv;
			std::vector<std::string> row;
			size_t ds_rowsize = 1+784; // each row in dataset must contains: the label [0-9] and input layer 28x28 or 784 pixels [0-255]
			if (!csv.Open("mnist_train.csv")) { // downloaded from https://pjreddie.com/media/files/mnist_train.csv
				std::cerr << "Can't open mnist_train.csv" << std::endl;
				break;
			}
			if (!csv.ReadRow(row, ',')) { // The first row contains header, so just skip it
				std::cerr << "error reading mnist_train.csv" << std::endl;
				break;
			}
			if (ds_rowsize != row.size()) { // if header row has incorrect size (incorrect dataset?)
				std::cerr << "error dataset header row size" << std::endl;
				break;
			}

			net.BuildTopology({784, 512, 256, 128, 64, 16, 10}); // create internal network infrastructure (layers, weights and so on...)
			net.Init(); // fill the net by random values

			Perceptron::Sample sample(net.InSize(), net.OutSize()); // create sample for the network;
			std::fill(sample.output.begin(), sample.output.end(), 0); // fill output by 0
			uint8_t lastLabel = 0; // remember last label to fast clearing output
			struct {
				size_t right;
				double errorSum;
			} stat = {0, 0.}; // will calculate statistic
			size_t rowId = 0;
			std::chrono::high_resolution_clock local_clock;
			auto start = local_clock.now();
			auto stop = start;
			while (csv.ReadRow(row, ',')) { // read dataset row by row...
				rowId++;
				if (ds_rowsize != row.size()) { // row has incorrect size, so skip it
					std::cerr << "row #" << rowId << " has incorrect size... skipping" << std::endl;
					continue;
				}
				try { // try to load sample
					sample.output[lastLabel]=0; // clear last output
					int label = std::stoi(row[0]);
					if ((label < 0) || (label > 9)) {
						std::cerr << "row #" << rowId << " contains out-of-range label... skipping" << std::endl;
						continue;
					}
					sample.output[label] = 1; // prepare output layer (right answer)
					lastLabel = label; // remember last label for next row
					for (size_t i=1; i<row.size(); ++i) {
						int pixel = std::stoi(row[i]);
						if ((pixel < 0) || (pixel > 255)) {
							std::cerr << "row #" << rowId << " contains out-of-range pixel... skipping" << std::endl;
							continue;
						}
						sample.input[i-1] = pixel/255.; // normalize pixel bright to (0-1) range
					}
				} catch (const std::invalid_argument &) {
					std::cerr << "row #" << rowId << " contains non-number... skipping" << std::endl;
					continue;
				} catch (const std::out_of_range &) {
					std::cerr << "row #" << rowId << " contains out-of-range values... skipping" << std::endl;
					continue;
				}
				// at this moment we have loaded sample (input layer and ideal output value)

				Perceptron::Vector answer = net.feedForward(sample.input); // feed the net
				int maxLabel = 0;
				double maxLabelWeight = -1;
				if (true) { // check result
					for (size_t k = 0; k < answer.size(); k++) {
						if (answer[k] > maxLabelWeight) {
							maxLabelWeight = answer[k];
							maxLabel = k;
						}
					}
				}
				if (true) { // update statistics
					if (lastLabel == maxLabel) { // net guess the lable right
						stat.right++;
					}
					for (size_t k = 0; k < answer.size(); k++) {
						stat.errorSum += ((lastLabel == k)?1:0 - answer[k]) * ((lastLabel == k)?1:0 - answer[k]);
					}
				}
				net.backpropagation(sample.output); // training

				if ((rowId > 0) && (0 == rowId % 100)) { // each 100 rows out statistic and reset it
					stop = local_clock.now();
					std::cout << std::setw(6) << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms " << std::setw(6) << rowId << " processed, guessed: " << std::setw(3) << (int)stat.right << "%, error: " << (int)stat.errorSum << std::endl;
					stat = {0, 0.};
					start = stop;
				}
			}
			net.SaveToFile("mnist.nn");
		} while (false);
	}
	void Test() {
		auto sigmoid = [](Perceptron::Number &x) -> void {
			x = 1./(1. + exp(-x));
		};
		auto dsigmoid = [](const Perceptron::Number &x) -> Perceptron::Number {
			return x*(1.-x);
		};
		Perceptron net(0.001, sigmoid, dsigmoid);

		do {
			IO::CSVReader csv;
			std::vector<std::string> row;
			size_t ds_rowsize = 1+784; // each row in dataset must contains: the label [0-9] and input layer 28x28 or 784 pixels [0-255]
			if (!csv.Open("mnist_test.csv")) { // downloaded from https://pjreddie.com/media/files/mnist_test.csv
				std::cerr << "Can't open mnist_test.csv" << std::endl;
				break;
			}
			if (!csv.ReadRow(row, ',')) { // The first row contains header, so just skip it
				std::cerr << "error reading mnist_test.csv" << std::endl;
				break;
			}
			if (ds_rowsize != row.size()) { // if header row has incorrect size (incorrect dataset?)
				std::cerr << "error dataset header row size" << std::endl;
				break;
			}
			net.LoadFromFile("mnist.nn"); // load trained network from file

			Perceptron::Sample sample(net.InSize(), net.OutSize()); // create sample for the network;
			uint8_t lastLabel = 0; // remember last label to fast clearing output
			struct {
				size_t right;
				double errorSum;
			} local_stat = {0, 0.}; // will calculate local (per 100 samples)
			auto global_stat = local_stat; //  and global (whole dataset) statistic
			size_t rowId = 0;
			std::chrono::high_resolution_clock local_clock;
			auto start = local_clock.now();
			auto stop = start;
			while (csv.ReadRow(row, ',')) { // read dataset row by row...
				rowId++;
				if (ds_rowsize != row.size()) { // row has incorrect size, so skip it
					std::cerr << "row #" << rowId << " has incorrect size... skipping" << std::endl;
					continue;
				}
				try { // try to load sample
					sample.output[lastLabel]=0; // clear last output
					int label = std::stoi(row[0]);
					if ((label < 0) || (label > 9)) {
						std::cerr << "row #" << rowId << " contains out-of-range label... skipping" << std::endl;
						continue;
					}
					sample.output[label] = 1; // prepare output layer (right answer)
					lastLabel = label; // remember last label for next row
					for (size_t i=1; i<row.size(); ++i) {
						int pixel = std::stoi(row[i]);
						if ((pixel < 0) || (pixel > 255)) {
							std::cerr << "row #" << rowId << " contains out-of-range pixel... skipping" << std::endl;
							continue;
						}
						sample.input[i-1] = pixel/255.; // normalize pixel bright to (0-1) range
					}
				} catch (const std::invalid_argument &) {
					std::cerr << "row #" << rowId << " contains non-number... skipping" << std::endl;
					continue;
				} catch (const std::out_of_range &) {
					std::cerr << "row #" << rowId << " contains out-of-range values... skipping" << std::endl;
					continue;
				}
				// at this moment we have loaded sample (input layer and ideal output value)

				Perceptron::Vector answer = net.feedForward(sample.input); // feed the net
				int maxLabel = 0;
				double maxLabelWeight = -1;
				if (true) { // check result
					for (size_t k = 0; k < answer.size(); k++) {
						if (answer[k] > maxLabelWeight) {
							maxLabelWeight = answer[k];
							maxLabel = k;
						}
					}
				}
				if (true) { // update statistics
					if (lastLabel == maxLabel) { // net guess the lable right
						local_stat.right++;
						global_stat.right++;
					}
					for (size_t k = 0; k < answer.size(); k++) {
						local_stat.errorSum += ((lastLabel == k)?1:0 - answer[k]) * ((lastLabel == k)?1:0 - answer[k]);
						global_stat.errorSum += ((lastLabel == k)?1:0 - answer[k]) * ((lastLabel == k)?1:0 - answer[k]);
					}
				}
				if ((rowId > 0) && (0 == rowId % 100)) { // each 100 rows out statistic and reset it
					stop = local_clock.now();
					std::cout << std::setw(8) << std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count() << " ms " << std::setw(5) << rowId << " processed, guessed: " << std::setw(3) << (int)local_stat.right << "%, error: " << (int)local_stat.errorSum << std::endl;
					local_stat = {0, 0.};
					start = stop;
				}
			}
			std::cout << "Total guessed: " << global_stat.right*100./rowId << "%" << std::endl;
		} while (false);
	}
}

int main() {
	Demo::Train();
	Demo::Test();

	std::cout << "Done." << std::endl;
	return 0;
}
