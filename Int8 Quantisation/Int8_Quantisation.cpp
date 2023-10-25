#include <iostream>

uint8_t** quantisation(float **input,int lengthX, int lengthY) {

	//create the return array
	uint8_t** quantizedOutput{ new uint8_t * [lengthX] };
	for (int x = 0; x < lengthX; x++) {
		quantizedOutput[x] = new uint8_t[lengthY];
	}

	//Compute minimum and maximum, then the scale and zero-point
	float minimum = input[0][0];
	float maximum = input[0][0];

	for (int x = 0; x < lengthX; x++) {
		for (int y = 0; y < lengthY; y++) {
			if (input[x][y] < minimum) { minimum = input[x][y]; }
			if (input[x][y] > maximum) { maximum = input[x][y]; }
		}
	}
	float range = (maximum - minimum);
	//scale = 1 when only one value is present, to avoid division by 0
	float scale = 1;
	if (range != 0) {
		scale = 255 / range;
	}
	int zeroPoint = -(scale * minimum);

	//quantize each weight
	for (int x = 0; x < lengthX; x++) {
		for (int y = 0; y < lengthY; y++) {

			int quantizedInput = input[x][y] * scale + zeroPoint;
			//clamping values
			if (quantizedInput > 255) { quantizedInput = 255; }
			if (quantizedInput < 0) { quantizedInput = 0; }

			quantizedOutput[x][y] = quantizedInput;
		}
	}
	return quantizedOutput;
}

float dequantisation(int input, int zeroPoint, float scale) {
	return ((float(input) - zeroPoint) / scale);
}

float generateFloat(int lowerB, int upperB) {
	//generates a random mantissa by dividing a random integer by the maximum random int
	float mantissa = (float(rand()) / float(RAND_MAX));
	//generate a random integer part within the given bounds
	float integer = (lowerB + (rand() % (upperB - lowerB)));
	return integer + mantissa;
}


int main() {
	//input of parameters
	std::cout << "input X range of Array:";
	int arraySizeX;
	std::cin >> arraySizeX;
	std::cout << "input Y range of Array:";
	int arraySizeY;
	std::cin >> arraySizeY;

	//creating the arrays
	float** randomWeights{ new float*[arraySizeX]};
	uint8_t** quantizedWeights{ new uint8_t* [arraySizeX] };
	float** dequantizedWeights{ new float* [arraySizeX] };
	for (int x = 0; x < arraySizeX; x++) {
		randomWeights[x] = new float[arraySizeY];
		dequantizedWeights[x] = new float[arraySizeY];
	}

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

		for (int x = 0; x < arraySizeX; x++) {
			for (int y = 0; y < arraySizeY; y++) {
				randomWeights[x][y] = generateFloat(lowerBound, upperBound);
			}
		}

		//Compute the scale and zero-point (ONLY FOR TESTING PUPOSES[needed for dequantisation])
		float minimum = randomWeights[0][0];
		float maximum = randomWeights[0][0];

		for (int x = 0; x < arraySizeX; x++) {
			for (int y = 0; y < arraySizeY; y++) {
				if (randomWeights[x][y] < minimum) { minimum = randomWeights[x][y]; }
				if (randomWeights[x][y] > maximum) { maximum = randomWeights[x][y]; }
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
		quantizedWeights = quantisation(randomWeights, arraySizeX, arraySizeY);

		//dequantize each weight
		for (int x = 0; x < arraySizeX; x++) {
			for (int y = 0; y < arraySizeY; y++) {
				dequantizedWeights[x][y] = dequantisation(quantizedWeights[x][y], zeroPoint, scale);
			}
		}

		//coumpute differences
		float totalDifference = 0;
		for (int x = 0; x < arraySizeX; x++) {
			for (int y = 0; y < arraySizeY; y++) {
				totalDifference += abs(randomWeights[x][y] - dequantizedWeights[x][y]);
			}
		}
		float averageDifference = totalDifference / (arraySizeX * arraySizeY);

		//print values when there are 25 maximum
		if ((arraySizeX < 6) && (arraySizeY < 6)) {
			std::cout << "generated values" << "\n";
			for (int x = 0; x < arraySizeX; x++) {
				for (int y = 0; y < arraySizeY; y++) {
					std::cout << randomWeights[x][y] << "\n";
				}
			}
			std::cout << "quantized Values:" << "\n";
			for (int x = 0; x < arraySizeX; x++) {
				for (int y = 0; y < arraySizeY; y++) {
					std::cout << unsigned int(quantizedWeights[x][y]) << "\n";
				}
			}
			std::cout << "dequantized Values" << "\n";
			for (int x = 0; x < arraySizeX; x++) {
				for (int y = 0; y < arraySizeY; y++) {
					std::cout << dequantizedWeights[x][y] << "\n";
				}
			}
		}

		std::cout << "zero-point: " << zeroPoint << " scale: " << scale << "\n";
		std::cout << "total difference of " << (arraySizeX * arraySizeY) << " values: " << totalDifference << "\n";
		std::cout << "average difference: " << averageDifference << "\n";
		std::cout << "difference relative to range: " << (averageDifference/(upperBound - lowerBound))*100  << "%\n\n";
	}

	//cleanup
	for (int x = 0; x < arraySizeX; x++) {
		delete[] randomWeights[x];
		delete[] quantizedWeights[x];
		delete[] dequantizedWeights[x];
	}
	delete[] randomWeights;
	delete[] quantizedWeights;
	delete[] dequantizedWeights;
	return 0;
}




