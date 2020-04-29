#include <iostream>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
//Librerias nuevas para el KD tree
#include <itkKdTree.h>
#include "itkImageToListSampleAdaptor.h"
#include <itkScalarImageKmeansImageFilter.h>
#include "itkWeightedCentroidKdTreeGenerator.h"
#include "itkImageToListSampleAdaptor.h"
#include "itkImage.h"
#include "itkRandomImageSource.h"
#include "itkComposeImageFilter.h"
//---------------------------------------

#include "ExtendedScalarImageKmeansImageFilter.h"

const unsigned int Dim = 2;
typedef short TPixel;
typedef unsigned char TLabel;

typedef itk::Image<TPixel, Dim> TImage;
typedef itk::Image<TLabel, Dim> TLabels;
typedef itk::Array<double> TArray;
typedef ExtendedScalarImageKmeansImageFilter<TImage, TLabels> TKmeans;
typedef itk::ImageFileReader<TImage> TReader;

//KD-tree
using MeasurementVectorType = itk::FixedArray<float, 1>;
using ArrayImageType = itk::Image<MeasurementVectorType, 2>;
using CasterType = itk::ComposeImageFilter<TLabels, ArrayImageType>;
using SampleType = itk::Statistics::ImageToListSampleAdaptor<TLabels>;
using MeasurementVectorType = itk::FixedArray<float, 1>;
using ArrayImageType = itk::Image<MeasurementVectorType, 2>;
using CasterType = itk::ComposeImageFilter<TLabels, ArrayImageType>;
typedef itk::Statistics::KdTreeGenerator<SampleType> TreeGeneratorType;
typedef TreeGeneratorType::KdTreeType TreeType;
typedef TreeType::KdTreeNodeType NodeType;

void inorder(NodeType *n)
{
  if(n->IsTerminal())
  {
    NodeType::CentroidType centroid;
     n->GetWeightedCentroid( centroid );
     for(auto t: centroid)
      std::cout<<t<<",";
    return ;
  }
   inorder( n->Left( ) );
  
 
    
 
 
  inorder( n->Right( ) );
}


int main(int argc, char *argv[])
{
  //Validación de argumentos
  if (argc < 4)
  {
    std::cerr << "Usage: " << argv[0] << " <input> <output> k1 k2 k3 ..." << std::endl;
    return -1;
  }
  std::string input_filename = argv[1];
  std::string output_filename = argv[2];

  //create the pointer
  TReader::Pointer reader = TReader::New();
  //set the file to get it like a image
  reader->SetFileName(input_filename);

  //Aplica la segmentación K-means
  TKmeans::Pointer kmeans = TKmeans::New();
  TKmeans::ParametersType Means;
  kmeans->SetInput(reader->GetOutput());
  //Add means to the filter
  for (int i = 3; i < argc; ++i)
  {
    kmeans->AddClassWithInitialMean(std::atof(argv[i]));
  }

  //------------------------------------------------------------------------------------
  //------------------------------   Paso del filtro al KD-tree  -----------------------
  //------------------------------------------------------------------------------------
  //Paso del k-means al sample

  CasterType::Pointer caster = CasterType::New();
  caster->SetInput(kmeans->GetOutput());
  caster->Update();

  SampleType::Pointer sample = SampleType::New();

  sample->SetImage(kmeans->GetOutput());

  //KD-tree
  TreeGeneratorType::Pointer treeGenerator = TreeGeneratorType::New();

  treeGenerator->SetSample(sample);
  treeGenerator->SetBucketSize(16);
  treeGenerator->Update();

  using CentroidTreeGeneratorType =
      itk::Statistics::WeightedCentroidKdTreeGenerator<SampleType>;

  CentroidTreeGeneratorType::Pointer centroidTreeGenerator =
      CentroidTreeGeneratorType::New();

  centroidTreeGenerator->SetSample(sample);
  centroidTreeGenerator->SetBucketSize(16);
  centroidTreeGenerator->Update();

  TreeType::Pointer tree = treeGenerator->GetOutput();
  TreeType::Pointer centroidTree = centroidTreeGenerator->GetOutput();

  NodeType *root = tree->GetRoot( );

  


  unsigned int partitionDimension;
  NodeType::MeasurementType partitionValue;
  root->GetParameters(partitionDimension, partitionValue);

  //----------------------------------------------------------
  //----------------------------------------------------------
  //----------------------------------------------------------

  //Escribe los resultados en memoria
  typedef ::itk::ImageFileWriter<TLabels> TWriter;
  TWriter::Pointer writer = TWriter::New();
  writer->SetInput(kmeans->GetOutput());
  writer->SetFileName(output_filename);
  try
  {
    writer->Update();
    Means = kmeans->GetFinalMeans();
    std::cout << "get numbers of elements " << kmeans->GetFinalMeans().GetNumberOfElements() << std::endl;
    for (int i = 0; i < 3; i++)
      std::cout << Means[i] << std::endl;
  }
  catch (std::exception &err)
  {
    std::cerr << "Error caught: " << err.what() << std::endl;
    return (EXIT_FAILURE);
  }

  if (root->IsTerminal())
  {
    std::cout << "Root node is a terminal node." << std::endl;
  }
  else
  {
    
    std::cout << "Root node is not a terminal node." << std::endl;
    inorder( root );
  }

  return (EXIT_SUCCESS);
}