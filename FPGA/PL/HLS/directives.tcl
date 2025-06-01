############################################################
## This file is generated automatically by Vitis HLS.
## Please DO NOT edit it.
## Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
############################################################
set_directive_unroll -factor 16 "linear/matrix_row"
set_directive_unroll -factor 16 "linear/add_bias"
set_directive_pipeline "flatten_array/row"
set_directive_top -name encoder_m_axi "encoder_m_axi"
set_directive_pipeline "tile_to_out_layer2/tile_col"
set_directive_pipeline "tile_to_out_layer2/addition_col"
set_directive_pipeline "tile_to_out_layer3/tile_col"
set_directive_pipeline "tile_to_out_layer3/addition_col"
set_directive_pipeline "tile_to_out_layer1/tile_col"
set_directive_pipeline "tile_to_out_layer1/addition_col"
set_directive_unroll -factor 2 "tile_to_out_layer1/pipeline_dup"
set_directive_unroll -factor 2 "tile_to_out_layer2/pipeline_dup"
set_directive_pipeline "linear/channel0"
set_directive_pipeline "linear/channel1"
set_directive_pipeline "linear/channel2"
set_directive_pipeline "linear/channel3"
set_directive_unroll -factor 2 "tile_to_out_layer3/pipeline_dup"
set_directive_array_partition -type cyclic -factor 2 -dim 1 "tile_to_out_layer3" inter
set_directive_unroll -factor 10 "encoder_layer1/pipeline_dup"
set_directive_pipeline "encoder_layer1/addition_col"
set_directive_unroll -factor 4 "encoder_layer2/pipeline_dup"
set_directive_pipeline "encoder_layer2/tile_col"
set_directive_pipeline "encoder_layer2/addition_col"
set_directive_pipeline "encoder_layer3/tile_col"
set_directive_pipeline "encoder_layer3/addition_col"
set_directive_unroll -factor 16 "encoder_layer3/pipeline_dup"
set_directive_array_reshape -type complete -dim 3 "encoder_m_axi" kernel1
set_directive_array_reshape -type complete -dim 3 "encoder_m_axi" kernel3
set_directive_array_partition -type complete -dim 2 "encoder_m_axi" tile
set_directive_array_partition -type cyclic -factor 8 -dim 2 "encoder_m_axi" out1
set_directive_array_partition -type cyclic -factor 2 -dim 3 "encoder_m_axi" out1
set_directive_bind_storage -type ram_2p -impl lutram "encoder_m_axi" out2
set_directive_array_partition -type cyclic -factor 16 -dim 2 "encoder_m_axi" out2
set_directive_array_partition -type cyclic -factor 4 -dim 3 "encoder_m_axi" out2
set_directive_array_partition -type complete -dim 4 "encoder_m_axi" out3
set_directive_array_partition -type complete -dim 3 "encoder_m_axi" out3
set_directive_array_reshape -type cyclic -factor 4 -dim 2 "encoder_m_axi" kernel3
set_directive_array_reshape -type cyclic -factor 5 -dim 2 "encoder_m_axi" kernel1
set_directive_unroll -factor 8 "encoder_layer2/pipeline_dup0"
set_directive_array_partition -type cyclic -factor 8 -dim 1 "encoder_layer2/encoder_2" inter
set_directive_array_partition -type complete -dim 1 "encoder_layer1/encoder_1" inter
set_directive_pipeline -off "linear/matrix_row1"
set_directive_pipeline "linear/channel"
set_directive_pipeline "data_load/init_col"
set_directive_loop_flatten -off "data_load/init_col"
set_directive_array_partition -type cyclic -factor 2 -dim 3 "encoder_m_axi" tile
set_directive_loop_flatten -off "data_load/init_num"
set_directive_pipeline "data_load/init_num"
set_directive_array_reshape -type cyclic -factor 4 -dim 2 "encoder_m_axi" kernel2
set_directive_array_partition -type complete -dim 0 "encoder_m_axi" flag
set_directive_array_partition -type complete -dim 0 "encoder_m_axi" flag1
set_directive_array_partition -type complete -dim 0 "encoder_m_axi" flag2
set_directive_array_partition -type complete -dim 0 "encoder_m_axi" flag3
set_directive_array_partition -type complete -dim 1 "encoder_m_axi" out1
set_directive_array_partition -type complete -dim 1 "encoder_m_axi" out2
set_directive_array_partition -type complete -dim 1 "encoder_m_axi" out3
set_directive_array_partition -type complete -dim 1 "encoder_m_axi" tile
set_directive_bind_storage -type ram_2p -impl lutram "encoder_m_axi" tile
set_directive_bind_storage -type ram_2p -impl lutram "encoder_m_axi" out1
set_directive_bind_storage -type ram_2p -impl lutram "encoder_layer2/encoder_2" inter
set_directive_array_partition -type complete -dim 1 "encoder_m_axi" tile0
set_directive_bind_storage -type ram_2p -impl uram "encoder_m_axi" tile0
set_directive_pipeline "pre_process_tile/init_num0"
set_directive_pipeline "pre_process_tile/init_num1"
set_directive_array_partition -type cyclic -factor 2 -dim 4 "encoder_m_axi" tile
set_directive_unroll -factor 2 "pre_process_tile/init_num0"
set_directive_array_partition -type cyclic -factor 64 -dim 2 "linear" mu_weights
set_directive_array_partition -type complete -dim 0 "encoder_m_axi" offset
set_directive_array_partition -type complete -dim 0 "encoder_m_axi" coef
set_directive_array_partition -type complete -dim 0 "encoder_m_axi" beta
set_directive_bind_storage -type ram_2p -impl uram "encoder_layer1/encoder_1" inter
set_directive_array_reshape -type complete -dim 4 "encoder_m_axi" kernel1
set_directive_array_partition -type cyclic -factor 4 -dim 2 "encoder_m_axi" out3
set_directive_array_partition -type cyclic -factor 2 -dim 4 "encoder_m_axi" out1
set_directive_bind_storage -type ram_2p -impl lutram "encoder_m_axi" out3
set_directive_bind_storage -type rom_2p -impl bram "linear" mu_weights
set_directive_array_reshape -type complete -dim 3 "encoder_m_axi" kernel2
set_directive_array_reshape -type complete -dim 4 "encoder_m_axi" kernel2
set_directive_array_reshape -type complete -dim 4 "encoder_m_axi" kernel3
set_directive_pipeline "encoder_layer2/tile_col0"
set_directive_bind_storage -type rom_2p -impl bram "encoder_m_axi" kernel3
set_directive_unroll -factor 4 "linear/channel"
set_directive_bind_storage -type rom_2p -impl lutram "encoder_m_axi" kernel1
set_directive_bind_storage -type rom_2p -impl lutram "encoder_m_axi" kernel2
set_directive_bind_storage -type ram_2p -impl lutram "encoder_layer3/encoder_3" inter
set_directive_array_partition -type cyclic -factor 16 -dim 1 "encoder_layer3/encoder_3" inter
set_directive_array_partition -type cyclic -factor 4 -dim 2 "encoder_layer2/encoder_2" inter
set_directive_pipeline "encoder_layer2/addition_col1"
set_directive_unroll -factor 2 "encoder_layer2/addition_col1"
set_directive_unroll -factor 4 "encoder_layer2/addition_col"
