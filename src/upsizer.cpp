#include "upsizer.hpp"
#ifndef __SYNTHESIS__
#include <bitset>
#include <iostream>
using namespace std;
#endif
#include <assert.h>


void capture_upsizer(hls::stream<out256_t> &instream, hls::stream<out512_t> &outstream) {
#pragma HLS PIPELINE II=1
#pragma HLS INTERFACE mode=axis port=instream register_mode=both register
#pragma HLS INTERFACE mode=axis port=outstream register_mode=both register
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
	static uint256_t cache;
	static bool cached=false;
	out256_t in;

	out512_t out, tmp;

	//Read on the interface. Value might be an even or an odd beat (e.g. if we've switched streams)
	in=instream.read();

	//Assume the value is an even value and so it goes in the high bits and we want it's tlast
	out.data.range(255,0)=cache;
	out.data.range(511,256)=in.data;
	out.last=in.last;
	//If the cache was valid we assume the input was an even beat and so we have a full value, output it.
	if (cached) outstream.write(out);

	//Cache this data, if we are even we will clobber it next beat, of we are odd we need it
	cache=in.data;

	//If this was an even (cached=true) cycle then next cycle will be odd and vice versa
	cached=!cached;

	//If we got a tlast then this was an even beat and the next beat will be odd
	cached&=!in.last; //reset cache on tlast

}


void capture_upsizer_frp(hls::stream<out256_t> &instream, hls::stream<out512_t> &outstream) {
#pragma HLS PIPELINE II=1 style=frp
#pragma HLS INTERFACE mode=axis port=instream register_mode=both register
#pragma HLS INTERFACE mode=axis port=outstream register_mode=both register
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
	static uint256_t cache;
	static bool cached=false;
	out256_t in;

	out512_t out, tmp;

	//Read on the interface. Value might be an even or an odd beat (e.g. if we've switched streams)
	in=instream.read();

	//Assume the value is an even value and so it goes in the high bits and we want it's tlast
	out.data.range(255,0)=cache;
	out.data.range(511,256)=in.data;
	out.last=in.last;
	//If the cache was valid we assume the input was an even beat and so we have a full value, output it.
	if (cached) outstream.write(out);

	//Cache this data, if we are even we will clobber it next beat, of we are odd we need it
	cache=in.data;

	//If this was an even (cached=true) cycle then next cycle will be odd and vice versa
	cached=!cached;

	//If we got a tlast then this was an even beat and the next beat will be odd
	cached&=!in.last; //reset cache on tlast

}


void f1(hls::stream<out256_t> &instream, hls::stream<axis256_tmp_t> &outstream ){
#pragma HLS PIPELINE II=1 style=frp
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
	static uint256_t cache;
	static bool cached=false;
	out256_t in;
	axis256_tmp_t tmp;
	//Read on the interface. Value might be an even or an odd beat (e.g. if we've switched streams)
	in=instream.read();

	//Assume the value is an even value and so it goes in the high bits and we want it's tlast
	tmp.data.range(255,0)=cache;
	tmp.data.range(511,256)=in.data;
	tmp.last=in.last;
	//If the cache was valid we assume the input was an even beat and so we have a full value, output it.
	if (cached)
		outstream.write_nb(tmp);
	//Cache this data, if we are even we will clobber it next beat, of we are odd we need it
	cache=in.data;

	//If this was an even (cached=true) cycle then next cycle will be odd and vice versa
	cached=!cached;

	//If we got a tlast then this was an even beat and the next beat will be odd
	cached&=!in.last; //reset cache on tlast
}

void f2(hls::stream<axis256_tmp_t> &out_cache, hls::stream<out512_t> &outstream) {
#pragma HLS PIPELINE II=1 style=frp
#pragma HLS INTERFACE mode=ap_ctrl_none port=return
	out512_t out;
	axis256_tmp_t tmp;
	tmp=out_cache.read();
	out.data=tmp.data;
	out.last=tmp.last;
	outstream.write(out);
}

//NB This design has not been tested
void capture_upsizer_dataflow(hls::stream<out256_t> &instream, hls::stream<out512_t> &outstream) {
#pragma HLS DATAFLOW
#pragma HLS INTERFACE mode=axis port=instream register_mode=both register
#pragma HLS INTERFACE mode=axis port=outstream register_mode=both register
#pragma HLS INTERFACE mode=ap_ctrl_none port=return

	hls::stream<axis256_tmp_t> out_cache;
#pragma HLS STREAM variable=out_cache depth=2

	f1(instream, out_cache);
	f2(out_cache, outstream);
}
