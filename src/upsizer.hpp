#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

typedef ap_uint<512> uint512_t;
typedef ap_uint<256> uint256_t;

typedef struct axis256_tmp_t {
	uint256_t data;
	bool last;
} axis256_tmp_t;

typedef ap_axiu<256,0,0,0> out256_t;
typedef ap_axiu<512,0,0,0> out512_t;

void capture_upsizer(hls::stream<out256_t> &instream, hls::stream<out512_t> &outstream);
