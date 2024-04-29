#pragma once

#include <iostream>
#include <vector>
#include "PolygonalMesh.hpp"

using namespace std;

namespace PolygonalLibrary{

bool importMesh(const string& filePath,
                PolygonalMesh& mesh);



bool importCell0Ds(const string& fileName,
                   PolygonalMesh& mesh);

bool importCell1Ds(const string& fileName,
                   PolygonalMesh& mesh);

bool importCell2Ds(const string& fileName,
                   PolygonalMesh& mesh);

}
