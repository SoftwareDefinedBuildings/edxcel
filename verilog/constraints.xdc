
create_clock -period 50.00000000000000000 -name mod_clk -waveform {0.00000000000000000 25.00000000000000000} [get_nets epu_clk]
create_clock -period 10.00000000000000000 -name axi_clk -waveform {0.00000000000000000 5.00000000000000000} [get_ports s00_axi_aclk]
