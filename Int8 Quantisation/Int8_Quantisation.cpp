#include <iostream>

int quantisation(float input, int zeroPoint, float scale);

float dequantisation(int input, int zeroPoint, float scale);

float generateFloat(int lowB, int upperB);

int main() {
	//input of parameters
	std::cout << "input size of Table:";
	int arraySize;
	std::cin >> arraySize;
	float* randomWeights = new float[arraySize * arraySize];
	uint8_t* quantizedWeights = new uint8_t[arraySize * arraySize];
	float* dequantizedWeights = new float[arraySize * arraySize];

	std::cout << "input lower boud of values:";
	int lowerBound;
	std::cin >> lowerBound;
	std::cout << "input upper bound of values:";
	int upperBound;
	std::cin >> upperBound;
	std::cout << "input number of cycles:";
	int cycles;
	std::cin >> cycles;



	for (cycles; cycles > 0; cycles--) {
		//filling the Weights with random floats

		srand(time(0));

		for (int y = 0; y < arraySize; y++) {
			for (int x = 0; x < arraySize; x++) {
				randomWeights[x + arraySize * y] = generateFloat(lowerBound, upperBound);
			}
		}

		//Compute the scale and zero-point
		float minimum = randomWeights[0];
		float maximum = randomWeights[0];

		for (int y = 0; y < arraySize; y++) {
			for (int x = 0; x < arraySize; x++) {
				if (randomWeights[x + arraySize * y] < minimum) { minimum = randomWeights[x + arraySize * y]; }
				if (randomWeights[x + arraySize * y] > maximum) { maximum = randomWeights[x + arraySize * y]; }
			}
		}
		std::cout << "maximum: " << maximum << " minimum: " << minimum << "\n";
		float range = (maximum - minimum);
		//scale = 1 when only one value is present, to avoid division by 0
		float scale = 1;
		if (range != 0) {
			scale = 255 / range;
		}
		int zeroPoint = -(scale * minimum);

		//quantize each weight
		for (int y = 0; y < arraySize; y++) {
			for (int x = 0; x < arraySize; x++) {
				quantizedWeights[x + arraySize * y] = quantisation(randomWeights[x + arraySize * y], zeroPoint, scale);
			}
		}

		//dequantize each weight
		for (int y = 0; y < arraySize; y++) {
			for (int x = 0; x < arraySize; x++) {
				dequantizedWeights[x + arraySize * y] = dequantisation(quantizedWeights[x + arraySize * y], zeroPoint, scale);
			}
		}

		//coumpute differences
		float totalDifference = 0;
		for (int y = 0; y < arraySize; y++) {
			for (int x = 0; x < arraySize; x++) {
				totalDifference += abs(randomWeights[x + arraySize * y] - dequantizedWeights[x + arraySize * y]);
			}
		}
		float averageDifference = totalDifference / (arraySize * arraySize);

		//print values when there are 25 maximum
		if (arraySize < 6) {
			std::cout << "generated values" << "\n";
			for (int y = 0; y < arraySize; y++) {
				for (int x = 0; x < arraySize; x++) {
					std::cout << randomWeights[x + arraySize * y] << "\n";
				}
			}
			std::cout << "quantized Values:" << "\n";
			for (int y = 0; y < arraySize; y++) {
				for (int x = 0; x < arraySize; x++) {
					std::cout << unsigned int(quantizedWeights[x + arraySize * y]) << "\n";
				}
			}
			std::cout << "dequantized Values" << "\n";
			for (int y = 0; y < arraySize; y++) {
				for (int x = 0; x < arraySize; x++) {
					std::cout << dequantizedWeights[x + arraySize * y] << "\n";
				}
			}
		}

		std::cout << "zero-point: " << zeroPoint << " scale: " << scale << "\n";
		std::cout << "total difference of " << (arraySize * arraySize) << " values: " << totalDifference << "\n";
		std::cout << "average difference: " << averageDifference << "\n";
		std::cout << "difference relative to range: " << (averageDifference/(upperBound - lowerBound))*100  << "%\n\n";
	}

	delete[] randomWeights;
	delete[] quantizedWeights;
	delete[] dequantizedWeights;
	return 0;
}


float generateFloat(int lowerB, int upperB) {
	//generates a random mantissa by dividing a random integer by the maximum random int
	float mantissa = (float(rand()) / float(RAND_MAX));
	//generate a random integer part within the given bounds
	float integer = (lowerB + (rand() % (upperB - lowerB)) );
	return integer + mantissa;
}

int quantisation(float input, int zeroPoint, float scale) {
	int quantizedInput = input * scale + zeroPoint;
	//clamping values
	if (quantizedInput > 255) { quantizedInput = 255; }
	if (quantizedInput < 0) { quantizedInput = 0; }
	return quantizedInput;
}

float dequantisation(int input, int zeroPoint, float scale) {
	return ((float(input) - zeroPoint) / scale);
}
