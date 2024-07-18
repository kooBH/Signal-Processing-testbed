#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE

#define _CRTDBG_MAP_ALLOC

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <crtdbg.h>

#include "STFT.h"
#include "WAV.h"
#include "RtInput.h"


// WAV_OR_MIC
// 1 : wav file input
// 0 : mic stream input
#define WAV_OR_MIC 1

int main(int argc, char* argv[])
{
	// memory leakage check
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	clock_t startTime, endTime;

	// Fixed parameters
	constexpr int sr = 16000;
	constexpr int n_fft = 512;
	constexpr int n_hop = 128;
	constexpr int in_channels = 4;
	constexpr int out_channels = 1;

	/* IO */
	// mic input [in_channels * n_hop]
	short* buf_in = new short[in_channels * n_hop];
	// processed output [out_channels * n_hop]
	short* buf_out = new short[out_channels * n_hop];

	// if in_channels == out_channles, we can use same STFT object
	STFT process(in_channels, n_fft, n_hop);
	STFT process(out_channels, n_fft, n_hop);

	double** data;
	data = new double* [in_channels];
	for (int i = 0; i < in_channels; i++) {
		data[i] = new double[n_fft + 2];
		memset(data[i], 0, sizeof(double) * (n_fft + 2));
	}


	startTime = clock();

	/********   IO   ******/
#if WAV_OR_MIC
	// WAV IO
	WAV input;

	input.OpenFile("input.wav");
#else

	// Find Device by Name
	const char* deviceName = "mpWAV";
	int device = 0;

	RtAudio::DeviceInfo info;
	RtAudio adc;
	int	nDevice = adc.getDeviceCount();
	std::cout << "api : " << adc.getCurrentApi() << "\n";
	if (nDevice < 1) {
		std::cout << "\nNo audio devices found!\n";
		exit(-1);
		return 0;
	}
	else {
		std::cout << "Total Device : " << nDevice << "\n\n";
		for (int i = 0; i < nDevice; i++) {
			info = adc.getDeviceInfo(i);
			if (info.name.find(deviceName) != std::string::npos) {
				if (info.inputChannels >= 1) {
					std::cout << "device = " << i << "\n";
					std::cout << "name = " << info.name << "\n";
					std::cout << "maximum input channels = " << info.inputChannels << "\n";
					std::cout << "maximum output channels = " << info.outputChannels << "\n";
					std::cout << "Samplerates : ";
					for (auto sr : info.sampleRates)
						std::cout << sr << " ";
					std::cout << "\n";
					std::cout << "----------------------------------------------------------" << "\n";
					device = i;
					break;
				}
			}
		}
	}
	RtInput input(device, raw_channels, sr, n_hop, n_fft, n_hop);

	// Save inputs for debugging
	WAV raw(raw_channels, sr);
	raw.NewFile("raw.wav");

#endif

	WAV output(out_channels, sr);
	output.NewFile("output.wav");

#if WAV_OR_MIC
	while (!input.IsEOF()) {
		input.ReadUnit(buf_in, n_hop * in_channels);
#else

	input.Start();
	while (input.IsRunning()) {

		if (input.data.stock.load() >= n_hop * raw_channels)
			input.GetBuffer(buf_in);
		else
			continue;

#endif
		//----- processing ------------//
		process.stft(buf_in, n_hop*in_channels, data);

		// Run algorithm here.

		process.istft(data, buf_out);
		output.Append(buf_out, out_channels*n_hop);


#if !WAV_OR_MIC
		raw.Append(buf_in, raw_channels * n_hop);
#endif
	}

	endTime = clock();

#if WAV_OR_MIC
	input.Finish();

#else
	input.Stop();

#endif
	output.Finish();

	printf("\n\nComplete. Execution Time is %.1f sec\n", (float)(endTime - startTime) / 1000);

	for (int i = 0; i < in_channels; i++)
		delete[] data[i];
	delete[] data;
	delete[] buf_in;
	delete[] buf_out;


	return 0;
	}
