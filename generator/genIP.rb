
if ARGV[0]
	@loopsize = ARGV[0].to_i
else
	puts "Wrong parameters, please follow this format: "
	puts "ruby genIP size [fifo depth]"
	exit
end

if ARGV[1]
	@fifosize = ARGV[1].to_i
else
	@fifosize = 1000
end

ip_name = "ip_mergesorter_32bit"
string_merge = <<eos
void merge_NUMBER(stream<uint32_t> &channel0,
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
eos

puts "#include \"main.hpp\""
puts "using namespace hls;"
puts

1.upto(@loopsize-1){|i|
	puts string_merge.gsub('_NUMBER', "#{i}")
}

string_ip_wrapper = <<eos
void #{ip_name}(
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
eos

puts string_ip_wrapper

print "\tstatic enum readState{READ_INIT=0, "
out = ""
0.upto((@loopsize-1)){ |i|
	out += "READ_CHANNEL#{i}, WRITE_CHANNEL#{i}, "
}
out = out[0, out.length - 2]
out += "} inState;\n"
print out

0.upto((@loopsize-1)*2-1){ |i|
	puts "\tstatic stream<uint32_t> channel#{i};"
}

target = @loopsize
operator = 1
0.upto((@loopsize-1)*2-1){ |i|
	if(operator == 1)
		puts "\t#pragma HLS STREAM variable=channel#{i} depth=#{@fifosize*operator}"
	else
		puts "\t#pragma HLS STREAM variable=channel#{i} depth=10"
	end
	if(i == target - 1)
		operator *= 2
		target = target + @loopsize / operator
	end
}
puts

string_switch_inState_init =<<eos
	switch(inState) {
	case READ_INIT : {
		if(in.read_nb(value)){
			sortLength = value;
			inState = READ_CHANNEL0;
		}
		break;
	}
eos
puts string_switch_inState_init

string_switch_inState_readwrite =<<eos
	case READ_CHANNEL_NUMBER0 : {
		if(in.read_nb(value)){
			inState = WRITE_CHANNEL_NUMBER0;
		}
		break;
	}
	case WRITE_CHANNEL_NUMBER0 : {
		if(value == 0xFFFFFFFF){
			inState = READ_INIT;
		}else{
			if(channel_NUMBER0.write_nb(value)){
				inState = READ_CHANNEL_NUMBER1;
			}
		}
		break;
	}
eos

0.upto((@loopsize-1)){ |i|
	tmpString = string_switch_inState_readwrite.clone
	tmpString.gsub!('_NUMBER0', "#{i}")
	if(i == @loopsize-1)
		tmpString.gsub!('_NUMBER1', "0")
	else
		tmpString.gsub!('_NUMBER1', "#{i+1}")
	end
	puts tmpString
}
puts "}"
puts 

cnt_ch_0 = 0
cnt_ch_1 = 0
target = @loopsize
operator = 1
length = ""
1.upto(@loopsize-1){|i|
	from0 = cnt_ch_0
	cnt_ch_0 += 1
	from1 = cnt_ch_0
	cnt_ch_0 += 1
	to = @loopsize + cnt_ch_1
	cnt_ch_1 += 1
	
	if(from0 == target)
		operator *= 2
		length = " * #{operator}"
		target = target + @loopsize / operator
	end

	if(to == (@loopsize-1)*2)
		puts "\tmerge#{i}(channel#{from0}, channel#{from1}, out, sortLength#{length});"
	else
		puts "\tmerge#{i}(channel#{from0}, channel#{from1}, channel#{to}, sortLength#{length});"
	end
}

puts "}"
