#include <cmath>

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
    AVTXContext *cx;
    av_tx_fn tx;
    AVComplexFloat *output;
};

std::unique_ptr<FFTPlan> FFT::create(int nbits)
{
    return std::unique_ptr<FFTPlan>(new FFTPlanImpl(nbits));
}

FFTPlanImpl::FFTPlanImpl(int nbits) :
    FFTPlan(nbits),
    cx(nullptr),
    tx(nullptr),
    output(static_cast<AVComplexFloat *>(
        av_malloc(sizeof(AVComplexFloat) * this->get_output_size())
    ))
{
    av_tx_init(
        &this->cx, &this->tx, AV_TX_FLOAT_RDFT, 0,
        this->get_input_size(), nullptr, 0
    );
}

FFTPlanImpl::~FFTPlanImpl()
{
    av_tx_uninit(&this->cx);
    av_freep(&this->output);
}

void FFTPlanImpl::execute()
{
    this->tx(this->cx, this->output, this->get_input(), sizeof(float));

    // Calculate magnitudes.
    int n = this->get_input_size();
    float n2 = n * n;
    this->set_output(
        0, 10.0f * log10f(this->output[0].re * this->output[0].re / n2)
    );
    this->set_output(
        n / 2, 10.0f * log10f(
            this->output[n / 2].re * this->output[n / 2].re / n2
        )
    );
    for (int i = 1; i < n / 2; i++) {
        float re = this->output[i].re;
        float im = this->output[i].im;
        this->set_output(i, 10.0f * log10f((re * re + im * im) / n2));
    }
}
