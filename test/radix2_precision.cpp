typedef double float_type;

#include "bench_helper.h"

#ifdef HAVE_FFTW
#include "bench_helper_fftw.h"
#endif

#include <metaFFT/radix2.h>
#include <metaFFT/radix2_kernel.h>
#include <metaFFT/radix2_complex.h>
#include <metaFFT/radix2_ctran.h>


template<int Sign, typename Transform, typename Cleanup>
void test(int N, const char* name, Transform transform, Cleanup cleanup)
{
    Data d;
    d.N = N;
    d.in = 0;
    d.out = 0;

    transform(&d);

    printf(" %3d  | %5d | %1.2e | %s\n", Sign, N, impulse_error(d, Sign), name);

    cleanup(&d);
}


template<int Sign, typename Func>
void test(int N, const char* name)
{
     test<Sign>(N, name, metaFFT_transform<Func, Sign, Allocator<float_type>>, &metaFFT_clean<Allocator<float_type>>);
}


template<int Sign, int N>
void test()
{
#ifdef HAVE_FFTW
    test<Sign>(N, "FFTW ", &fftw_transform<Sign>, &fftw_clean);
#endif

    using metaFFT::radix2::in_place::fft;

    {
        using namespace metaFFT::radix2::std_complex;
        using namespace metaFFT::radix2::std_complex::unrolled_loop;
        test<Sign, fft<N, std::complex<float_type>, bit_reverse_policy, butterfly_policy>>(N, "std+unrolled  ");
    }

    {
        using namespace metaFFT::radix2::std_complex;
        using namespace metaFFT::radix2::std_complex::loop;
        test<Sign, fft<N, std::complex<float_type>, bit_reverse_policy, butterfly_policy>>(N, "std+loop      ");
    }

    {
        using namespace metaFFT::radix2::ctran;
        using namespace metaFFT::radix2::ctran::unrolled_loop;
        test<Sign, fft<N, std::complex<float_type>, bit_reverse_policy, butterfly_policy>>(N, "ctran+unrolled");
    }

    {
        using namespace metaFFT::radix2::ctran;
        using namespace metaFFT::radix2::ctran::loop;
        test<Sign, fft<N, std::complex<float_type>, bit_reverse_policy, butterfly_policy>>(N, "ctran+loop    ");
    }

    printf("\n");
}


template<int N>
void test()
{
    test<+1, N>();
    test<-1, N>();
}


int main(int, char *[])
{
    printf(" Sign |      Size |     L2 Error\n");
    printf("------+-----------+-------------\n");

    test<   4>();
    test<   8>();
    test<  16>();
    test<  32>();
    test<  64>();
    test< 128>();
    test< 256>();

#if 0 || defined(LARGE_FFTS)
    test< 512>();
    test<1024>();
    test<2048>();
    test<2048*2>();
#endif

    return 0;
}
