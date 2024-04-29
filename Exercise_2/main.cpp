#include <iostream>
#include "PolygonalMesh.hpp"
#include "Utils.hpp"

using namespace std;
using namespace PolygonalLibrary;

int main()
{
    PolygonalMesh mesh;

    string filePath = "PolygonalMesh";
    if(!importMesh(filePath, mesh))
    {
        return 1;
    }

    return 0;
}
