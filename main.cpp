#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>

int main(int argc, char** argv)
{
  int ArgCount = 1;

  std::string InputName = argv[ArgCount++];
  float threshold = atof(argv[ArgCount++]); 
  std::string OutputName = argv[ArgCount++];

  std::ifstream ifm;
  ifm.open(InputName.c_str());

  if(!ifm.good())
    {
      std::cerr << "Cannot open " << InputName << std::endl;
      return 0;
    }

  const int BufSize = 256;
  char LineBuf[BufSize];

  int NumOfVertices = 0;
  int NumOfFaces = 0;

  while(true)
    {
      memset(LineBuf, 0, BufSize);
      ifm.getline(LineBuf, BufSize);

      std::string line = LineBuf;
      
      if(line.compare("end_header")==0)
	break;
      
      std::string affix = line.substr(0, 14);

      if(affix.compare("element vertex")==0)
	{
	  std::string NumOfVertices_str = line.substr(15);
	  NumOfVertices = atoi(NumOfVertices_str.c_str());
	}

      affix = line.substr(0, 12);
      if(affix.compare("element face")==0)
	{
	  std::string NumOfFaces_str = line.substr(13);
	  NumOfFaces = atoi(NumOfFaces_str.c_str());
	}
    }
  
  float* vertex = new float[NumOfVertices*3];
  int* face = new int[NumOfFaces*3];

  char face_c;

  // Read vertices
  ifm.read((char*)vertex, NumOfVertices*3*sizeof(float));

  int FaceSize = 3*sizeof(int);

  // Read faces
  for(int i=0; i<NumOfFaces; i++)
    {
      ifm.get(face_c);

      if(face_c!=3)
	{
	  std::cout << "Dude, this is not a triangle!!!" << std::endl;
	  break;
	}

      ifm.read((char*)(&face[i*3]), FaceSize);
    }
  
  ifm.close();

  // Now let's kill some triangles
  int* neo_face = new int[NumOfFaces*3];

  threshold *= threshold;

  float v1[3], v2[3], v3[3];
  float e1 = 0;
  float e2 = 0; 
  float e3 = 0;
  int idx = 0;
  int neo_face_count = 0;

  for(int i=0; i<NumOfFaces; i++)
    {
      
      v1[0] = vertex[face[idx  ]*3  ];
      v1[1] = vertex[face[idx  ]*3+1];
      v1[2] = vertex[face[idx  ]*3+2];

      v2[0] = vertex[face[idx+1]*3  ];
      v2[1] = vertex[face[idx+1]*3+1];
      v2[2] = vertex[face[idx+1]*3+2];

      v3[0] = vertex[face[idx+2]*3  ];
      v3[1] = vertex[face[idx+2]*3+1];
      v3[2] = vertex[face[idx+2]*3+2];

      e1 = ( (v1[0]-v2[0])*(v1[0]-v2[0]) +
      	     (v1[1]-v2[1])*(v1[1]-v2[1]) + 
      	     (v1[2]-v2[2])*(v1[2]-v2[2]) );

      e2 = ( (v1[0]-v3[0])*(v1[0]-v3[0]) +
      	     (v1[1]-v3[1])*(v1[1]-v3[1]) + 
      	     (v1[2]-v3[2])*(v1[2]-v3[2]) );

      e3 = ( (v2[0]-v3[0])*(v2[0]-v3[0]) +
      	     (v2[1]-v3[1])*(v2[1]-v3[1]) + 
      	     (v2[2]-v3[2])*(v2[2]-v3[2]) );

      if(e1<threshold && e2<threshold && e3<threshold)
	{
	  neo_face[neo_face_count++] = face[idx  ];
	  neo_face[neo_face_count++] = face[idx+1];
	  neo_face[neo_face_count++] = face[idx+2];
	}

      idx += 3;

    }

  // Write results to output file
  std::ofstream ofm;
  ofm.open(OutputName.c_str(), std::ios::trunc);

  ofm << "ply"                                           << std::endl;
  ofm << "format binary_little_endian 1.0"               << std::endl;
  ofm << "element vertex "  << NumOfVertices             << std::endl;
  ofm << "property float x"                              << std::endl;
  ofm << "property float y"                              << std::endl;
  ofm << "property float z"                              << std::endl;
  ofm << "element face " << (int)(neo_face_count/3)      << std::endl;
  ofm << "property list uchar int vertex_indices"        << std::endl;
  ofm << "end_header"                                    << std::endl;

  // Write vertex
  ofm.write((char*)vertex, NumOfVertices*3*sizeof(float));
  
  // std::cout << neo_face_count/3 << " " << NumOfFaces << std::endl;

  // Write faces
  face_c = 3;
  for(int i=0; i<neo_face_count/3; i++)
    {
      ofm.write((char*)&face_c, 1);
      
      ofm.write((char*)(&neo_face[i*3]), FaceSize);
    }

  ofm.close();

  delete[] vertex;
  delete[] face;
  delete[] neo_face;
  
  return 0;
}
