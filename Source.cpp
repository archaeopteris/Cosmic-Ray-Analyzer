#define __STDC_WANT_LIB_EXT1__ 1
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <time.h>

// Function Declaration.
// Converts epoch time to calendar time.
std::string EpochToCalendar(time_t epochTime);

int main()
{
	// Definition of local variables.
	std::string line = "";
	int muonCount = 0;			// Temporary variable tor store muon value at each point of time.
	std::tm dateTime;
	time_t epochTime;

	double muonThreshold = 500;		// Threshold above which a peak should be identified.
	
	// Constructor of a vector to store the muon intensity values.
	std::vector<int> muonIntensityAll;		// Vectore to store Muon Values
	std::vector<time_t> valueTime;			// Vector to store time points of Muon Measurements
	std::vector<float> muonDerivative;		// Vector to store Derivativs for Muon Intensity measurements.

	// Create an instance for ifstream to read from file.
	std::ifstream muonDataFile("MuonDetector_Oct20-21.csv");
//	std::ifstream muonDataFile("Muon_Test.txt");
	// Create an instance for ofstream to write to file.
	std::ofstream outputFile("Muon_Peaks.txt", std::ofstream::trunc);

	// Read the first line of the file which is not needed.
	std::getline(muonDataFile, line);

	std::cout << "Extracting Muon Flux Intensity Values from the file..." << std::endl;
	// Run on the file reading line by line.
	// Extract the Muon flux intensity values.
	while (std::getline(muonDataFile, line))
	{
		// Extract the Muon flux intensity values from the file.
		size_t commaPlace = line.find(",");

		// Convert string date time from the file into calendar time,
		// holding components broken down.
		std::istringstream tempTime(line.substr(0, commaPlace));
		tempTime >> std::get_time(&dateTime, "%d-%b-%y %H:%M:%S.");

		// Check if all date time have been converted correctly.
		if (tempTime.fail())
		{
			std::cout << "Parse of " << line.substr(0, commaPlace) << " failed\n" << std::endl;
			std::cout << "Probably time is not in format of \"dd-mmm-yy H:M:S\"." << std::endl;
			std::cout << "Correct the format of time and try again." << std::endl;

			// Prevent the console screen from closing at the end.
			std::cin.get();

			// CLose the files at the end;
			muonDataFile.close();
			outputFile.close();

			return 0;
		}

		// Add time point to vector;
		valueTime.push_back(std::mktime(&dateTime));
		
		// Convert string to number.
		muonCount = stoi(line.substr(commaPlace + 2));

		muonIntensityAll.push_back(muonCount);
		
	}

	// Resizes the vector to remove all empty elements at the end of a vector.
	// This is done just in case.
	muonIntensityAll.shrink_to_fit();
	valueTime.shrink_to_fit();

	// Hopefully, this should never happen;
	if (muonIntensityAll.size() != valueTime.size())
	{
		std::cout << "Something went wrong..." << std::endl;
		std::cout << "Number of Muon intencity Values and time points is different." << std::endl;
		std::cout << "Number of Muon intencity values is " << muonIntensityAll.size() << std::endl;
		std::cout << "Number of Time points is " << valueTime.size() << std::endl;

		// Prevent the console screen from closing at the end.
		std::cin.get();

		// CLose the files at the end;
		muonDataFile.close();
		outputFile.close();

		return 0;
	}

	std::cout << std::endl << "Calculating derivatives for each point in time..." << std::endl;
	
	// Calculate derivatives for each point in time.
	muonDerivative.push_back(0);
	for (int i = 1; i <= (valueTime.size()-1); i++)
	{
		muonDerivative.push_back((muonIntensityAll[i] - muonIntensityAll[i - 1]) / (valueTime[i] - valueTime[i - 1]));
	}

	// Shrink vectors to fit the values.
	muonIntensityAll.shrink_to_fit();
	valueTime.shrink_to_fit();
	muonDerivative.shrink_to_fit();

	std::cout << "Done." << std::endl;

	std::cout << valueTime.size() << std::endl;
	std::cout << muonIntensityAll.size() << std::endl;
	std::cout << muonDerivative.size() << std::endl;

	std::cout << "Press any key to start writing data to disk." << std::endl;
	std::cin.get();
	std::cout << std::endl << "Applying Threshold." << std::endl;
	std::cout << "Writing data to disk..." << std::endl;
	for (int i = 0; i <= (valueTime.size() - 2); i++)
	{
		if (muonDerivative[i] >= muonThreshold)
		{
			std::cout << EpochToCalendar(valueTime[i]) << ", " << muonDerivative[i] << "\t" << muonIntensityAll[i] << std::endl;
			outputFile << EpochToCalendar(valueTime[i]) << ", " << muonDerivative[i] << "\t" << muonIntensityAll[i] << std::endl;
		}
	}

	std::cout << "All data is written to file 'Muon_Peaks.txt'." << std::endl;
	std::cout << "Press any key to exit..." << std::endl;


	// Prevent the console screen from closing at the end.
	std::cin.get();

	// CLose the files at the end;
	muonDataFile.close();
	outputFile.close();


	return 0;
}

// Converts epoch time to calendar time.
std::string EpochToCalendar(time_t epochTime)
{
	// This will be used later to convert month number to it's textual representation, e.g. 02 to Feb, 08 to Aug.
	std::string monthes[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	struct tm buf;

	// Initialization of buf.
	buf.tm_year = 0; buf.tm_mon = 0; buf.tm_mday = 0;
	buf.tm_hour = 0;   buf.tm_min = 0; buf.tm_sec = 0;
			
	localtime_s(&buf, &epochTime);

	return std::to_string(buf.tm_mday) + "-" + monthes[buf.tm_mon - 1] + "-" + std::to_string(buf.tm_year + 1900)
			+ " " + std::to_string(buf.tm_hour) + ":" + std::to_string(buf.tm_min) + ":" + std::to_string(buf.tm_sec);
}
