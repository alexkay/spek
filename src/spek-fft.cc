#include <cmath>

#define __STDC_CONSTANT_MACROS
extern "C" {
#include <libavutil/tx.h>
}

#include "spek-fft.h"

class FFTPlanImpl : public FFTPlan
{
public:
    FFTPlanImpl(int nbits);
    ~FFTPlanImpl() override;

    void execute() override;

private:
    AVTXContext *tx_ctx;
    av_tx_fn tx_fn;
    AVComplexFloat *complex_out;
};

std::unique_ptr<FFTPlan> FFT::create(int nbits)
{
    return std::unique_ptr<FFTPlan>(new FFTPlanImpl(nbits));
}

FFTPlanImpl::FFTPlanImpl(int nbits) :
    FFTPlan(nbits),
    tx_ctx(nullptr),
    tx_fn(nullptr),
    complex_out(nullptr)
{
    float scale = 1.0f;
    av_tx_init(&this->tx_ctx, &this->tx_fn, AV_TX_FLOAT_RDFT, 0, 1 << nbits, &scale, 0);
    this->complex_out = (AVComplexFloat *) av_malloc(sizeof(AVComplexFloat) * ((1 << (nbits - 1)) + 1));
}

FFTPlanImpl::~FFTPlanImpl()
{
    av_tx_uninit(&this->tx_ctx);
    av_freep(&this->complex_out);
}

void FFTPlanImpl::execute()
{
    this->tx_fn(this->tx_ctx, this->complex_out, this->get_input(), sizeof(float));

    // Calculate magnitudes.
    int n = this->get_input_size();
    float n2 = (float)n * n;
    this->set_output(0, 10.0f * log10f(this->complex_out[0].re * this->complex_out[0].re / n2));
    this->set_output(n / 2, 10.0f * log10f(this->complex_out[n / 2].re * this->complex_out[n / 2].re / n2));
    for (int i = 1; i < n / 2; i++) {
        float re = this->complex_out[i].re;
        float im = this->complex_out[i].im;
        this->set_output(i, 10.0f * log10f((re * re + im * im) / n2));
    }
}
