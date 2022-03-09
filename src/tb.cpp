#include "upsizer.hpp"
#include <iostream>
#include <bitset>
#define MAX_SAMP 512+3

using namespace std;


bool drive_upconvert() {

	hls::stream<out256_t> instream;
	hls::stream<out512_t> outstream, expected;
	out512_t tmpout;
	out256_t tmp;
	bool fail;

	int i=0;

	//variant1
	tmp.data=++i;
	tmp.last=1;
	instream.write(tmp); //expect nothing
	tmp.data=++i;
	tmp.last=0;
	tmpout.data.range(255,0)=i;
	instream.write(tmp);
	tmp.data=++i;
	tmp.last=0;
	tmpout.data.range(511,256)=i;
	tmpout.last=0;
	expected.write(tmpout);
	instream.write(tmp); //expect 2,3 from this
	tmp.data=++i;
	tmp.last=1;
	instream.write(tmp); //expect nothing as cache was just reset
	tmp.data=++i;
	tmp.last=1;
	instream.write(tmp); //expect nothing as cache was held in reset
	tmp.data=++i;
	tmp.last=0;
	tmpout.data.range(255,0)=i;
	instream.write(tmp); //expect nothing as cache was held in reset
	tmp.data=++i;
	tmp.last=1;
	tmpout.data.range(511,256)=i;
	tmpout.last=1;
	expected.write(tmpout);
	instream.write(tmp); //expect something with a last


	while (!instream.empty()) capture_upsizer(instream, outstream);

	cout<<"Expected "<<expected.size()<<" samples, got "<<outstream.size()<<endl;
	fail|=expected.size()!=outstream.size();
	i=0;
	while(outstream.size()>0 && expected.size()>0) {
		out512_t e,g;
		uint256_t e0,e1,g0,g1;
		e=expected.read();
		g=outstream.read();
		e0=e.data.range(255,0);
		e1=e.data.range(511,256);
		g0=g.data.range(255,0);
		g1=g.data.range(511,256);
		cout<<i<<": expected "<<e0<<", "<<e1<<" last="<<e.last<<" got: "<<g0<<", "<<g1<<" last="<<g.last<<endl;
		fail|=e.data!=g.data || e.last!=g.last;
		i++;
	}

	return fail;

}

int main (void){

	bool fail=false;

	fail|=drive_upconvert();

	if (fail) {
		std::cout << "Test failed" << std::endl;
	} else {
		std::cout << "Test passed" << std::endl;
	}
	return(fail);
}
