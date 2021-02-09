#include "pvector.h"
#include <fstream>
#include <random>
#include <chrono>
#include <string>
#include <tuple>
#include <stdlib.h>

// defining some contexts that are invariant of the experiment
const double speed_of_light = 299792458;
const double PI = 3.1415926535898;
const double sensorDim = 97.28e-3; // 97.28mm is the size of each sensor
const bool INHOMOGENOUS_B_FIELD = false; // turn on for controlling exact B field
const bool SCATTERING = false; // turn on for including particle-detector scattering



std::vector<double> magneticField(double x, double y, double z){
	// given a (x,y,z) a specific value of the magnetic field will be returned

	if(INHOMOGENOUS_B_FIELD){
		// assuming these parameters are provided in advance
		// default goes to 0,0,3
		double Bx = 0.0;
		double By = 0.0;
		double Bz = 3.0;
		std::vector<double>magField = {Bx,By,Bz};		
	}
	else{
		std::vector<double> magField = {0.0,0.0,3.0};
	}
	
	return magField;
}


// limits have been set purely based on the dimensions of the pixels
std::vector<double> generatePositions(double rlim=2*sensorDim+(75.5e-3), double zlim=194.5e-3, double core=75.5e-3){
	// This function generates a vector<double> {x_coord, y_coord, z_coord}, based on the given limitations

	// using a time-based seed
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	// initializing random generator enginer
	std::default_random_engine generator (seed);

	// producing the uniform distribution
	std::uniform_real_distribution<double> rad_distribution (core,rlim); // limits : from the inner core to the walls of the detector
	std::uniform_real_distribution<double> phi_dis (0,2*PI);
	std::uniform_real_distribution<double> z_dis (-zlim,zlim); // limits : -zlim in the bottom to +zlim on the top
	double rad = rad_distribution(generator);
	double phi = phi_dis(generator);
	double z = z_dis(generator);

	// generating random position
	std::vector<double> pos = {rad*std::cos(phi), rad*std::sin(phi), z};

	// no need to check for limits, it is inbuilt from the generation

	return pos;
}

double energy_to_speed(double energy, double mass = 0.511){
	// input energy in MeV
		// input mass in MeV/c^2

	double x = 1/(1 + energy/mass);
	double speed = speed_of_light*std::sqrt(1 - x*x);
	return speed;
}

std::tuple<std::vector<double>, double> generateVelocities(double lower_energy, double upper_energy, double mass=9.1e-31){

	// using a time-based seed
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	// initializing random generator enginer
	std::default_random_engine generator (seed);

	// producing the uniform distribution
	std::uniform_real_distribution<double> distribution(-1,+1); // making vectors that are unit vectors, basically
	std::uniform_real_distribution<double> energy_distribution(lower_energy,upper_energy); // "choosing" an energy
		
		double energy_of_particle = energy_distribution(generator); // actually choosing an energy (in MeV)
		double speed = energy_to_speed(energy_of_particle); // getting the speed out of the energy

	// generating random position
	pVector vels({distribution(generator), distribution(generator), distribution(generator)});
		// first normalizing vector and scaling it with speed
	vels = pVector::scale(speed/pVector::norm(vels),vels);

	return std::make_tuple(vels.elements, energy_of_particle);
}

double gamma_factor(double speed){
	double boost = speed/speed_of_light; // calculating boost first
	return (1/std::sqrt(1 - boost*boost));  // this returns nan and 1 + 0.5 b^2 always gives 1.5
}

int isOOB(pVector pos){
	// a function that checks to see if the current position of the particle is out of bounds
	std::vector<double> posv = pos.elements;

	double r2 = posv[0]*posv[0] + posv[1]*posv[1];

	if(posv[2] > 2*sensorDim || posv[2] < -2*sensorDim){
		// up-down boundary
		return 1; // 1 for true, I don't trust its boolean
	}
	else if(r2 < (75.5e-3)*(75.5e-3) || r2 > (2*sensorDim+(75.5e-3))*(2*sensorDim + (75.5e-3))){
		// radial in or out boundary
		return 1;
	}
	return 0;
}


int lorentzSimulator(std::vector<double> consts, pVector B, pVector initial_position, pVector initial_velocity, double kinetic,std::string run_id = ""){
	// this function simulates for given initial conditions

	std::vector<pVector> poss = {initial_position}; // a vector (list) of pVectors indicating position
	std::vector<pVector> vels = {initial_velocity}; // a vectr (list) of pVectors indicating velocity

	// extracting the constants needed
	double q = consts[0];
	double c = consts[2];
		double gam = 1 + kinetic/(0.511); // 0.511 being the mass of the thing (will make the code better someday)
	double m = gam*consts[1]; // using the relativity corrected mass
		
		// displaying also what the gamma factor is for this track
		//
		std::cout<< "Gamma : " << gam << "\n";

	double dt = 1e-15; // timestep (calculated by distance/speed of light)

	// doing the Euler method -- dt and the number of points optimized for loops
	for(long long i = 1; i < 1500000; i++){
		pVector lorForce = pVector::scale(q/m, pVector::cross(vels.back(),B)); // calculating the lorentz force
		poss.push_back(pVector::add(poss.back(), pVector::scale(dt,vels.back())));   // x_n = x_n-1 + h*vx
		vels.push_back(pVector::add(vels.back(), pVector::scale(dt,lorForce)));      // vx_n = vx_n-1 + h*(lorentz force)

		// check if the position has gone out of bounds and break; when that happens
				if( isOOB(poss.back()) == 1 ){
					break;
				}

	}

	// storing the position and speed data
	std::ofstream simPosData;
	std::ofstream simVelData;
	simPosData.open("../data/tracks100/simPosData-" + run_id + ".csv");
	simVelData.open("../data/tracks100/simVelData-" + run_id + ".csv");
	simPosData << "x,y,z" << std::endl;
	simVelData << "vx,vy,vz" << std::endl;
	for(int i = 0; i < poss.size(); i++){
		simPosData << poss[i].elements[0] << ","<< poss[i].elements[1] << ","<< poss[i].elements[2] << std::endl;
		simVelData << vels[i].elements[0] << ","<< vels[i].elements[1] << ","<< vels[i].elements[2] << std::endl;
	}
	simPosData.close();
	simVelData.close();
 	return 0;
}
int main(int argc, char* argv[]){
	// Let's try to code the Lorentz equation
	std::cout<<"Starting Track Generation\n";
	// Firstly, entering the properties of a positron
	double charge = +1.6e-19; // coulombs
	double mass = +9.1e-31; // kilograms
	// Now, let's put in the electric and magnetic fields
	// assuming E = 0, because I'm prototyping

	pVector magField({0.0,0.0,3}); // Tesla (purely in the z-axis) (homogenous approximation)

	// the differential equation we're concerned with is
	// mx'' = qv x B
	// deedee x = scale(charge/mass, cross(velocity, magField))

	std::vector<double> cons = {charge,mass,speed_of_light};

	int numOfRuns = atoi(argv[1]); // taking the first argument that's not the filename
		// will deal with error management later

	for(int run = 0; run < numOfRuns; run++){
	pVector curPos(generatePositions());
		auto vel_inf = generateVelocities(50,280);
	pVector curVel(std::get<0>(vel_inf));
	std::cout << "\nTrack number : " << run << std::endl;
	std::cout<<"Positions: ";
	curPos.vprint();
	std::cout<< "Velocities: ";
	curVel.vprint();

	int success = lorentzSimulator(cons, magField, curPos, curVel, std::get<1>(vel_inf),std::to_string(run));
 
	}
	return 1;
}
