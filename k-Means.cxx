#include <iostream>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include "ExtendedScalarImageKmeansImageFilter.h"

const unsigned int  Dim = 2;
typedef short TPixel ;
typedef unsigned char TLabel; 

typedef itk::Image< TPixel, Dim > TImage;
typedef itk::Image< TLabel, Dim > TLabels;
typedef itk::Array<double> TArray;
int main(int argc, char* argv[])
{
	
  if(argc < 4)
  {
    std::cerr
      << "Usage: " << argv[ 0 ] << "input output k1 k2 k3 ..." << std::endl;
			
  }
  std::string input_filename = argv[ 1 ];
  std::string output_filename = argv[ 2 ];

  //we set the image filtrer reder as TReader
  typedef itk::ImageFileReader< TImage> TReader;
  //create the pointer 
  TReader::Pointer reader = TReader::New( );
  //set the file to get it like a image 
  reader->SetFileName( input_filename );

  typedef ExtendedScalarImageKmeansImageFilter< TImage , TLabels> TKmeans;
  TKmeans::Pointer kmeans = TKmeans::New( );
  TKmeans::ParametersType  Means ;
  kmeans->SetInput( reader->GetOutput( ) );
  for( int i = 3; i < argc; ++i )
    kmeans->AddClassWithInitialMean( std::atof( argv[ i ] ) );
	 
	

  typedef:: itk::ImageFileWriter< TLabels > TWriter;
  TWriter::Pointer writer = TWriter::New( );
  writer->SetInput(  kmeans->GetOutput( ) );
  writer->SetFileName( output_filename );
  int counter = 3;
  do
  {
    try
    {
      writer->Update( );
      Means = kmeans->GetFinalMeans();
      std::cout<<"get numbers of elements "<<kmeans->GetFinalMeans( ).GetNumberOfElements( )<<std::endl;
      for(int i = 0 ; i<3 ; i++)
        std::cout<<Means[i]<<std::endl;

    } 
    catch(std::exception& err)
    {
      std::cerr<<"Error caught: "<<err.what( )<<std::endl;
      return(EXIT_FAILURE);
    }
    kmeans->ClearInitialMeans( );
    for( int i = 3; i < argc; ++i )
      kmeans->AddClassWithInitialMean( Means[i] ) ;	
    counter--; 
  } while (counter > 0);
	 	
	 


  return(EXIT_SUCCESS);
}
