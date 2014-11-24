
create_clock -period 10.000 -name mod_clk -waveform {0.000 5.000} [get_nets epu_clk]
create_clock -period 10.000 -name axi_clk -waveform {0.000 5.000} [get_ports s00_axi_aclk]

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h0_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 5
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h0_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 4 -hold

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h1_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 4 
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h1_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 3 -hold

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h2_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 4 
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h2_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 3 -hold

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h3_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 4 
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h3_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 3 -hold

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h4_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 4 
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h4_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 3 -hold

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h5_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 3 
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h5_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 2 -hold

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h6_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 3 
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h6_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 2 -hold

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h7_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 3 
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h7_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 2 -hold

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h8_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 3 
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h8_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 2 -hold

set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h9_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 3 
set_multicycle_path -from [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/h9_reg[*]/C}] -to [get_pins {EdXel_v1_S00_AXI_inst/genblk1[*].U/ML/rres_reg[*]/D}] 2 -hold


