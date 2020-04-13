/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef ExtendedExtendedScalarImageKmeansImageFilter_h
#define ExtendedExtendedScalarImageKmeansImageFilter_h


#include "itkKdTree.h"
#include "itkKdTreeBasedKmeansEstimator.h"
#include "itkWeightedCentroidKdTreeGenerator.h"

#include "itkSampleClassifierFilter.h"
#include "itkImageToListSampleAdaptor.h"
#include "itkMinimumDecisionRule.h"

#include "itkRegionOfInterestImageFilter.h"

#include <vector>

template< typename TInputImage,
          typename TOutputImage = itk::Image< unsigned char, TInputImage::ImageDimension > >
class ExtendedScalarImageKmeansImageFilter:
  public itk::ImageToImageFilter< TInputImage, TOutputImage >
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(ExtendedScalarImageKmeansImageFilter);

  /** Extract dimension from input and output image. */
  static constexpr unsigned int ImageDimension = TInputImage::ImageDimension;

  /** Convenient type alias for simplifying declarations. */
  using InputImageType = TInputImage;
  using OutputImageType = TOutputImage;

  /** Standard class type aliases. */
  using Self = ExtendedScalarImageKmeansImageFilter;
  using Superclass = itk::ImageToImageFilter< InputImageType, OutputImageType >;
  using Pointer = itk::SmartPointer< Self >;
  using ConstPointer = itk::SmartPointer< const Self >;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ExtendedScalarImageKmeansImageFilter, ImageToImageFilter);

  /** Image type alias support */
  using InputPixelType = typename InputImageType::PixelType;
  using OutputPixelType = typename OutputImageType::PixelType;

  /** Type used for representing the Mean values. */
  using RealPixelType = typename itk::NumericTraits< InputPixelType >::RealType;

  /** Create a List from the scalar image. */
  using AdaptorType = itk::Statistics::ImageToListSampleAdaptor< InputImageType >;

  /** Define the Measurement vector type from the AdaptorType. */
  using MeasurementVectorType = typename AdaptorType::MeasurementVectorType;

  using MembershipFunctionType = itk::Statistics::DistanceToCentroidMembershipFunction< MeasurementVectorType >;
  using ClassifierType = itk::Statistics::SampleClassifierFilter< AdaptorType >;
  using DecisionRuleType = itk::Statistics::MinimumDecisionRule;

  using ClassLabelVectorType = typename ClassifierType::ClassLabelVectorType;

  using MembershipFunctionVectorType = typename ClassifierType::MembershipFunctionVectorType;
  using MembershipFunctionOriginType = typename MembershipFunctionType::CentroidType;

  using MembershipFunctionPointer = typename MembershipFunctionType::Pointer;

  /** Create the K-d tree structure. */
  using TreeGeneratorType = itk::Statistics::WeightedCentroidKdTreeGenerator< AdaptorType >;
  using TreeType = typename TreeGeneratorType::KdTreeType;
  using EstimatorType = itk::Statistics::KdTreeBasedKmeansEstimator< TreeType >;

  using ParametersType = typename EstimatorType::ParametersType;

  using ImageRegionType = typename InputImageType::RegionType;

  using RegionOfInterestFilterType = itk::RegionOfInterestImageFilter<
    InputImageType,
    InputImageType  >;

  /** Add a new class to the classification by specifying its initial mean. */
  void AddClassWithInitialMean(RealPixelType mean);
  void ClearInitialMeans( );

  /** Return the array of Means found after the classification. */
  itkGetConstReferenceMacro(FinalMeans, ParametersType);

  /** Set/Get the UseNonContiguousLabels flag. When this is set to false the
   * labels are numbered contiguously, like in {0,1,3..N}. When the flag is set
   * to true, the labels are selected in order to span the dynamic range of the
   * output image. This last option is useful when the output image is intended
   * only for display. The default value is false. */
  itkSetMacro(UseNonContiguousLabels, bool);
  itkGetConstReferenceMacro(UseNonContiguousLabels, bool);
  itkBooleanMacro(UseNonContiguousLabels);

  /** Set Region method to constrain classfication to a certain region */
  void SetImageRegion(const ImageRegionType & region);

  /** Get the region over which the statistics will be computed */
  itkGetConstReferenceMacro(ImageRegion, ImageRegionType);

#ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
  itkConceptMacro( InputHasNumericTraitsCheck,
                   ( itk::Concept::HasNumericTraits< InputPixelType > ) );
  // End concept checking
#endif

protected:
  ExtendedScalarImageKmeansImageFilter();
  ~ExtendedScalarImageKmeansImageFilter() override = default;
  void PrintSelf(std::ostream & os, itk::Indent indent) const override;

  /** This method runs the statistical methods that identify the means of the
   * classes and the use the distances to those means in order to label the
   * image pixels.
   * \sa ImageToImageFilter::GenerateData()
   */
  void GenerateData() override;

  /* See superclass for doxygen. This methods additionally checks that
   * the number of means is not 0. */
  void VerifyPreconditions() ITKv5_CONST override;

private:
  using MeansContainer = std::vector< RealPixelType >;

  MeansContainer m_InitialMeans;

  ParametersType m_FinalMeans;

  bool m_UseNonContiguousLabels{ false };

  ImageRegionType m_ImageRegion;

  bool m_ImageRegionDefined{ false };
};

#include "ExtendedScalarImageKmeansImageFilter.hxx"

#endif

