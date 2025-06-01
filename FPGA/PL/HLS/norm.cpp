#include "data.hpp"
#include "hls_math.h"
#include <stdio.h>
#include <string.h>

void encoder_m_axi(volatile data_in *input, volatile float *mu0, volatile float *mu1, volatile float *mu2, volatile float *output, int w_num, int h_num) {
	#pragma HLS INTERFACE m_axi port=input depth=30000*10*32*1 offset=slave bundle=first

    #pragma HLS INTERFACE m_axi port=mu0 depth=30000*129 offset=slave bundle=last
    #pragma HLS INTERFACE m_axi port=mu1 depth=30000*129 offset=slave bundle=last
    #pragma HLS INTERFACE m_axi port=mu2 depth=30000*129 offset=slave bundle=last

	#pragma HLS INTERFACE m_axi port=output depth=30000*130 offset=slave bundle=last

    #pragma HLS INTERFACE s_axilite port=w_num
    #pragma HLS INTERFACE s_axilite port=h_num

	#pragma HLS INTERFACE s_axilite port=return

	data_in tile0[2][BAND*SIZE*SIZE/32];   // dim1 = 2 for PIPO buffer 

	data_t0 tile[2][BAND][SIZE][SIZE];

	data_t out1[2][NUM1][16][16];

	data_t out2[2][NUM2][8][8];

	data_t out3[2][NUM3][OUT_SIZE3][OUT_SIZE3];

	int flag[2][1];
	int flag1[2][1];
	int flag2[2][1];
	int flag3[2][1];

	int num = w_num * h_num;

	data_load(input, 0, w_num, h_num, tile0[0]);

	data_load(input, 1, w_num, h_num, tile0[1]);
	pre_process_tile(tile0[0], tile[1], flag[1]);

	data_load(input, 2, w_num, h_num, tile0[0]);
	pre_process_tile(tile0[1], tile[0], flag[0]);
    encoder_layer1(tile[1], flag[1], out1[0], flag1[0]);

	data_load(input, 3, w_num, h_num, tile0[1]);
	pre_process_tile(tile0[0], tile[1], flag[1]);
    encoder_layer1(tile[0], flag[0], out1[1], flag1[1]);
    encoder_layer2(out1[0], flag1[0], out2[1], flag2[1]);

	data_load(input, 4, w_num, h_num, tile0[0]);
	pre_process_tile(tile0[1], tile[0], flag[0]);
    encoder_layer1(tile[1], flag[1], out1[0], flag1[0]);
    encoder_layer2(out1[1], flag1[1], out2[0], flag2[0]);
    encoder_layer3(out2[1], flag2[1], out3[0], flag3[0]);

	process_tiles: for (int num_t = 5; num_t < num ; num_t ++) { // HLS dataflow directive sometimes doesn't functionally work, I choose to do stage pipeline maunally instead 

		if (num_t & 0x01) {

			data_load(input, num_t, w_num, h_num, tile0[1]);
			pre_process_tile(tile0[0], tile[1], flag[1]);
			encoder_layer1(tile[0], flag[0], out1[1], flag1[1]);
		    encoder_layer2(out1[0], flag1[0], out2[1], flag2[1]);
		    encoder_layer3(out2[0], flag2[0], out3[1], flag3[1]);
		    linear(out3[0], num_t, flag3[0], mu0, mu1, mu2, output);

		} else {

			data_load(input, num_t,  w_num, h_num, tile0[0]);
			pre_process_tile(tile0[1], tile[0], flag[0]);
            encoder_layer1(tile[1], flag[1], out1[0], flag1[0]);
            encoder_layer2(out1[1], flag1[1], out2[0], flag2[0]);
            encoder_layer3(out2[1], flag2[1], out3[0], flag3[0]);
            linear(out3[1], num_t, flag3[1], mu0, mu1, mu2, output);

		}

	}

	if (num & 0x01) {

		if (num >= 5) {
			pre_process_tile(tile0[0], tile[1], flag[1]);
			encoder_layer1(tile[0], flag[0], out1[1], flag1[1]);
		    encoder_layer2(out1[0], flag1[0], out2[1], flag2[1]);
		    encoder_layer3(out2[0], flag2[0], out3[1], flag3[1]);
		    linear(out3[0], num, flag3[0], mu0, mu1, mu2, output);
		}

		if (num >= 4) {
            encoder_layer1(tile[1], flag[1], out1[0], flag1[0]);
            encoder_layer2(out1[1], flag1[1], out2[0], flag2[0]);
            encoder_layer3(out2[1], flag2[1], out3[0], flag3[0]);
            linear(out3[1], num+1, flag3[1], mu0, mu1, mu2, output);
		}
		if (num >= 3) {
            encoder_layer2(out1[0], flag1[0], out2[1], flag2[1]);
            encoder_layer3(out2[0], flag2[0], out3[1], flag3[1]);
            linear(out3[0], num+2, flag3[0], mu0, mu1, mu2, output);
		}
		if (num >= 2) {
            encoder_layer3(out2[1], flag2[1], out3[0], flag3[0]);
            linear(out3[1], num+3, flag3[1], mu0, mu1, mu2, output);
		}
            linear(out3[0], num+4, flag3[0], mu0, mu1, mu2, output);

	} else {

		if (num >= 5) {
			pre_process_tile(tile0[1], tile[0], flag[0]);
            encoder_layer1(tile[1], flag[1], out1[0], flag1[0]);
            encoder_layer2(out1[1], flag1[1], out2[0], flag2[0]);
            encoder_layer3(out2[1], flag2[1], out3[0], flag3[0]);
            linear(out3[1], num, flag3[1], mu0, mu1, mu2, output);
		}

		if (num >= 4) {
            encoder_layer1(tile[0], flag[0], out1[1], flag1[1]);
            encoder_layer2(out1[0], flag1[0], out2[1], flag2[1]);
            encoder_layer3(out2[0], flag2[0], out3[1], flag3[1]);
            linear(out3[0], num+1, flag3[0], mu0, mu1, mu2, output);
		}
		if (num >= 3) {
            encoder_layer2(out1[1], flag1[1], out2[0], flag2[0]);
            encoder_layer3(out2[1], flag2[1], out3[0], flag3[0]);
            linear(out3[1], num+2, flag3[1], mu0, mu1, mu2, output);
		}
		if (num >= 2) {
            encoder_layer3(out2[0], flag2[0], out3[1], flag3[1]);
            linear(out3[0], num+3, flag3[0], mu0, mu1, mu2, output);
		}
            linear(out3[1], num+4, flag3[1], mu0, mu1, mu2, output);
        }
}



void data_load(volatile data_in* input, int num, int w_num, int h_num, // Stage 1: Tile Loading
		data_in tile0[BAND*SIZE*SIZE/32]) {
	int w = w_num;
	int h = 32*h_num;

	int add = num/w*32*w + (num-num/w*w)*1;

	channel_row: for (int k = 0; k < BAND; k++) {
        init_row: for (int i = 0; i < SIZE; i++) {
        	int t0 = 1*32*k+1*i;
		    int t1 = k*w*h +i*w + add;

		  	memcpy((data_in*)(tile0+t0),(const data_in*)(input+t1), 1*sizeof(data_in)); // use this for synthesis and implementation
		    // memcpy((data_in*)(tile0+t0),(const data_in*)(input+t0+num*BAND*SIZE), sizeof(data_in));  // use this for C simulation in Vitis HLS

		  }
	}

}

void pre_process_tile(data_in tile0[BAND*SIZE*SIZE/32],  // Stage 2: Normalization
		data_t0 tile[BAND][SIZE][SIZE], int flag[1]) {

	flag[0] = 0;

	channel_row: for (int k = 0; k < BAND; k++) {
		ap_ufixed <28,4> a = 0;
		ap_ufixed <28,4> b = 0;
		if (k == 0) {
			a=6.9;
			b=3.33333333;
		} else if (k == 1) {
			a=6.5;
			b=2.22222222;
		} else if (k == 2) {
			a=6.2;
			b=1.53846154;
		} else if (k == 3) {
			a=6.1;
			b=1.42857143;
		} else if (k == 8) {
			a=6;
			b=1;
		} else if (k == 9) {
			a=6;
			b=1;
		} else {
			a=6.5;
			b=1.33333333;
		}

	init_row0: for (int i = 0; i < SIZE; i++) {
		init_num0: for (int n = 0; n < 32; n++) {  // Extract pixels from input data
			uint16_t num = 0;
			if (n == 0) {
				num =  tile0[1*32*k+1*i].range(15,0);
			} else if (n == 1) {
				num =  tile0[1*32*k+1*i].range(31,16);
			} else if (n == 2) {
				num =  tile0[1*32*k+1*i].range(47,32);
			} else if (n == 3) {
				num =  tile0[1*32*k+1*i].range(63,48);
			} else if (n == 4) {
				num =  tile0[1*32*k+1*i].range(79,64);
			} else if (n == 5) {
				num =  tile0[1*32*k+1*i].range(95,80);
			} else if (n == 6) {
				num =  tile0[1*32*k+1*i].range(111,96);
			} else if (n == 7) {
				num =  tile0[1*32*k+1*i].range(127,112);
			} else if (n == 8) {
				num =  tile0[1*32*k+1*i].range(143,128);
			} else if (n == 9) {
				num =  tile0[1*32*k+1*i].range(159,144);
			} else if (n == 10) {
				num =  tile0[1*32*k+1*i].range(175,160);
			} else if (n == 11) {
				num =  tile0[1*32*k+1*i].range(191,176);
			} else if (n == 12) {
				num =  tile0[1*32*k+1*i].range(207,192);
			} else if (n == 13) {
				num =  tile0[1*32*k+1*i].range(223,208);
			} else if (n == 14) {
				num =  tile0[1*32*k+1*i].range(239,224);
			} else if (n == 15) {
				num =  tile0[1*32*k+1*i].range(255,240);
			} else if (n == 16) {
				num =  tile0[1*32*k+1*i].range(271,256);
			} else if (n == 17) {
				num =  tile0[1*32*k+1*i].range(287,272);
			} else if (n == 18) {
				num =  tile0[1*32*k+1*i].range(303,288);
			} else if (n == 19) {
				num =  tile0[1*32*k+1*i].range(319,304);
			} else if (n == 20) {
				num =  tile0[1*32*k+1*i].range(335,320);
			} else if (n == 21) {
				num =  tile0[1*32*k+1*i].range(351,336);
			} else if (n == 22) {
				num =  tile0[1*32*k+1*i].range(367,352);
			} else if (n == 23) {
				num =  tile0[1*32*k+1*i].range(383,368);
			} else if (n == 24) {
				num =  tile0[1*32*k+1*i].range(399,384);
			} else if (n == 25) {
				num =  tile0[1*32*k+1*i].range(415,400);
			} else if (n == 26) {
				num =  tile0[1*32*k+1*i].range(431,416);
			} else if (n == 27) {
				num =  tile0[1*32*k+1*i].range(447,432);
			} else if (n == 28) {
				num =  tile0[1*32*k+1*i].range(463,448);
			} else if (n == 29) {
				num =  tile0[1*32*k+1*i].range(479,464);
			} else if (n == 30) {
				num =  tile0[1*32*k+1*i].range(495,480);
			} else if (n == 31) {
				num =  tile0[1*32*k+1*i].range(511,496);
			}

			if ((isnan(num)) || (num == 0)) {   // identif if the input pixel is NAN or 0, if so, set the flag
				flag[0] = 1;
			}
			tile[k][i][n] = (log_f[num] - a) * b - (ap_ufixed <28,4>)1;
			}
		}
	}
}

void encoder_layer1(data_t0 tile[BAND][SIZE][SIZE], int flagi[1],   // Stage 3: CONV Layer1 (architecture1)
		data_t out[NUM1][16][16], int flago[1]) {

	flago[0] = flagi[0];        // propagate the NAN/0 flag to the next stage
	encoder_1: for (int index = 0; index < NUM1; index++) {
		tile_row: for (int i = 0; i < OUT_SIZE1; i++) {   // sometimes HLS unrolling directive doesn't make datapath work in parallel, so I have to unroll manually...
			int x0 = (i * 2)-1;
			int x1 = x0 + 1;
			int x2 = x1 + 1;

			data_t val002 = 0;
			data_t val012 = 0;
			data_t val022 = 0;

			data_t val102 = 0;
			data_t val112 = 0;
			data_t val122 = 0;

			data_t val202 = 0;
			data_t val212 = 0;
			data_t val222 = 0;

			data_t val302 = 0;
			data_t val312 = 0;
			data_t val322 = 0;

			data_t val402 = 0;
			data_t val412 = 0;
			data_t val422 = 0;

			data_t val502 = 0;
			data_t val512 = 0;
			data_t val522 = 0;

			data_t val602 = 0;
			data_t val612 = 0;
			data_t val622 = 0;

			data_t val702 = 0;
			data_t val712 = 0;
			data_t val722 = 0;

			data_t val802 = 0;
			data_t val812 = 0;
			data_t val822 = 0;

			data_t val902 = 0;
			data_t val912 = 0;
			data_t val922 = 0;

			tile_col: for (int j = 0; j < OUT_SIZE1; j++) {

				int y1 = j * 2;
				int y2 = y1 + 1;
				data_t sum = 0;

				data_t val000 = val002;
				data_t val010 = val012;
				data_t val020 = val022;
				data_t val001 = 0;
				data_t val100 = val102;
				data_t val110 = val112;
				data_t val120 = val122;
				data_t val101 = 0;
				data_t val200 = val202;
				data_t val210 = val212;
				data_t val220 = val222;
				data_t val201 = 0;
				data_t val300 = val302;
				data_t val310 = val312;
				data_t val320 = val322;
				data_t val301 = 0;
				data_t val400 = val402;
				data_t val410 = val412;
				data_t val420 = val422;
				data_t val401 = 0;
				data_t val500 = val502;
				data_t val510 = val512;
				data_t val520 = val522;
				data_t val501 = 0;
				data_t val600 = val602;
				data_t val610 = val612;
				data_t val620 = val622;
				data_t val601 = 0;
				data_t val700 = val702;
				data_t val710 = val712;
				data_t val720 = val722;
				data_t val701 = 0;
				data_t val800 = val802;
				data_t val810 = val812;
				data_t val820 = val822;
				data_t val801 = 0;
				data_t val900 = val902;
				data_t val910 = val912;
				data_t val920 = val922;
				data_t val901 = 0;

				if (i == 0) {
					val001 = 0;
					val002 = 0;
					val101 = 0;
					val102 = 0;
					val201 = 0;
					val202 = 0;
					val301 = 0;
					val302 = 0;
					val401 = 0;
					val402 = 0;
					val501 = 0;
					val502 = 0;
					val601 = 0;
					val602 = 0;
					val701 = 0;
					val702 = 0;
					val801 = 0;
					val802 = 0;
					val901 = 0;
					val902 = 0;
				} else {
					val001 = tile[0][x0][y1];
					val002 = tile[0][x0][y2];
					val101 = tile[1][x0][y1];
					val102 = tile[1][x0][y2];
					val201 = tile[2][x0][y1];
					val202 = tile[2][x0][y2];
					val301 = tile[3][x0][y1];
					val302 = tile[3][x0][y2];
					val401 = tile[4][x0][y1];
					val402 = tile[4][x0][y2];
					val501 = tile[5][x0][y1];
					val502 = tile[5][x0][y2];
					val601 = tile[6][x0][y1];
					val602 = tile[6][x0][y2];
					val701 = tile[7][x0][y1];
					val702 = tile[7][x0][y2];
					val801 = tile[8][x0][y1];
					val802 = tile[8][x0][y2];
					val901 = tile[9][x0][y1];
					val902 = tile[9][x0][y2];
				}
				data_t val011 = tile[0][x1][y1];
				data_t val021 = tile[0][x2][y1];
				val012 = tile[0][x1][y2];
				val022 = tile[0][x2][y2];
				sum += ((val000 * kernel1[index][0][0][0] + val001 * kernel1[index][0][0][1]) + (val002 * kernel1[index][0][0][2] +  // CONV with one 3x3 kernel
				val010 * kernel1[index][0][1][0])) + ((val011 * kernel1[index][0][1][1] + val012 * kernel1[index][0][1][2]) +
				(val020 * kernel1[index][0][2][0] + val021 * kernel1[index][0][2][1])) + val022 * kernel1[index][0][2][2];

				data_t val111 = tile[1][x1][y1];
				data_t val121 = tile[1][x2][y1];
				val112 = tile[1][x1][y2];
				val122 = tile[1][x2][y2];
				sum += ((val100 * kernel1[index][1][0][0] + val101 * kernel1[index][1][0][1]) + (val102 * kernel1[index][1][0][2] +
				val110 * kernel1[index][1][1][0])) + ((val111 * kernel1[index][1][1][1] + val112 * kernel1[index][1][1][2]) +
				(val120 * kernel1[index][1][2][0] + val121 * kernel1[index][1][2][1])) + val122 * kernel1[index][1][2][2];

				data_t val211 = tile[2][x1][y1];
				data_t val221 = tile[2][x2][y1];
				val212 = tile[2][x1][y2];
				val222 = tile[2][x2][y2];
				sum += ((val200 * kernel1[index][2][0][0] + val201 * kernel1[index][2][0][1]) + (val202 * kernel1[index][2][0][2] +
				val210 * kernel1[index][2][1][0])) + ((val211 * kernel1[index][2][1][1] + val212 * kernel1[index][2][1][2]) +
				(val220 * kernel1[index][2][2][0] + val221 * kernel1[index][2][2][1])) + val222 * kernel1[index][2][2][2];

				data_t val311 = tile[3][x1][y1];
				data_t val321 = tile[3][x2][y1];
				val312 = tile[3][x1][y2];
				val322 = tile[3][x2][y2];
				sum += ((val300 * kernel1[index][3][0][0] + val301 * kernel1[index][3][0][1]) + (val302 * kernel1[index][3][0][2] +
				val310 * kernel1[index][3][1][0])) + ((val311 * kernel1[index][3][1][1] + val312 * kernel1[index][3][1][2]) +
				(val320 * kernel1[index][3][2][0] + val321 * kernel1[index][3][2][1])) + val322 * kernel1[index][3][2][2];

				data_t val411 = tile[4][x1][y1];
				data_t val421 = tile[4][x2][y1];
				val412 = tile[4][x1][y2];
				val422 = tile[4][x2][y2];
				sum += ((val400 * kernel1[index][4][0][0] + val401 * kernel1[index][4][0][1]) + (val402 * kernel1[index][4][0][2] +
				val410 * kernel1[index][4][1][0])) + ((val411 * kernel1[index][4][1][1] + val412 * kernel1[index][4][1][2]) +
				(val420 * kernel1[index][4][2][0] + val421 * kernel1[index][4][2][1])) + val422 * kernel1[index][4][2][2];

				data_t val511 = tile[5][x1][y1];
				data_t val521 = tile[5][x2][y1];
				val512 = tile[5][x1][y2];
				val522 = tile[5][x2][y2];
				sum += ((val500 * kernel1[index][5][0][0] + val501 * kernel1[index][5][0][1]) + (val502 * kernel1[index][5][0][2] +
				val510 * kernel1[index][5][1][0])) + ((val511 * kernel1[index][5][1][1] + val512 * kernel1[index][5][1][2]) +
				(val520 * kernel1[index][5][2][0] + val521 * kernel1[index][5][2][1])) + val522 * kernel1[index][5][2][2];

				data_t val611 = tile[6][x1][y1];
				data_t val621 = tile[6][x2][y1];
				val612 = tile[6][x1][y2];
				val622 = tile[6][x2][y2];
				sum += ((val600 * kernel1[index][6][0][0] + val601 * kernel1[index][6][0][1]) + (val602 * kernel1[index][6][0][2] +
				val610 * kernel1[index][6][1][0])) + ((val611 * kernel1[index][6][1][1] + val612 * kernel1[index][6][1][2]) +
				(val620 * kernel1[index][6][2][0] + val621 * kernel1[index][6][2][1])) + val622 * kernel1[index][6][2][2];

				data_t val711 = tile[7][x1][y1];
				data_t val721 = tile[7][x2][y1];
				val712 = tile[7][x1][y2];
				val722 = tile[7][x2][y2];
				sum += ((val700 * kernel1[index][7][0][0] + val701 * kernel1[index][7][0][1]) + (val702 * kernel1[index][7][0][2] +
				val710 * kernel1[index][7][1][0])) + ((val711 * kernel1[index][7][1][1] + val712 * kernel1[index][7][1][2]) +
				(val720 * kernel1[index][7][2][0] + val721 * kernel1[index][7][2][1])) + val722 * kernel1[index][7][2][2];

				data_t val811 = tile[8][x1][y1];
				data_t val821 = tile[8][x2][y1];
				val812 = tile[8][x1][y2];
				val822 = tile[8][x2][y2];
				sum += ((val800 * kernel1[index][8][0][0] + val801 * kernel1[index][8][0][1]) + (val802 * kernel1[index][8][0][2] +
				val810 * kernel1[index][8][1][0])) + ((val811 * kernel1[index][8][1][1] + val812 * kernel1[index][8][1][2]) +
				(val820 * kernel1[index][8][2][0] + val821 * kernel1[index][8][2][1])) + val822 * kernel1[index][8][2][2];

				data_t val911 = tile[9][x1][y1];
				data_t val921 = tile[9][x2][y1];
				val912 = tile[9][x1][y2];
				val922 = tile[9][x2][y2];
				sum += ((val900 * kernel1[index][9][0][0] + val901 * kernel1[index][9][0][1]) + (val902 * kernel1[index][9][0][2] +
				val910 * kernel1[index][9][1][0])) + ((val911 * kernel1[index][9][1][1] + val912 * kernel1[index][9][1][2]) +
				(val920 * kernel1[index][9][2][0] + val921 * kernel1[index][9][2][1])) + val922 * kernel1[index][9][2][2];

				data_t out1 = (sum + offset[index]) * coef[index] + beta[index];   // Bach Normalization
				if (out1 < 0) out1 *= (data_t)0.01;   // Leaky ReLU
				out[index][i][j] = out1;
				}
			}
		}
	}

void encoder_layer2(data_t tile[NUM1][16][16], int flagi[1],   // Stage 4: CONV Layer2 (architecture2)
		data_t out[NUM2][8][8], int flago[1]) {

	flago[0] = flagi[0];

	encoder_2: for (int index = 0; index < NUM2; index++) {

		data_t inter[IN_CHANNEL2/2][8][8];  // for the dim1, "IN_CHANNEL2/2" because the unroll factor of loop pipeline_dup0 is IN_CHANNEL2/2 
		pipeline_dup0: for (int k = 0; k < IN_CHANNEL2; k++) {   // (sometimes "IN_CHANNEL2/2" causes data violation, if so, change "IN_CHANNEL2/2" back to "IN_CHANNEL2")
			tile_row0: for (int i = 0; i < OUT_SIZE2; i++) {
				data_t val02 = 0;
				data_t val12 = 0;
				data_t val22 = 0;

				int x0 = (i * 2)-1;
				int x1 = x0 + 1;
				int x2 = x1 + 1;
				tile_col0: for (int j = 0; j < OUT_SIZE2; j++) {

					data_t val00 = val02;
					data_t val10 = val12;
					data_t val20 = val22;
					data_t val01 = 0;

					int y1 = j * 2;
					int y2 = y1 + 1;

					if (i == 0) {
						val01 = 0;
						val02 = 0;
					} else {
						val01 = tile[k][x0][y1];
						val02 = tile[k][x0][y2];

					}

					data_t val11 = tile[k][x1][y1];
					data_t val21 = tile[k][x2][y1];

					val12 = tile[k][x1][y2];
					val22 = tile[k][x2][y2];

					data_t sum =  ((val00 * kernel2[index][k][0][0] + val01 * kernel2[index][k][0][1]) + (val02 * kernel2[index][k][0][2] +   // CONV with one 3x3 kernel
						val10 * kernel2[index][k][1][0])) + ((val11 * kernel2[index][k][1][1] + val12 * kernel2[index][k][1][2]) +
						(val20 * kernel2[index][k][2][0] + val21 * kernel2[index][k][2][1])) + val22 * kernel2[index][k][2][2];

					if ( k < IN_CHANNEL2/2) {
						inter[k][i][j] = sum;
					}else {
						inter[k-IN_CHANNEL2/2][i][j] = inter[k-IN_CHANNEL2/2][i][j] + sum;
					}
				}
			}
		}

		addtion_row: for (int i = 0; i < OUT_SIZE2; i++) {
			addition_col: for (int j = 0; j < OUT_SIZE2; j++) {
				data_t sum = 0;
				addition_width: for (int k = 0; k < IN_CHANNEL2/2; k++) {
					sum += inter[k][j][i];
				}

				data_t out2 = (sum + offset[NUM1 + index]) * coef[NUM1 + index] + beta[NUM1 + index];   // Bach Normalization
				if (out2 < 0) out2 *= (data_t)0.01;   // Leaky ReLU
				out[index][j][i] = out2;
			}
		}
	}
}

void encoder_layer3(data_t tile[NUM2][8][8], int flagi[1],     // Stage 5: CONV Layer3 (architecture2)
		data_t out[NUM3][OUT_SIZE3][OUT_SIZE3], int flago[1]) {

	flago[0] = flagi[0];

	encoder_3: for (int index = 0; index < NUM3; index++) {

		data_t inter[IN_CHANNEL3/2][OUT_SIZE3][OUT_SIZE3];
			pipeline_dup: for (int k = 0; k < IN_CHANNEL3; k++) {
				tile_row: for (int i = 0; i < OUT_SIZE3; i++) {
					data_t val02 = 0;
					data_t val12 = 0;
					data_t val22 = 0;

					int x0 = (i * 2)-1;
					int x1 = x0 + 1;
					int x2 = x1 + 1;
					tile_col: for (int j = 0; j < OUT_SIZE3; j++) {

						data_t val00 = val02;
						data_t val10 = val12;
						data_t val20 = val22;
						data_t val01 = 0;

						int y1 = j * 2;
						int y2 = y1 + 1;

						if (i == 0) {
							val01 = 0;
							val02 = 0;
						} else {
							val01 = tile[k][x0][y1];
							val02 = tile[k][x0][y2];

						}

						data_t val11 = tile[k][x1][y1];
						data_t val21 = tile[k][x2][y1];

						val12 = tile[k][x1][y2];
					    val22 = tile[k][x2][y2];

						data_t sum = ((val00 * kernel3[index][k][0][0] + val01 * kernel3[index][k][0][1]) + (val02 * kernel3[index][k][0][2] +
							val10 * kernel3[index][k][1][0])) + ((val11 * kernel3[index][k][1][1] + val12 * kernel3[index][k][1][2]) +
							(val20 * kernel3[index][k][2][0] + val21 * kernel3[index][k][2][1])) + val22 * kernel3[index][k][2][2];

						if (k < IN_CHANNEL3/2) {
						    inter[k][i][j] = sum;
						} else {
							inter[k-IN_CHANNEL3/2][i][j] = inter[k-IN_CHANNEL3/2][i][j]+sum;
						}
					}
				}

			}

		addtion_row: for (int i = 0; i < OUT_SIZE3; i++) {
			addition_col: for (int j = 0; j < OUT_SIZE3; j++) {
				data_t sum = 0;
				addition_width: for (int k = 0; k < IN_CHANNEL3/2; k++) {
					sum += inter[k][i][j];
				}
				data_t out3 = (sum + offset[NUM1 + NUM2 + index]) * coef[NUM1 + NUM2 + index] + beta[NUM1 + NUM2 + index];

				if (out3 < 0) out3 *= (data_t)0.01;
				out[index][i][j] = out3;

			}
		}
	}
}

void linear(data_t out3[NUM3][OUT_SIZE3][OUT_SIZE3],  // Stage 6: FC_Change
		int num_t, int flagi[0], volatile float* mu0, volatile float* mu1, volatile float* mu2, volatile float* mu) {

	int flag = flagi[0];

	float mu_0[MU_SIZE+1];
	float mu_1[MU_SIZE+1];
	float mu_2[MU_SIZE+1];

	int t1 = (num_t-5)*(MU_SIZE+1);

	copy_mu0: for (int i = 0; i < MU_SIZE+1; i++) {
		mu_0[i] = mu0[i+t1];
	}

	copy_mu1: for (int i = 0; i < MU_SIZE+1; i++) {
		mu_1[i] = mu1[i+t1];
	}

	copy_mu2: for (int i = 0; i < MU_SIZE+1; i++) {
		mu_2[i] = mu2[i+t1];
	}

	ap_ufixed <40,10> all = 0;
	ap_fixed <40,11> mul0 = 0;
	ap_fixed <40,11> mul1 = 0;
	ap_fixed <40,11> mul2 = 0;

	int t0 = (num_t-5)*(MU_SIZE+2);

	matrix_row1: for (int t = 0; t < 128; t++) {   // sometimes HLS unrolling directive doesn't make datapath work in parallel, so I have to unroll manually...
		data_t sum000 = 0;
		data_t sum001 = 0;
		data_t sum002 = 0;
		data_t sum003 = 0;

		data_t sum010 = 0;
		data_t sum011 = 0;
		data_t sum012 = 0;
		data_t sum013 = 0;

		data_t sum020 = 0;
		data_t sum021 = 0;
		data_t sum022 = 0;
		data_t sum023 = 0;

		data_t sum030 = 0;
		data_t sum031 = 0;
		data_t sum032 = 0;
		data_t sum033 = 0;

		data_t sum100 = 0;
		data_t sum101 = 0;
		data_t sum102 = 0;
		data_t sum103 = 0;

		data_t sum110 = 0;
		data_t sum111 = 0;
		data_t sum112 = 0;
		data_t sum113 = 0;

		data_t sum120 = 0;
		data_t sum121 = 0;
		data_t sum122 = 0;
		data_t sum123 = 0;

		data_t sum130 = 0;
		data_t sum131 = 0;
		data_t sum132 = 0;
		data_t sum133 = 0;

		int count = 0;  // use this for synthesis and implementation
		channel: for (int i = 0; i < NUM3; i=i+2) {
			sum000 += out3[i][0][0] * mu_weights[t][count];   // one MAC
			sum010 += out3[i][0][1] * mu_weights[t][count+1];
			sum020 += out3[i][0][2] * mu_weights[t][count+2];
			sum030 += out3[i][0][3] * mu_weights[t][count+3];

			sum001 += out3[i][1][0] * mu_weights[t][count+4];
			sum011 += out3[i][1][1] * mu_weights[t][count+5];
			sum021 += out3[i][1][2] * mu_weights[t][count+6];
			sum031 += out3[i][1][3] * mu_weights[t][count+7];

			sum002 += out3[i][2][0] * mu_weights[t][count+8];
			sum012 += out3[i][2][1] * mu_weights[t][count+9];
			sum022 += out3[i][2][2] * mu_weights[t][count+10];
			sum032 += out3[i][2][3] * mu_weights[t][count+11];


			sum003 += out3[i][3][0] * mu_weights[t][count+12];
			sum013 += out3[i][3][1] * mu_weights[t][count+13];
			sum023 += out3[i][3][2] * mu_weights[t][count+14];
			sum033 += out3[i][3][3] * mu_weights[t][count+15];

			sum100 += out3[i+1][0][0] * mu_weights[t][count+16];
			sum110 += out3[i+1][0][1] * mu_weights[t][count+17];
			sum120 += out3[i+1][0][2] * mu_weights[t][count+18];
			sum130 += out3[i+1][0][3] * mu_weights[t][count+19];

			sum101 += out3[i+1][1][0] * mu_weights[t][count+20];
			sum111 += out3[i+1][1][1] * mu_weights[t][count+21];
			sum121 += out3[i+1][1][2] * mu_weights[t][count+22];
			sum131 += out3[i+1][1][3] * mu_weights[t][count+23];

			sum102 += out3[i+1][2][0] * mu_weights[t][count+24];
			sum112 += out3[i+1][2][1] * mu_weights[t][count+25];
			sum122 += out3[i+1][2][2] * mu_weights[t][count+26];
			sum132 += out3[i+1][2][3] * mu_weights[t][count+27];

			sum103 += out3[i+1][3][0] * mu_weights[t][count+28];
			sum113 += out3[i+1][3][1] * mu_weights[t][count+29];
			sum123 += out3[i+1][3][2] * mu_weights[t][count+30];
			sum133 += out3[i+1][3][3] * mu_weights[t][count+31];

			count += 32;
		}

		// int count = 0;   // use this for C simulation in Vitis HLS
		// channel: for (int i = 0; i < NUM3; i=i+2) {     
		// 	sum000 += out3[i][0][0] * (ap_fixed <26,2>)mu_weights[t][count];    
		// 	sum010 += out3[i][0][1] * (ap_fixed <26,2>)mu_weights[t][count+1];
		// 	sum020 += out3[i][0][2] * (ap_fixed <26,2>)mu_weights[t][count+2];
		// 	sum030 += out3[i][0][3] * (ap_fixed <26,2>)mu_weights[t][count+3];

		// 	sum001 += out3[i][1][0] * (ap_fixed <26,2>)mu_weights[t][count+4];
		// 	sum011 += out3[i][1][1] * (ap_fixed <26,2>)mu_weights[t][count+5];
		// 	sum021 += out3[i][1][2] * (ap_fixed <26,2>)mu_weights[t][count+6];
		// 	sum031 += out3[i][1][3] * (ap_fixed <26,2>)mu_weights[t][count+7];

		// 	sum002 += out3[i][2][0] * (ap_fixed <26,2>)mu_weights[t][count+8];
		// 	sum012 += out3[i][2][1] * (ap_fixed <26,2>)mu_weights[t][count+9];
		// 	sum022 += out3[i][2][2] * (ap_fixed <26,2>)mu_weights[t][count+10];
		// 	sum032 += out3[i][2][3] * (ap_fixed <26,2>)mu_weights[t][count+11];


		// 	sum003 += out3[i][3][0] * (ap_fixed <26,2>)mu_weights[t][count+12];
		// 	sum013 += out3[i][3][1] * (ap_fixed <26,2>)mu_weights[t][count+13];
		// 	sum023 += out3[i][3][2] * (ap_fixed <26,2>)mu_weights[t][count+14];
		// 	sum033 += out3[i][3][3] * (ap_fixed <26,2>)mu_weights[t][count+15];

		// 	sum100 += out3[i+1][0][0] * (ap_fixed <26,2>)mu_weights[t][count+16];
		// 	sum110 += out3[i+1][0][1] * (ap_fixed <26,2>)mu_weights[t][count+17];
		// 	sum120 += out3[i+1][0][2] * (ap_fixed <26,2>)mu_weights[t][count+18];
		// 	sum130 += out3[i+1][0][3] * (ap_fixed <26,2>)mu_weights[t][count+19];

		// 	sum101 += out3[i+1][1][0] * (ap_fixed <26,2>)mu_weights[t][count+20];
		// 	sum111 += out3[i+1][1][1] * (ap_fixed <26,2>)mu_weights[t][count+21];
		// 	sum121 += out3[i+1][1][2] * (ap_fixed <26,2>)mu_weights[t][count+22];
		// 	sum131 += out3[i+1][1][3] * (ap_fixed <26,2>)mu_weights[t][count+23];

		// 	sum102 += out3[i+1][2][0] * (ap_fixed <26,2>)mu_weights[t][count+24];
		// 	sum112 += out3[i+1][2][1] * (ap_fixed <26,2>)mu_weights[t][count+25];
		// 	sum122 += out3[i+1][2][2] * (ap_fixed <26,2>)mu_weights[t][count+26];
		// 	sum132 += out3[i+1][2][3] * (ap_fixed <26,2>)mu_weights[t][count+27];

		// 	sum103 += out3[i+1][3][0] * (ap_fixed <26,2>)mu_weights[t][count+28];
		// 	sum113 += out3[i+1][3][1] * (ap_fixed <26,2>)mu_weights[t][count+29];
		// 	sum123 += out3[i+1][3][2] * (ap_fixed <26,2>)mu_weights[t][count+30];
		// 	sum133 += out3[i+1][3][3] * (ap_fixed <26,2>)mu_weights[t][count+31];

		// 	count += 32;
		// }

		data_t sum = sum000 + sum001 + sum002 + sum003 + sum010 + sum011 + sum012 + sum013 + sum020 + sum021 + sum022 + sum023 + sum030 + sum031 + sum032 + sum033 + sum100 + sum101 + sum102 + sum103 + sum110 + sum111 + sum112 + sum113 + sum120 + sum121 + sum122 + sum123 + sum130 + sum131 + sum132 + sum133 + mu_bias[t];

		if (flag == 1) {
	    	mu[t+t0] = NAN;
	    } else {
	        mu[t+t0] = sum;
	    }
	    all += sum * sum;


	    mul0 += sum * (data_t) mu_0[t];
	    mul1 += sum * (data_t) mu_1[t];
	    mul2 += sum * (data_t) mu_2[t];
	}

	ap_ufixed <32,5> sqrt_all = sqrt((float)all);

	if (flag == 1) {
	    mu[128+t0] = NAN;
	} else {
		mu[128+t0] = sqrt_all;
	}

	float ml0 = 1 - mul0/ sqrt_all / (data_t) mu_0[128];  // Caculate one of the cosine distance
	float ml1 = 1 - mul1/ sqrt_all / (data_t) mu_1[128];
	float ml2 = 1 - mul2/ sqrt_all / (data_t) mu_2[128];

	float d = NAN;
	if ((isnan(mu_0[128]) == false)&&(isnan(mu_1[128]) == true)) {  // Solve condition when the historical latent vectors are NAN 
		d = ml0;
	} else if ((isnan(mu_0[128]) == true)&&(isnan(mu_1[128]) == false)) {
		d = ml1;
	} else if ((isnan(mu_0[128]) == false)&&(isnan(mu_1[128]) == false)) {
		if (ml1 < ml0) {         // Compare three distance scores
			d = ml1;
		} else {
			d = ml0;
		}
	}

	if ((isnan(d) == true)&&(isnan(mu_2[128]) == false)) {
		d = ml2;
	} else if ((isnan(d) == false)&&(isnan(mu_2[128]) == false)) {
		if (ml2 < d) {
			d = ml2;
		}
	}

	if (flag == 1) {
		mu[129+t0] = NAN;
	} else {
        mu[129+t0] = d;
	}
}
