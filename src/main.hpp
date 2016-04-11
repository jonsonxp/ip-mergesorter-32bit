#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <hls_stream.h>

using namespace hls;

void ip_mergesorter_32bit(
		stream<uint32_t> &inData,
		stream<uint32_t> &outData);
