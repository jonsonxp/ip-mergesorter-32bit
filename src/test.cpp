#include "main.hpp"
#include <stdio.h>
#include <stdint.h>
#define N 128

using namespace hls;
using namespace std;

int main(){
	stream<uint32_t> in;
	stream<uint32_t> out;
	uint32_t data[N];
	uint32_t i;
	uint32_t v;
	int numO=0;
	int pass=0;

	//send sortlength to IP
	in.write(1);
	ip_mergesorter_32bit(in, out);


	//init data
	for(i = 0 ; i < N ; i++){
		data[i] = N-i;
	}
	i=0;

	//send data to IP, and get results from IP
	while(numO < N)
	{
		if(i<N){
			in.write(data[i]);
			i++;
		}
		ip_mergesorter_32bit(in, out);
		if(!out.empty()){
			out.read(v);
			std::cout << v << std::endl;
			numO++;
		}
	}

	//send reset signal to IP
	in.write(0xFFFFFFFF);
	ip_mergesorter_32bit(in, out);
}
