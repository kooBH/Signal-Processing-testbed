#include "STFT.h"
#include "WAV.h"

#include <time.h>
#include <string>
#include <filesystem>
/* Inlcude Algorithm source here */

/* Set Parameter of Input */
constexpr int n_channel = 6;
constexpr int sr = 16000;
constexpr int n_fft = 512;
constexpr int n_hop = 128;

int main() {
 clock_t start, end;
	double result;
  
  /* Define Algorithm Class here */
  int length;
  WAV input;
  WAV output(n_channel, sr);
  STFT process(n_channel, n_fft, n_hop);
  
  short buf_in[n_channel * n_hop]
  short buf_out[n_channel * n_hop];
  
  double** data;
  data = new double* [n_channel];
  for (int i = 0; i < n_channel; i++) {
    data[i] = new double[n_fft + 2];
    memset(data[i], 0, sizeof(double) * (n_fft + 2));
  }

  start = clock();
  for (auto path : std::filesystem::directory_iterator{"../input"}) {
    std::string i(path.path().string());
    printf("processing : %s\n",i.c_str());
    input.OpenFile(i.c_str());
    i = "../output/"+ i.substr(9, i.length() - 9);
    output.NewFile((i).c_str());
    while (!input.IsEOF()) {
      length = input.ReadUnit(buf_in, n_hop * n_channel);
      process.stft(buf_in, length, data);

      /* Run Process here */

      process.istft(data, buf_out);
      output.Append(buf_out, n_hop * n_channel);
    }
    output.Finish();
    input.Finish();
  }
  
  end = clock();
  result = (double)(end - start) / CLOCKS_PER_SEC;
  printf("Process time %f sec.", result);

  for (int i = 0; i < n_channel; i++)
    delete[] data[i];
  delete[] data;

  return 0;
}
