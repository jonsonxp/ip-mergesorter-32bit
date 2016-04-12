{
  "name": "ip-mergesorter-32bit",
  "type": "ip",
  "version": "0.0.1",
  "summary": "A mergesorter for 32bit integer numbers.",
  "description": "A merge sorter IP with 32-bit width ap_fifo interface implemented in Vivado HLS.",
  "homepage": "https://github.com/jonsonxp/ip-mergesorter-32bit/",
  "license": "MIT",
  "authors": {
    "jonsonxp": "ofmsmile@msn.com"
  },
  "source": {
    "git": "https://github.com/jonsonxp/ip-mergesorter-32bit.git",
    "tag": "0.0.1"
  },
  "code": {
    "verilog": true, 
    "verilog_generator": false,
    "vhdl": false,
    "vhdl_generator": false,
    "vivado_hls": true,
    "hls_generator": true
  },
  "ides": {
    "version": "vivado2015.3",
    "version": "vivado2015.4"
  },
  "platforms": {
    "vc707": {
      "shell": "shell-vc707-xillybus-ap_fifo32",
      "size": 16,
      "reference": " ip_mergesorter_32bit ip_mergesorter_32bit_0 (.ap_clk(ip_clk), .ap_rst(~ip_rst_n), .in_V_dout(in_r_dout), .in_V_empty_n(in_r_empty_n), .in_V_read(in_r_read), .out_V_din(out_r_din), .out_V_full_n(!out_r_full), .out_V_write(out_r_write));",
      "clk_period": 5
    },
    "zybo": {
      "shell": "shell-zybo-xillybus-ap_fifo32",
      "size": 32,
      "reference": " ip_mergesorter_32bit ip_mergesorter_32bit_0 (.ap_clk(ip_clk), .ap_rst(~ip_rst_n), .in_V_dout(in_r_dout), .in_V_empty_n(in_r_empty_n), .in_V_read(in_r_read), .out_V_din(out_r_din), .out_V_full_n(!out_r_full), .out_V_write(out_r_write));",
      "clk_period": 5
    }
  },
  "interfaces": {
    "host-fpga": {
        "name": "xillybus",
        "interface": {
            "protocol" : "ap-stream",
            "datawidth" : "32"
        }
    }
  }
}

