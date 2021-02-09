#include <iostream>
#include <cmath>
#include <vector>

class pVector{
    /* List of functions and stuff for quick reference
     * v.elements       - this is a vector holding all the data
     * v.vprint         - this prints the elements as a column vector
     * add(v1,v2)       - adds the two argument vectors
     * scale(alpha, v)  - scales vector v by alpha
     * subtract(v1,v2)  - v1 - v2
     * cross(v1, v2)    - cross product between v1 and v2 (only works if dim v1 = 3)
     * dot(v1, v2)      - inner product between v1 and v2
     * norm(v)          - gives the norm (or magnitude) of a vector
     */
	public:
		std::vector<double> elements;
		pVector(int size){
			for(int i = 0; i < size; i++){
				elements.push_back(0);
			}
		}
		pVector(std::vector<double> data){
			elements = data;
		}
		void vprint(){
			// function that prints the vector
			for(int i = 0; i < elements.size(); i++){
				std::cout << elements[i];
				if(i==elements.size()-1){
					std::cout << std::endl;
				}
				else{
					std::cout << ", ";
				}
			}
		}

		// all operations are, for now, static functions

		static pVector add(pVector v1, pVector v2){
			if(v1.elements.size() != v1.elements.size()){
				// if the sizes of the vectors don't match, we reject it outright
				std::cerr<<"Vector sizes do not match."<<std::endl;
			}
			// if the sizes _do_ match, then
			int vector_length = v1.elements.size();
			pVector res(0);
			for(int i = 0; i < vector_length; i++){
				res.elements.push_back(v1.elements[i] + v2.elements[i]);
			}
			// then element-wise addition of vectors

			return res;
			// and we return the resultant vector
		}

		static pVector scale(double alpha, pVector v){
			int vector_length = v.elements.size();
			pVector res(0);
			for(int i = 0; i < vector_length; i++){
				res.elements.push_back(v.elements[i]*alpha);
			}
			// multiplying each element with the double scalar
			return res;
			// and we return the resultant vector
		}

		static pVector subtract(pVector v1, pVector v2){
			pVector a = v1;
			pVector b = pVector::scale(-1.0,v2);
			return pVector::add(a,b);
		}

		// cross product function
		static pVector cross(pVector v1, pVector v2){
			// this only works if the size of the array is 3
			if(v1.elements.size() == v2.elements.size() && v1.elements.size()==3){
				// if the sizes match and if the size is 3, then we can get down to business

				// calculating each component of the resultant vector individually
				// v[0] -> x, v[1] -> y, v[2] -> z
				double xterm = v1.elements[1]*v2.elements[2] - v2.elements[1]*v1.elements[2];
				double yterm = -(v1.elements[0]*v2.elements[2] - v1.elements[2]*v2.elements[0]);
				double zterm = v1.elements[0]*v2.elements[1] - v1.elements[1]*v2.elements[0];

				// creating a new pVector object using the calculated components and returning that
				pVector cross_product({xterm,yterm,zterm});
				return cross_product;

			}
			else{
				//otherwise, we are going to have problems
				std::cerr<<"Cross product cannot be performed on the given vectors."<<std::endl;
				return 1;
			}
		}

		// dot product function

		static double dot(pVector v1, pVector v2){
			if(v1.elements.size() != v1.elements.size()){
				// if the sizes of the vectors don't match, we reject it outright
				std::cerr<<"Vector sizes do not match."<<std::endl;
			}
			// if the sizes _do_ match, then
			int vector_length = v1.elements.size();
			double sum = 0.0;
			for(int i = 0; i < vector_length; i++){
				sum += (v1.elements[i] * v2.elements[i]);
			}
			// then element-wise addition of vectors

			return sum;
			// and we return the resultant sum
		}

		// now that we have defined a dot (inner) product, we can define the norm

		static double norm(pVector v1){
			return std::sqrt(pVector::dot(v1,v1));
		}

};

void vectorTest(pVector v1, pVector v2, pVector (*oper)(pVector a, pVector b), char codename){
	// this function tests if my function boi is working
	pVector res = oper(v1,v2);
	std::cout<<"Testing operation : " << codename << std::endl;
	res.vprint();
}

