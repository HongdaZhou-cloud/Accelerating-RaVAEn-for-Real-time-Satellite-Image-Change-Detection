#include <stdio.h>
#include "data.hpp"

int main() {

  int times = 30;

  ap_uint <512> in[times*(BAND*SIZE)];
  float out[times*(MU_SIZE+2)];

  float mu0[times*(MU_SIZE+1)];
  float mu1[times*(MU_SIZE+1)];
  float mu2[times*(MU_SIZE+1)];

  for (int n = 0; n < times; n++) {
  for (int i = 0; i < 129; i++) {
  		mu0[n*129+i] = NAN;
  		mu1[n*129+i] = NAN;
  		mu2[n*129+i] = 1;
  	}
  }

  int t = 0;

  for (int n = 0; n < times; n++) {

  float in_f;
  FILE *fp_tile = fopen("output.dat", "r");

	// load tile

	for (int layer_num = 0; layer_num < BAND; layer_num++) {
		for (int i = 0; i < 32; i++) {
			for (int j = 0; j < 32; j++) {
				fscanf(fp_tile, "%f,", &in_f);
				if (n % 2 == 0) {
				    in[t].range(16*(j+1)-1,16*j) = (ap_uint <16>)in_f;
				} else {
					in[t].range(16*(j+1)-1,16*j) = NAN;
				}
			}
			t = t+1;
		}
	}
	fclose(fp_tile);

  }

	// top function
	encoder_m_axi(in, mu0, mu1, mu2, out, times, 1);

	// Compare outputs
	int t1 = 0;

	for (int n = 0; n < times; n++) {
	float gold;
	float accum_error = 0;
	FILE *fp_gold = fopen("out.gold.dat", "r");
	for (int i = 0; i < 130; i++) {
		fscanf(fp_gold, "%f,", &gold);
		float error = abs((out[n*130+i] - gold)/gold) * 100;
		printf("out: %f, gold: %f, error: %f\n", out[n*130+i], gold, error);
		if (i<128) {
		    accum_error += abs(error);
		}
	}
	printf("Accum_error: %f\n", accum_error);

	fclose(fp_gold);
	}

    return 0;
}



  


