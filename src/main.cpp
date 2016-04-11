#include "main.hpp"
using namespace hls;

void merge1(stream<uint32_t> &channel0,
		stream<uint32_t> &channel1,
		stream<uint32_t> &out,
		int sortLength
		){
	#pragma HLS PIPELINE II=0 enable_flush
	#pragma HLS LATENCY max=0
	static uint32_t value0;
	static uint32_t value1;
	static uint32_t cnt0;
	static uint32_t cnt1;
	static enum sortState { SORT_INIT=0, SORT_READ_CH0, SORT_READ_CH1, SORT_COMPARE, FLUSH_READ_CH0, FLUSH_READ_CH1, FLUSH_CH1, FLUSH_CH0} mState;

	switch (mState){
		case SORT_INIT:{
			if(!channel0.empty() && !channel1.empty()){
				channel0.read(value0);
				channel1.read(value1);
				cnt0=1;
				cnt1=1;
				mState = SORT_COMPARE;
			}
			break;
		}
		case SORT_READ_CH0:{
			if(channel0.read_nb(value0)){
				mState = SORT_COMPARE;
				cnt0++;
			}
			break;
		}
		case SORT_READ_CH1:{
			if(channel1.read_nb(value1)){
				mState = SORT_COMPARE;
				cnt1++;
			}
			break;
		}
		case SORT_COMPARE:{
			if(value0 < value1){
				if(out.write_nb(value0)){
					if(cnt0<sortLength){
						mState = SORT_READ_CH0;
					}else{
						mState = FLUSH_CH1;
					}
				}
			}else{
				if(out.write_nb(value1)){
					if(cnt1<sortLength){
						mState = SORT_READ_CH1;
					}else{
						mState = FLUSH_CH0;
					}
				}
			}
			break;
		}
		case FLUSH_READ_CH0:{
			if(channel0.read_nb(value0)){
				mState = FLUSH_CH0;
			}
			break;
		}
		case FLUSH_READ_CH1:{
			if(channel1.read_nb(value1)){
				mState = FLUSH_CH1;
			}
			break;
		}
		case FLUSH_CH0:{
			if(out.write_nb(value0)){
				if(cnt0 < sortLength){
					mState = FLUSH_READ_CH0;
				}
				else{
					mState = SORT_INIT;
				}
				cnt0++;
			}
			break;
		}
		case FLUSH_CH1:{
			if(out.write_nb(value1)){
				if(cnt1 < sortLength){
					mState = FLUSH_READ_CH1;
				}
				else{
					mState = SORT_INIT;
				}
				cnt1++;
			}
			break;
		}
	}
}
void ip_mergesorter_32bit(
		stream<uint32_t> &in,
		stream<uint32_t> &out
	){
	#pragma HLS PIPELINE II=0 enable_flush
	#pragma HLS LATENCY max=0
	#pragma HLS INTERFACE ap_fifo depth=100 port=in
	#pragma HLS INTERFACE ap_fifo depth=100 port=out
	#pragma AP interface ap_ctrl_none port=return
	static uint32_t sortLength;
	static uint32_t value;
	static enum readState{READ_INIT=0, READ_CHANNEL0, WRITE_CHANNEL0, READ_CHANNEL1, WRITE_CHANNEL1} inState;
	static stream<uint32_t> channel0;
	static stream<uint32_t> channel1;
	#pragma HLS STREAM variable=channel0 depth=1000
	#pragma HLS STREAM variable=channel1 depth=1000

	switch(inState) {
	case READ_INIT : {
		if(in.read_nb(value)){
			sortLength = value;
			inState = READ_CHANNEL0;
		}
		break;
	}
	case READ_CHANNEL0 : {
		if(in.read_nb(value)){
			inState = WRITE_CHANNEL0;
		}
		break;
	}
	case WRITE_CHANNEL0 : {
		if(value == 0xFFFFFFFF){
			inState = READ_INIT;
		}else{
			if(channel0.write_nb(value)){
				inState = READ_CHANNEL1;
			}
		}
		break;
	}
	case READ_CHANNEL1 : {
		if(in.read_nb(value)){
			inState = WRITE_CHANNEL1;
		}
		break;
	}
	case WRITE_CHANNEL1 : {
		if(value == 0xFFFFFFFF){
			inState = READ_INIT;
		}else{
			if(channel1.write_nb(value)){
				inState = READ_CHANNEL0;
			}
		}
		break;
	}
}

	merge1(channel0, channel1, out, sortLength);
}
