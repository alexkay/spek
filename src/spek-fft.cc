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
    struct AVTXContext *cx;
    av_tx_fn tx;
    float* tmp;
    const int len;
};

std::unique_ptr<FFTPlan> FFT::create(int nbits)
{
    return std::unique_ptr<FFTPlan>(new FFTPlanImpl(nbits));
}

FFTPlanImpl::FFTPlanImpl(int nbits) : FFTPlan(nbits), len(1 << nbits)
{
    const float scale = 1.0;
    av_tx_init(&this->cx, &this->tx, AV_TX_FLOAT_RDFT, 0, this->len, &scale, 0);
    this->tmp = (float*) av_malloc((this->len + 2) * sizeof(float));
}

FFTPlanImpl::~FFTPlanImpl()
{
    av_tx_uninit(&this->cx);
    av_freep(&this->tmp);
}

void FFTPlanImpl::execute()
{
    this->tx(this->cx, this->tmp, this->get_input(), sizeof(AVComplexFloat));

    // Calculate magnitudes.
    int n = this->get_input_size();
    float n2 = n * n;
    this->set_output(0, 10.0f * log10f(this->tmp[0] * this->tmp[0] / n2));
    for (int i = 1; i < n / 2; i++) {
        float re = this->tmp[i * 2];
        float im = this->tmp[i * 2 + 1];
        this->set_output(i, 10.0f * log10f((re * re + im * im) / n2));
    }
    this->set_output(n / 2, 10.0f * log10f(this->tmp[this->len] * this->tmp[this->len] / n2));
}
