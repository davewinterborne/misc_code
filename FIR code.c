#define N_FIR_I 11				// Filter order + 1

// Globals 
float ic1_raw;					// Current value
float ic1_buffer[N_FIR_I];		// Buffer to hold past values
float fir_10[11];				// Filter coefficients
uint16_t i_start1;				// Counter pointing to most recent value in buffer
float ic1;						// Variable to hold filtered result

// Prototype
float fir_cla(float sample, float* buffer, float* filter, uint16_t length, uint16_t start);

void main(){
	// Define the filter coefficients (only needs to be done once -
	// can be done at declaration in the CPU, but not the CLA)
    // 10th order low-pass FIR filter with cut-off at fs/10
    fir_10[0] = 0.0000;
    fir_10[1] = 0.0093;
    fir_10[2] = 0.0476;
    fir_10[3] = 0.1224;
    fir_10[4] = 0.2022;
    fir_10[5] = 0.2370;
    fir_10[6] = 0.2022;
    fir_10[7] = 0.1224;
    fir_10[8] = 0.0476;
    fir_10[9] = 0.0093;
    fir_10[10] = 0.0000;
	

	ic1 = fir_cla(ic1_raw, &ic1_buffer, &fir_10, N_FIR_I, i_start1);
    i_start1++;
    if (i_start1 >= N_FIR_I) i_start1 -= N_FIR_I;

}

float fir_cla(float sample, float* buffer, float* filter, uint16_t length, uint16_t start){

    int16_t i, j;
    float result = 0;

    buffer[start] = sample;
    for(i=0; i < length; i++){
        j = i+start;
        if (j >= length) j -= length;
        result += buffer[j]*filter[i];
    }

    return result;
}