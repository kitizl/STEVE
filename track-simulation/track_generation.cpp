#include <stdlib.h>
#include <iostream>
#include <valarray>

const double speed_of_light = 299792458;
const double PI = 3.1415926535898;




int main(int argc, char* argv[]){
	// simulating the positrons moving through the detector

	// defining parameters of the dynamics
	double charge = +1.6E-19; // C
	double mass = +9.1E-31; // kg
	std::valarray<double> magField = {0.0,0.0,3.0}; // T (purely in the z-axis)
	
	int numOfRuns = 100; // generating 100 tracks

	std::valarray<double> init_pos = generatePosition();
	std::valarray<double> init_vel = generateVelocity(50,280); // args : lower and upper limits on energy




	return 0;
}