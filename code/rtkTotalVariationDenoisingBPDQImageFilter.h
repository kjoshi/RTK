#ifndef __rtkTotalVariationDenoisingBPDQImageFilter_h
#define __rtkTotalVariationDenoisingBPDQImageFilter_h

#include <itkImageToImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkImage.h>
#include <itkSubtractImageFilter.h>
#include <itkMultiplyImageFilter.h>

#include "rtkForwardDifferenceGradientImageFilter.h"
#include "rtkBackwardDifferenceDivergenceImageFilter.h"
#include "rtkMagnitudeThresholdImageFilter.h"

namespace rtk
{
/** \class TotalVariationDenoisingBPDQImageFilter
 * \brief Applies a total variation denoising, only along the dimensions specified, on an image.
 *
 * This filter finds the minimum of lambda * || f - f_0 ||_2^2 + TV(f)
 * using basis pursuit dequantaization, where f is the current image, f_0 the
 * input image, and TV the total variation calculated with only the gradients
 * along the dimensions specified. This filter can be used, for example, to
 * perform 3D total variation denoising on a 4D dataset
 * (by calling SetDimensionsProcessed([true true true false]).
 * More information on the algorithm can be found at
 * http://wiki.epfl.ch/bpdq#download
 *
 * \dot
 * digraph TotalVariationDenoisingBPDQImageFilter {
 *
 * Input [label="Input"];
 * Input [shape=Mdiamond];
 * Output [label="Output"];
 * Output [shape=Mdiamond];
 *
 * node [shape=box];
 * ZeroMultiply [ label="itk::MultiplyImageFilter (by zero)" URL="\ref itk::MultiplyImageFilter"];
 * ZeroGradient [ label="rtk::ForwardDifferenceGradientImageFilter" URL="\ref rtk::ForwardDifferenceGradientImageFilter"];
 * Divergence [ label="rtk::BackwardDifferenceDivergenceImageFilter" URL="\ref rtk::BackwardDifferenceDivergenceImageFilter"];
 * Subtract [ label="itk::SubtractImageFilter" URL="\ref itk::SubtractImageFilter"];
 * Multiply [ label="itk::MultiplyImageFilter (by beta)" URL="\ref itk::MultiplyImageFilter"];
 * Gradient [ label="rtk::ForwardDifferenceGradientImageFilter" URL="\ref rtk::ForwardDifferenceGradientImageFilter"];
 * SubtractGradient [ label="itk::SubtractImageFilter" URL="\ref itk::SubtractImageFilter"];
 * MagnitudeThreshold [ label="rtk::rtkMagnitudeThresholdImageFilter" URL="\ref rtk::rtkMagnitudeThresholdImageFilter"];
 * OutOfZeroGradient [label="", fixedsize="false", width=0, height=0, shape=none];
 *
 * Input -> ZeroMultiply;
 * Input -> Subtract;
 * ZeroMultiply -> ZeroGradient;
 * ZeroGradient -> OutOfZeroGradient [arrowhead=None];
 * OutOfZeroGradient -> Divergence;
 * OutOfZeroGradient -> SubtractGradient;
 * Divergence -> Subtract;
 * Subtract -> Multiply;
 * Multiply -> Gradient;
 * Gradient -> SubtractGradient;
 * SubtractGradient -> MagnitudeThreshold;
 * MagnitudeThreshold -> OutOfZeroGradient [style=dashed];
 * Subtract -> Output;
 * }
 * \enddot
 *
 * \author Cyril Mory
 *
 * \ingroup IntensityImageFilters
 */
template <class TInputImage>
class TotalVariationDenoisingBPDQImageFilter :
        public itk::ImageToImageFilter< TInputImage, TInputImage >
{
public:
    /** Extract dimension from input and output image. */
    itkStaticConstMacro(InputImageDimension, unsigned int,
                        TInputImage::ImageDimension);

    /** Convenient typedefs for simplifying declarations. */
    typedef TInputImage InputImageType;

    /** Standard class typedefs. */
    typedef TotalVariationDenoisingBPDQImageFilter Self;
    typedef itk::ImageToImageFilter< InputImageType, InputImageType> Superclass;
    typedef itk::SmartPointer<Self> Pointer;
    typedef itk::SmartPointer<const Self>  ConstPointer;

    /** Method for creation through the object factory. */
    itkNewMacro(Self)

    /** Run-time type information (and related methods). */
    itkTypeMacro(TotalVariationDenoisingBPDQImageFilter, ImageToImageFilter)

    /** Image typedef support. */
    typedef typename InputImageType::PixelType InputPixelType;
    typedef typename InputImageType::RegionType InputImageRegionType;
    typedef typename InputImageType::SizeType InputSizeType;

    /** Sub filter type definitions */
    typedef ForwardDifferenceGradientImageFilter<TInputImage> GradientFilterType;
    typedef typename GradientFilterType::OutputImageType GradientImageType;
    typedef itk::MultiplyImageFilter<TInputImage> MultiplyFilterType;
    typedef itk::SubtractImageFilter<TInputImage> SubtractImageFilterType;
    typedef itk::SubtractImageFilter<GradientImageType> SubtractGradientFilterType;
    typedef MagnitudeThresholdImageFilter<GradientImageType> MagnitudeThresholdFilterType;
    typedef BackwardDifferenceDivergenceImageFilter<GradientImageType> DivergenceFilterType;

    itkGetMacro(NumberOfIterations, int)
    itkSetMacro(NumberOfIterations, int)

    itkSetMacro(Lambda, double)
    itkGetMacro(Lambda, double)

    void SetDimensionsProcessed(bool* arg);

    protected:
        TotalVariationDenoisingBPDQImageFilter();
    virtual ~TotalVariationDenoisingBPDQImageFilter() {}

    virtual void GenerateData();

    virtual void GenerateOutputInformation();

    /** Sub filter pointers */
    typename GradientFilterType::Pointer             m_GradientFilter;
    typename GradientFilterType::Pointer             m_ZeroGradientFilter;
    typename MultiplyFilterType::Pointer             m_MultiplyFilter;
    typename MultiplyFilterType::Pointer             m_ZeroMultiplyFilter;
    typename SubtractImageFilterType::Pointer        m_SubtractFilter;
    typename SubtractGradientFilterType::Pointer     m_SubtractGradientFilter;
    typename MagnitudeThresholdFilterType::Pointer   m_MagnitudeThresholdFilter;
    typename DivergenceFilterType::Pointer           m_DivergenceFilter;

    double m_Lambda;
    int m_NumberOfIterations;
    bool* m_DimensionsProcessed;

private:
    TotalVariationDenoisingBPDQImageFilter(const Self&); //purposely not implemented
    void operator=(const Self&); //purposely not implemented

    double m_beta;
    double m_gamma;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "rtkTotalVariationDenoisingBPDQImageFilter.txx"
#endif

#endif //__rtkTotalVariationDenoisingBPDQImageFilter__
